#include "gk300_stock_nmi_seed_clock.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ORIGINAL_LEN 262160u
#define FIXED_BANK_OFFSET (16u+15u*16384u)
/* Original ROM SHA-256: identity metadata only. */
static const uint8_t original_hash[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3};
/* Verify meaningful instruction/operand boundaries rather than embedding an
 * executable snippet of copyrighted program bytes. Stock fixed-bank IRQ/NMI
 * source PC observations are different from hypothetical arithmetic. */
static int source_nmi_clock_intact(const uint8_t *p){
 /* LDA $008A; BNE skips both increments; CLC; both +2 independent. */
 if(p[0x106]!=0xad||p[0x107]!=0x8a||p[0x108]!=0||
    p[0x109]!=0xd0||p[0x10a]!=0x22||p[0x10b]!=0x18)
   return 0;
 /* Four absolute loads/stores for two 16-bit arithmetic words. */
 static const struct {unsigned load,adc,store;uint8_t addr,imm;} chunks[]={
  {0x10c,0x10f,0x111,0x87,2},
  {0x114,0x117,0x119,0x86,0},
  {0x11d,0x120,0x122,0x85,2},
  {0x125,0x128,0x12a,0x84,0}
 };
 if(p[0x11c]!=0x18||p[0x12d]!=0xa9)return 0;
 for(unsigned i=0;i<sizeof chunks/sizeof chunks[0];i++){
  const unsigned l=chunks[i].load,a=chunks[i].adc,s=chunks[i].store;
  if(p[l]!=0xad||p[l+1u]!=chunks[i].addr||p[l+2u]!=0||
     p[a]!=0x69||p[a+1u]!=chunks[i].imm||
     p[s]!=0x8d||p[s+1u]!=chunks[i].addr||p[s+2u]!=0)
   return 0;
 }
 return 1;
}
Gk300Status gk300_model_nmi_gate_history(const uint8_t initial[6],
 const uint8_t *gates,size_t count,Gk300NmiResult *out){
 if(!initial||!gates||!out||!count||count>GK300_MAX_NMIS)
  return GK300_ARGUMENT;
 Gk300NmiResult result={0};
 memcpy(result.initial,initial,6u);memcpy(result.after,initial,6u);
 result.nmi_observations=(uint16_t)count;
 for(size_t i=0;i<count;i++){
  if(gates[i]!=0u){result.gated_nmis++;continue;}
  /* Exact 6502 sequence: CLC, ADD #$02 to low byte, carry to high byte,
   * then a SECOND CLC before advancing the other pair. 16-bit wraps. */
  for(unsigned j=0;j<2u;j++){
   /* Source reads low $87, high $86; then low $85, high $84.
    * RAM pairs are BIG-ENDIAN in address order, despite the CPU being 6502.
    * Never reverse the two source-observed `ADC #$02` targets. */
   unsigned high=j*2u,low=high+1u;
   unsigned word=((unsigned)result.after[high]<<8)|result.after[low];
   word=(word+2u)&0xffffu;
   result.after[high]=(uint8_t)(word>>8);
   result.after[low]=(uint8_t)word;
  }
  result.enabled_nmis++;
 }
 result.conditional_history=1u;
 *out=result;return GK300_OK;
}
Gk300Status gk300_authentic_nmi_gate_history(const char *rom,
 const uint8_t initial[6],const uint8_t *gates,size_t count,
 Gk300NmiResult *out){
 if(!rom||!*rom||!initial||!gates||!out||!count||count>GK300_MAX_NMIS)
  return GK300_ARGUMENT;
 FILE *f=fopen(rom,"rb");if(!f)return GK300_ROM_IO;
 uint8_t *bytes=malloc(ORIGINAL_LEN+1u);
 if(!bytes){fclose(f);return GK300_ROM_IO;}
 const size_t n=fread(bytes,1,ORIGINAL_LEN+1u,f);
 const int err=ferror(f),closed=fclose(f);
 if(err||closed){free(bytes);return GK300_ROM_IO;}
 uint8_t hash[32];
 if(n!=ORIGINAL_LEN){free(bytes);return GK300_ROM_IDENTITY;}
 gk1_sha256_bytes(bytes,n,hash);
 if(memcmp(bytes,"NES\032",4u)||memcmp(hash,original_hash,32u)){
  free(bytes);return GK300_ROM_IDENTITY;
 }
 const int valid=source_nmi_clock_intact(bytes+FIXED_BANK_OFFSET);
 memset(bytes,0,ORIGINAL_LEN);free(bytes);
 if(!valid)return GK300_SOURCE_MISMATCH;
 Gk300NmiResult next={0};
 Gk300Status rc=gk300_model_nmi_gate_history(initial,gates,count,&next);
 if(rc!=GK300_OK)return rc;
 next.original_rom_sha256_verified=1u;
 *out=next;return GK300_OK;
}
