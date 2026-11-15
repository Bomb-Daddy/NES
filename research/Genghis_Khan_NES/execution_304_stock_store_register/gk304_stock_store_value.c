#include "gk304_stock_store_value.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK304_ROM_SIZE 262160u
#define GK304_PRG_SIZE 16384u
static const uint8_t stock_sha256[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3
};
static int load_original(const char *path,uint8_t **bytes){
 FILE *f=fopen(path,"rb");if(!f)return 0;
 uint8_t *b=(uint8_t*)malloc(GK304_ROM_SIZE+1u);
 if(!b){fclose(f);return 0;}
 size_t n=fread(b,1,GK304_ROM_SIZE+1u,f);
 int err=ferror(f),closed=fclose(f);
 uint8_t digest[32];
 if(err||closed||n!=GK304_ROM_SIZE){free(b);return 0;}
 gk1_sha256_bytes(b,n,digest);
 if(memcmp(stock_sha256,digest,sizeof digest)!=0){free(b);return 0;}
 *bytes=b;return 1;
}
/* Original iNES: 16 x 16KiB banks. $C000-$FFFF is hardwired bank15,
 * $8000-$BFFF is a mapper-selected bank0..14. Never confuse ROM bank with
 * CPU RAM/SRAM; only authentic ROM STORE opcodes are inspected here. */
static int reg_from_instruction(const uint8_t *rom,
 const Gk304Registers *w,uint8_t *source_value,uint8_t *which){
 if(w->destination<0x84u||w->destination>0x89u||
    w->mapped_prg_bank>15u)return 0;
 size_t off;
 if(w->pc>=0xc000u){
  if(w->mapped_prg_bank!=15u)return 0;
  off=(size_t)(w->pc-0xc000u);
 }else if(w->pc>=0x8000u){
  if(w->mapped_prg_bank==15u)return 0;
  off=(size_t)(w->pc-0x8000u);
 }else return 0;
 size_t pos=16u+(size_t)w->mapped_prg_bank*GK304_PRG_SIZE+off;
 uint8_t op=rom[pos];size_t len=0;
 switch(op){
  case 0x85u:case 0x8du:len=op==0x85u?2u:3u;*which=1u;*source_value=w->a;break;
  case 0x86u:case 0x8eu:len=op==0x86u?2u:3u;*which=2u;*source_value=w->x;break;
  case 0x84u:case 0x8cu:len=op==0x84u?2u:3u;*which=3u;*source_value=w->y;break;
  default:return 0;
 }
 if(off+len>GK304_PRG_SIZE)return 0;
 unsigned operand=(unsigned)rom[pos+1u];
 if(len==3u)operand|=((unsigned)rom[pos+2u]<<8);
 return operand==(unsigned)w->destination;
}
Gk304Status gk304_verify_store_values(const char *private_rom,
 const uint8_t initial[6],const Gk302SeedEvent *events,size_t count,
 const Gk304Registers *regs,size_t nr,Gk304Report *out){
 if(!private_rom||!*private_rom||!initial||!events||!out||!regs||
    !count||count>GK302_MAX_EVENTS||!nr||nr>GK304_MAX_STORES)
  return GK304_ARGUMENT;
 Gk302SeedResult replay={0};
 Gk302Status ts=gk302_replay_stock_seed_timeline(private_rom,initial,
                                                events,count,&replay);
 if(ts!=GK302_OK)
  return ts==GK302_ROM?GK304_STOCK_ROM:GK304_TIMELINE;
 uint8_t *rom=NULL;
 if(!load_original(private_rom,&rom))return GK304_STOCK_ROM;
 Gk304Report result={0};result.seed=replay;
 size_t used=0u;
 for(size_t i=0;i<count;i++){
  const Gk302SeedEvent *e=&events[i];
  if(e->kind!=GK302_STORE)continue;
  if(used==nr){free(rom);return GK304_REGISTER_COUNT;}
  const Gk304Registers *r=&regs[used++];
  if(r->cpu_cycle!=e->cpu_cycle||r->pc!=e->source_pc||
     r->mapped_prg_bank!=e->bank||r->destination!=e->target){
   free(rom);return GK304_VALUE;
  }
  uint8_t from_reg=0u,which=0u;
  if(!reg_from_instruction(rom,r,&from_reg,&which)){
   free(rom);return GK304_SOURCE_OPCODE;
  }
  if(from_reg!=e->after){free(rom);return GK304_VALUE;}
  if(which==1u)result.sta_count_nonzero=1u;
  else if(which==2u)result.stx_count_nonzero=1u;
  else result.sty_count_nonzero=1u;
 }
 free(rom);
 if(used!=nr||used!=replay.seed_stores)return GK304_REGISTER_COUNT;
 result.register_verified_writes=(uint16_t)used;
 result.authentic_original_rom_verified=1u;
 result.claimed_register_consistency_only=1u;
 /* Neither a fabricated witness nor an authentic static opcode can prove
  * CPU execution, establish all country states, or mint playable orders. */
 *out=result;
 return GK304_OK;
}
