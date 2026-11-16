#include "gk327_fixed6502_rng.h"
#include <string.h>
#include "gk326_sha256.h"

/* No emulated PPU, input or timer: deliberately source-bounded CPU execution
 * of a fixed-bank routine. All memory and output are local until final commit.
 * This is original *instruction execution*, not source event authentication. */
typedef struct {
 const uint8_t *rom;
 uint8_t zp[256],a,x,y,c,z,n;
 uint16_t pc;
 uint32_t instructions;
} Cpu;
static uint8_t flags(Cpu *c,uint8_t v){c->z=(uint8_t)(v==0);c->n=(uint8_t)(v>>7);return v;}
static int read8(Cpu *c,uint16_t addr,uint8_t *out){
 if(addr<256u){*out=c->zp[addr];return 1;}
 if(addr>=0xc000u){*out=c->rom[GK327_PRG_BANK15_START+(unsigned)(addr-0xc000u)];return 1;}
 return 0;
}
static int write8(Cpu *c,uint16_t addr,uint8_t v){
 if(addr>=256u)return 0; /* refuse untraced PPU/SRAM/IO writes */
 c->zp[addr]=v;return 1;
}
static int next8(Cpu *c,uint8_t *out){
 if(c->pc<0xc000u||c->pc==0xffffu)return 0;
 if(!read8(c,c->pc,out))return 0;
 ++c->pc;return 1;
}
static int next16(Cpu *c,uint16_t *out){
 uint8_t lo,hi;
 if(!next8(c,&lo)||!next8(c,&hi))return 0;
 *out=(uint16_t)((uint16_t)lo|((uint16_t)hi<<8));return 1;
}
static uint8_t adc(Cpu *c,uint8_t lhs,uint8_t rhs){
 unsigned sum=(unsigned)lhs+(unsigned)rhs+(unsigned)c->c;
 c->c=(uint8_t)(sum>255u);
 return flags(c,(uint8_t)sum);
}
static int branch(Cpu *c,uint8_t when){
 uint8_t rel;
 if(!next8(c,&rel))return 0;
 if(when)c->pc=(uint16_t)(c->pc+(int8_t)rel);
 return c->pc>=0xc000u;
}
static Gk327Status run(const uint8_t *image,size_t n,uint16_t entry,
 const uint8_t seed[6],uint32_t max_steps,Gk327RngReceipt *out,
 int authentic){
 if(!image||!seed||!out||!max_steps||max_steps>GK327_RNG_MAX_STEPS)
  return GK327_BAD_ARG;
 if(n!=GK327_ROM_BYTES||memcmp(image,"NES\x1a",4)!=0||image[4]!=16u||
    image[5]!=0u||image[6]!=0x12u||image[7]!=0u)
  return GK327_BAD_ROM;
 if(entry<0xc000u||entry==0xffffu)return GK327_BAD_PC;
 Cpu c={0};c.rom=image;c.pc=entry;
 for(unsigned i=0;i<6;i++)c.zp[0x84u+i]=seed[i];
 for(uint32_t i=0;i<max_steps;i++){
  uint8_t op,imm;uint16_t addr;uint8_t value;
  if(!next8(&c,&op))return GK327_BAD_PC;
  c.instructions++;
  switch(op){
   case 0xad: /* LDA absolute */
    if(!next16(&c,&addr)||!read8(&c,addr,&value))return GK327_BAD_PC;
    c.a=flags(&c,value);break;
   case 0xa9: /* LDA immediate */
    if(!next8(&c,&imm))return GK327_BAD_PC;
    c.a=flags(&c,imm);break;
   case 0xa0: /* LDY immediate */
    if(!next8(&c,&imm))return GK327_BAD_PC;
    c.y=flags(&c,imm);break;
   case 0xa2: /* LDX immediate */
    if(!next8(&c,&imm))return GK327_BAD_PC;
    c.x=flags(&c,imm);break;
   case 0x09: /* ORA immediate */
    if(!next8(&c,&imm))return GK327_BAD_PC;
    c.a=flags(&c,(uint8_t)(c.a|imm));break;
   case 0x29: /* AND immediate */
    if(!next8(&c,&imm))return GK327_BAD_PC;
    c.a=flags(&c,(uint8_t)(c.a&imm));break;
   case 0x49: /* EOR immediate */
    if(!next8(&c,&imm))return GK327_BAD_PC;
    c.a=flags(&c,(uint8_t)(c.a^imm));break;
   case 0x18:c.c=0u;break; /* CLC */
   case 0x38:c.c=1u;break; /* SEC */
   case 0x69: /* ADC immediate */
    if(!next8(&c,&imm))return GK327_BAD_PC;
    c.a=adc(&c,c.a,imm);break;
   case 0x7d: /* ADC absolute,X */
    if(!next16(&c,&addr))return GK327_BAD_PC;
    if(!read8(&c,(uint16_t)(addr+c.x),&value))return GK327_BAD_PC;
    c.a=adc(&c,c.a,value);break;
   case 0xb9: /* LDA absolute,Y */
    if(!next16(&c,&addr))return GK327_BAD_PC;
    if(!read8(&c,(uint16_t)(addr+c.y),&value))return GK327_BAD_PC;
    c.a=flags(&c,value);break;
   case 0xbd: /* LDA absolute,X */
    if(!next16(&c,&addr))return GK327_BAD_PC;
    if(!read8(&c,(uint16_t)(addr+c.x),&value))return GK327_BAD_PC;
    c.a=flags(&c,value);break;
   case 0x8d: /* STA absolute */
    if(!next16(&c,&addr))return GK327_BAD_PC;
    if(!write8(&c,addr,c.a))return GK327_BAD_WRITE;
    break;
   case 0x99: /* STA absolute,Y */
    if(!next16(&c,&addr))return GK327_BAD_PC;
    if(!write8(&c,(uint16_t)(addr+c.y),c.a))return GK327_BAD_WRITE;
    break;
   case 0x9d: /* STA absolute,X */
    if(!next16(&c,&addr))return GK327_BAD_PC;
    if(!write8(&c,(uint16_t)(addr+c.x),c.a))return GK327_BAD_WRITE;
    break;
   case 0x3e: /* ROL absolute,X: carry in, carry out */
    if(!next16(&c,&addr))return GK327_BAD_PC;
    addr=(uint16_t)(addr+c.x);
    if(!read8(&c,addr,&value))return GK327_BAD_PC;
    imm=(uint8_t)(value>>7);
    value=flags(&c,(uint8_t)((value<<1)|(unsigned)c.c));c.c=imm;
    if(!write8(&c,addr,value))return GK327_BAD_WRITE;
    break;
   case 0x0e: /* ASL absolute */
    if(!next16(&c,&addr)||!read8(&c,addr,&value))return GK327_BAD_PC;
    c.c=(uint8_t)(value>>7);
    if(!write8(&c,addr,flags(&c,(uint8_t)(value<<1))))return GK327_BAD_WRITE;
    break;
   case 0xce: /* DEC absolute */
    if(!next16(&c,&addr)||!read8(&c,addr,&value))return GK327_BAD_PC;
    if(!write8(&c,addr,flags(&c,(uint8_t)(value-1u))))return GK327_BAD_WRITE;
    break;
   case 0xca:c.x=flags(&c,(uint8_t)(c.x-1u));break; /* DEX */
   case 0x88:c.y=flags(&c,(uint8_t)(c.y-1u));break; /* DEY */
   case 0x10:if(!branch(&c,(uint8_t)!c.n))return GK327_BAD_PC;break;/* BPL */
   case 0x90:if(!branch(&c,(uint8_t)!c.c))return GK327_BAD_PC;break;/* BCC */
   case 0xd0:if(!branch(&c,(uint8_t)!c.z))return GK327_BAD_PC;break;/* BNE */
   case 0x60:{ /* RTS: terminates one bounded subroutine; no fake call stack */
    Gk327RngReceipt receipt={0};
    memcpy(receipt.seed_before,seed,6);
    memcpy(receipt.seed_after,c.zp+0x84u,6);
    receipt.result_66=c.zp[0x66u];receipt.result_67=c.zp[0x67u];
    receipt.final_pc=(uint16_t)(c.pc-1u);
    receipt.instructions=c.instructions;receipt.authentic_rom=(uint8_t)authentic;
    *out=receipt;return GK327_OK;
   }
   default:return GK327_UNSUPPORTED;
  }
  if(c.pc<0xc000u)return GK327_BAD_PC;
 }
 return GK327_STEP_LIMIT;
}
Gk327Status gk327_execute_fixture(const uint8_t *image,size_t n,
 uint16_t entry_pc,const uint8_t seed[6],uint32_t step_limit,
 Gk327RngReceipt *out){
 return run(image,n,entry_pc,seed,step_limit,out,0);
}
Gk327Status gk327_execute_original_rng(const uint8_t *image,size_t n,
 const uint8_t seed[6],Gk327RngReceipt *out){
 static const uint8_t expect[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3};
 uint8_t digest[32];
 if(!image||!seed||!out)return GK327_BAD_ARG;
 if(n!=GK327_ROM_BYTES)return GK327_BAD_ROM;
 gk326_sha256(image,n,digest);
 if(memcmp(digest,expect,32)!=0)return GK327_BAD_SOURCE;
 Gk327RngReceipt temp;
 Gk327Status s=run(image,n,GK327_RNG_PC,seed,GK327_RNG_MAX_STEPS,&temp,1);
 if(s!=GK327_OK)return s;
 if(temp.final_pc!=0xc2ebu)return GK327_BAD_SOURCE;
 *out=temp;return GK327_OK;
}
