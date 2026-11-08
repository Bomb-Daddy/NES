#include "gk83_reset_trace.h"
#include "gk84_reset_hypothesis.h"
#include "gk82_stock_cartridge.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROM_BYTES 262160u
#define PRG_START 16u
#define BANK_BYTES 16384u
#define F_C 1u
#define F_Z 2u
#define F_I 4u
#define F_D 8u
#define F_V 64u
#define F_N 128u
/* The public source contains no private cartridge data: only the expected
 * SHA256 fingerprint, also verified independently by GK82. */
static const uint8_t expected_sha[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3
};
typedef struct {
 const uint8_t *rom;
 uint8_t ram[2048],known[2048],a,x,y,sp,p;
 uint8_t shift,control,prg,chr0,chr1,control_known,prg_known;
 uint16_t pc;
 Gk83Trace result;
} Machine;
static void nz(Machine *m,uint8_t v){m->p=(uint8_t)((m->p&~(F_N|F_Z))|(v?0:F_Z)|(v&F_N));}
static Gk83Stop read_byte(Machine *m,uint16_t addr,uint8_t *v){
 if(addr<0x2000u){
  unsigned at=addr&0x7ffu;
  if(!m->known[at]){m->result.first_unknown_ram_read=addr;return GK83_UNKNOWN_RAM_GATE;}
  *v=m->ram[at];return GK83_INVALID;
 }
 if(addr<0x8000u){
  if(addr>=0x2000u&&addr<0x6000u)m->result.first_hardware_read=addr;
  return GK83_HARDWARE_GATE;
 }
 unsigned bank=15u;
 if(addr<0xc000u){
  m->result.first_banked_pc=m->result.first_banked_pc?m->result.first_banked_pc:addr;
  if(!m->control_known||!m->prg_known)return GK83_BANK_GATE;
  unsigned mode=(m->control>>2u)&3u;
  if(mode==3u)bank=m->prg&15u;
  else if(mode==2u)bank=0u;
  else bank=m->prg&14u;
 }else if(m->control_known){
  unsigned mode=(m->control>>2u)&3u;
  if(mode==2u)bank=m->prg&15u;
  else if(mode<2u)bank=(m->prg&14u)+1u;
  if(mode!=3u&&!m->prg_known)return GK83_BANK_GATE;
 }
 *v=m->rom[PRG_START+bank*BANK_BYTES+(addr&0x3fffu)];
 return GK83_INVALID;
}
static void mmc1_write(Machine *m,uint16_t addr,uint8_t value){
 m->result.mapper_serial_writes++;
 if(value&0x80u){m->shift=0x10u;return;}
 unsigned full=m->shift&1u;
 m->shift=(uint8_t)((m->shift>>1u)|((value&1u)<<4u));
 if(!full)return;
 unsigned group=(addr>>13u)&3u;
 if(group==0u){m->control=m->shift;m->control_known=1;}
 else if(group==1u)m->chr0=m->shift;
 else if(group==2u)m->chr1=m->shift;
 else {if(m->prg_known&&m->prg!=(m->shift&15u))m->result.rom_bank_switches++;
       m->prg=m->shift&15u;m->prg_known=1;}
 m->result.mapper_register_commits++;
 m->shift=0x10u;
}
static void write_byte(Machine *m,uint16_t addr,uint8_t v){
 if(addr<0x2000u){unsigned at=addr&0x7ffu;
  m->ram[at]=v;m->known[at]=1u;m->result.ram_writes++;
 }else if(addr>=0x8000u)mmc1_write(m,addr,v);
 else m->result.peripheral_writes++;
}
static void push(Machine *m,uint8_t v){write_byte(m,(uint16_t)(0x100u+m->sp),v);m->sp--;}
static Gk83Stop pop(Machine *m,uint8_t *v){m->sp++;return read_byte(m,(uint16_t)(0x100u+m->sp),v);}
static Gk83Stop operand(Machine *m,uint8_t *v){Gk83Stop s=read_byte(m,m->pc,v);if(s==GK83_INVALID)m->pc++;return s;}
static Gk83Stop word(Machine *m,uint16_t *v){
 uint8_t lo,hi;Gk83Stop s=operand(m,&lo);if(s!=GK83_INVALID)return s;
 s=operand(m,&hi);if(s!=GK83_INVALID)return s;
 *v=(uint16_t)((unsigned)lo|((unsigned)hi<<8u));return GK83_INVALID;
}
static Gk83Stop branch(Machine *m,int cond){
 uint8_t rel;Gk83Stop s=operand(m,&rel);if(s!=GK83_INVALID)return s;
 if(cond)m->pc=(uint16_t)((int)m->pc+(int)(int8_t)rel);
 return GK83_INVALID;
}
static Gk83Stop one(Machine *m){
 uint8_t op,b;uint16_t addr;
 Gk83Stop s=read_byte(m,m->pc,&op);if(s!=GK83_INVALID)return s;
 m->result.stop_pc=m->pc;m->result.stop_opcode=op;m->pc++;
 switch(op){
 case 0x78:m->p|=F_I;break; /* SEI */
 case 0xd8:m->p&=(uint8_t)~F_D;break; /* CLD */
 case 0x18:m->p&=(uint8_t)~F_C;break; /* CLC */
 case 0xa9:s=operand(m,&m->a);if(!s)nz(m,m->a);break;
 case 0xa2:s=operand(m,&m->x);if(!s)nz(m,m->x);break;
 case 0xa0:s=operand(m,&m->y);if(!s)nz(m,m->y);break;
 case 0x9a:m->sp=m->x;break; /* TXS */
 case 0x8a:m->a=m->x;nz(m,m->a);break;
 case 0x98:m->a=m->y;nz(m,m->a);break;
 case 0xaa:m->x=m->a;nz(m,m->x);break;
 case 0xa8:m->y=m->a;nz(m,m->y);break;
 case 0xe8:m->x++;nz(m,m->x);break;
 case 0xca:m->x--;nz(m,m->x);break;
 case 0xc8:m->y++;nz(m,m->y);break;
 case 0x88:m->y--;nz(m,m->y);break;
 case 0x4a:m->p=(uint8_t)((m->p&~F_C)|(m->a&1u));m->a>>=1u;nz(m,m->a);break;
 case 0x48:push(m,m->a);break;
 case 0x68:s=pop(m,&m->a);if(!s)nz(m,m->a);break;
 case 0x8d:s=word(m,&addr);if(!s)write_byte(m,addr,m->a);break;
 case 0x85:s=operand(m,&b);if(!s)write_byte(m,b,m->a);break;
 case 0x9d:s=word(m,&addr);if(!s)write_byte(m,(uint16_t)(addr+m->x),m->a);break;
 case 0x99:s=word(m,&addr);if(!s)write_byte(m,(uint16_t)(addr+m->y),m->a);break;
 case 0xad:s=word(m,&addr);if(!s){s=read_byte(m,addr,&m->a);if(!s)nz(m,m->a);}break;
 case 0xae:s=word(m,&addr);if(!s){s=read_byte(m,addr,&m->x);if(!s)nz(m,m->x);}break; /* LDX abs; preserve exact unknown/IO gate. */
 case 0xa5:s=operand(m,&b);if(!s){s=read_byte(m,b,&m->a);if(!s)nz(m,m->a);}break;
 case 0xbd:s=word(m,&addr);if(!s){s=read_byte(m,(uint16_t)(addr+m->x),&m->a);if(!s)nz(m,m->a);}break;
 case 0xb9:s=word(m,&addr);if(!s){s=read_byte(m,(uint16_t)(addr+m->y),&m->a);if(!s)nz(m,m->a);}break;
 case 0xb1:{ /* LDA ($nn),Y -- 6502 wraps high pointer byte in zero page. */
  uint8_t lo,hi;s=operand(m,&b);
  if(!s)s=read_byte(m,b,&lo);
  if(!s)s=read_byte(m,(uint8_t)(b+1u),&hi);
  if(!s){addr=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+m->y);
   s=read_byte(m,addr,&m->a);if(!s)nz(m,m->a);}
  break;}

 case 0xc0:s=operand(m,&b);if(!s){unsigned diff=(unsigned)m->y-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->y>=b?F_C:0));nz(m,(uint8_t)diff);}break;
 case 0xe0:s=operand(m,&b);if(!s){unsigned diff=(unsigned)m->x-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->x>=b?F_C:0));nz(m,(uint8_t)diff);}break;
 case 0xcd:s=word(m,&addr);if(!s){s=read_byte(m,addr,&b);if(!s){unsigned diff=(unsigned)m->a-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->a>=b?F_C:0));nz(m,(uint8_t)diff);}}break;
 case 0xc9:s=operand(m,&b);if(!s){unsigned diff=(unsigned)m->a-b;
   m->p=(uint8_t)((m->p&~F_C)|(m->a>=b?F_C:0));nz(m,(uint8_t)diff);}break;
 case 0x29:s=operand(m,&b);if(!s){m->a&=b;nz(m,m->a);}break;
 case 0x09:s=operand(m,&b);if(!s){m->a|=b;nz(m,m->a);}break;
 case 0xd0:s=branch(m,(m->p&F_Z)==0u);break;
 case 0xf0:s=branch(m,(m->p&F_Z)!=0u);break;
 case 0x10:s=branch(m,(m->p&F_N)==0u);break;
 case 0x30:s=branch(m,(m->p&F_N)!=0u);break;
 case 0x20:s=word(m,&addr);if(!s){
  uint16_t ret=(uint16_t)(m->pc-1u);push(m,(uint8_t)(ret>>8u));push(m,(uint8_t)ret);
  if(!m->result.first_jsr)m->result.first_jsr=addr;
  m->result.jsr_count++;m->pc=addr;
 }break;
 case 0x60:{uint8_t lo,hi;s=pop(m,&lo);if(!s)s=pop(m,&hi);
  if(!s){m->pc=(uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+1u);}
  break;}
 case 0x4c:s=word(m,&addr);if(!s)m->pc=addr;break;
 case 0x00:s=GK83_BRK_GATE;break;
 default:s=GK83_OPCODE_GATE;break;
 }
 if(s==GK83_INVALID)m->result.instructions++;
 return s;
}
/* EXEC0084: a single explicitly hypothetical RAM byte may be preseeded
 * for controlled branching; this does NOT describe a genuine NES power-on
 * value. The regular trace never presumes RAM was initialized. */
static Gk83Stop trace_impl(const char *path,uint32_t max_steps,Gk83Trace *out,
                          int use_hypothesis,uint16_t ram_address,uint8_t value,int mesen_zero_ram){
 if(!path||!out||max_steps==0u||max_steps>1000000u||
    (use_hypothesis && ram_address>=2048u))return GK83_INVALID;
 Gk82RomBootstrap boot;
 if(gk82_probe_stock_rom(path,&boot)!=GK82_ROM_OK)return GK83_SOURCE_REFUSED;
 uint8_t *rom=(uint8_t*)malloc(ROM_BYTES+1u);
 if(!rom)return GK83_SOURCE_REFUSED;
 FILE *file=fopen(path,"rb");if(!file){free(rom);return GK83_SOURCE_REFUSED;}
 size_t n=fread(rom,1,ROM_BYTES+1u,file);
 int bad=ferror(file);
 if(fclose(file)!=0)bad=1;
 if(n!=ROM_BYTES||bad){free(rom);return GK83_SOURCE_REFUSED;}
 uint8_t sha[32];gk1_sha256_bytes(rom,ROM_BYTES,sha);
 if(memcmp(sha,expected_sha,32)!=0){free(rom);return GK83_SOURCE_REFUSED;}
 Machine *m=(Machine*)calloc(1,sizeof *m);
 if(!m){free(rom);return GK83_SOURCE_REFUSED;}
 m->rom=rom;m->pc=boot.reset_vector;m->shift=0x10u;
 /* CPU 6502 reset enters at $C000; X/SP subsequently established by ROM.
  * Unknown RAM reads fail closed; no arbitrary power-on RAM is invented. */
 m->sp=0xfdu;m->p=F_I;
 /* A new, explicitly selected MesenCE power-on mode, not an NES-hardware
  * universal. The archived fresh-reset script sets RamPowerOnState=1;
  * MesenCE SettingTypes.h maps 1 to AllZeros, and EmuSettings.cpp uses
  * memset(data,0,length). The reset interpreter remains bounded by
  * unknown peripherals/opcodes/MMC1 behavior. A different capture setting
  * must NEVER silently take this path. */
 if(mesen_zero_ram)memset(m->known,1,sizeof m->known);
 if(use_hypothesis){m->ram[ram_address]=value;m->known[ram_address]=1u;}
 Gk83Stop status=GK83_BUDGET_GATE;
 for(uint32_t i=0;i<max_steps;i++){status=one(m);if(status!=GK83_INVALID)break;}
 if(status==GK83_INVALID)status=GK83_BUDGET_GATE;
 m->result.stop=status;
 m->result.a=m->a;m->result.x=m->x;m->result.y=m->y;
 m->result.sp=m->sp;m->result.flags=m->p;
 m->result.control=m->control;m->result.prg=m->prg;
 m->result.chr0=m->chr0;m->result.chr1=m->chr1;
 m->result.control_known=m->control_known;m->result.prg_known=m->prg_known;
 memcpy(out,&m->result,sizeof *out);
 free(m);free(rom);
 return status;
}

Gk83Stop gk83_trace_original_reset(const char *path,uint32_t max_steps,Gk83Trace *out){
 return trace_impl(path,max_steps,out,0,0u,0u,0);
}
Gk83Stop gk84_trace_reset_single_ram_hypothesis(const char *path,
 uint16_t ram_address,uint8_t value,uint32_t max_steps,Gk83Trace *out){
 return trace_impl(path,max_steps,out,1,ram_address,value,0);
}

/* EXEC0091: exact archived fresh Mesen script selected zero-filled CPU RAM.
 * This is source-driven emulation of that particular setting, NOT a new
 * Mesen runtime observation or a completed original Spring generator. */
Gk83Stop gk91_trace_reset_mesen_allzeros(const char *path,
 uint32_t max_steps,Gk83Trace *out){
 return trace_impl(path,max_steps,out,0,0u,0u,1);
}
