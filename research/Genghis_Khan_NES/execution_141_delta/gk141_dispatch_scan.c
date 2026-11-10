#include "gk141_dispatch_scan.h"
#include "gk82_stock_cartridge.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PRG_HEADER 16u
#define FIXED_BANK 15u
#define LOW_TABLE 0x3788u /* CPU $F788 in fixed bank */
#define HIGH_TABLE 0x3888u /* CPU $F888 in fixed bank */
Gk141Result gk141_scan_image(const uint8_t *rom,size_t n,Gk141DispatchMap *out){
 if(!rom||!out||n!=GK141_IMAGE_SIZE||rom[0]!='N'||rom[1]!='E'||
    rom[2]!='S'||rom[3]!=0x1au||rom[4]!=16u||rom[5]!=0u||
    rom[6]!=0x12u)return GK141_BAD_INPUT;
 Gk141DispatchMap next={0};
 const size_t final=PRG_HEADER+FIXED_BANK*GK141_BANK_BYTES;
 for(unsigned i=0;i<256u;i++){
  const uint16_t dest=(uint16_t)((uint16_t)rom[final+LOW_TABLE+i]|
                     ((uint16_t)rom[final+HIGH_TABLE+i]<<8u));
  next.dispatch_target[i]=dest;
  if(dest<0xc000u)next.target_below_fixed++;
 }
 for(unsigned bank=0;bank<GK141_BANK_COUNT;bank++){
  const size_t base=PRG_HEADER+bank*GK141_BANK_BYTES;
  for(unsigned pos=0;pos+3u<GK141_BANK_BYTES;pos++){
   /* JSR $EF85 followed by a one-byte dispatcher selector.
    * Data may contain false positives; do not call these executed JSRs. */
   if(rom[base+pos]!=0x20u||rom[base+pos+1u]!=0x85u||
      rom[base+pos+2u]!=0xefu)continue;
   const uint8_t selector=rom[base+pos+3u];
   if(next.candidate_total==UINT32_MAX)return GK141_BAD_INPUT;
   if(next.candidate_total==0u){
    next.first_bank=(uint8_t)bank;
    next.first_candidate_pc=(uint16_t)((bank==FIXED_BANK?0xc000u:0x8000u)+pos);
    next.first_selector=selector;
   }
   next.candidate_total++;next.by_bank[bank]++;next.by_selector[selector]++;
  }
  /* The last 3 bytes may look like a JSR but have no following selector
   * in the same bank; require unknown bank crossing to be refused. */
  for(unsigned pos=GK141_BANK_BYTES-3u;pos<GK141_BANK_BYTES;pos++){
   if(pos+2u<GK141_BANK_BYTES && rom[base+pos]==0x20u&&
      rom[base+pos+1u]==0x85u&&rom[base+pos+2u]==0xefu)
    next.candidates_boundary_rejected++;
  }
 }
 *out=next;return GK141_OK;
}
/* Independently hash the bytes actually scanned. Probe-then-read alone is
 * vulnerable to a file replacement between the two I/O operations. */
static const uint8_t stock_sha256[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3};
Gk141Result gk141_scan_authenticated(const char *path,Gk141DispatchMap *out){
 if(!path||!out)return GK141_BAD_INPUT;
 Gk82RomBootstrap boot={0};
 if(gk82_probe_stock_rom(path,&boot)!=GK82_ROM_OK)
  return GK141_SOURCE_UNVERIFIED;
 FILE *f=fopen(path,"rb");if(!f)return GK141_SOURCE_UNVERIFIED;
 uint8_t *buf=(uint8_t*)malloc(GK141_IMAGE_SIZE+1u);
 if(!buf){fclose(f);return GK141_BAD_INPUT;}
 const size_t n=fread(buf,1,GK141_IMAGE_SIZE+1u,f);
 const int bad=ferror(f),close_bad=fclose(f);
 Gk141Result result=GK141_SOURCE_UNVERIFIED;
 if(!bad&&!close_bad&&n==GK141_IMAGE_SIZE){
  uint8_t sha[32];gk1_sha256_bytes(buf,n,sha);
  if(memcmp(sha,stock_sha256,32u)==0)
   result=gk141_scan_image(buf,n,out);
 }
 free(buf);return result;
}
