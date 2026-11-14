#include "gk288_original_rng_stream.h"
#include "gk178_source_rng.h"
#include "gk137_stock_rng.h"
#include "gk82_stock_cartridge.h"
#include <stdio.h>
#include <string.h>

/* 16 PRG banks, 16-byte header, fixed last bank maps CPU C000-FFFF.
 * Authenticated bank C2EC holds six private coefficients; load from the
 * owner's exact ROM, never place original coefficient data into this code. */
#define GK288_COEFF_OFF (16L+15L*16384L+0x02ecL)
Gk288Status gk288_calculate_original_stream(const char *rom,
 const uint8_t seed[6],uint8_t difficulty,unsigned draws,Gk288Stream *out){
 if(!rom||!*rom||!seed||!out||difficulty<1u||difficulty>5u||
    draws==0u||draws>GK288_MAX_DRAWS)return GK288_ARGUMENT;
 Gk82RomBootstrap boot={0};
 if(gk82_probe_stock_rom(rom,&boot)!=GK82_ROM_OK)return GK288_ROM;
 FILE *f=fopen(rom,"rb");if(!f)return GK288_ROM;
 uint8_t coeff[6]={0};
 int read_ok=fseek(f,GK288_COEFF_OFF,SEEK_SET)==0&&fread(coeff,1,6,f)==6;
 int error=ferror(f),closed=fclose(f);
 if(!read_ok||error||closed){memset(coeff,0,sizeof coeff);return GK288_ROM;}
 Gk288Stream next={0};next.difficulty=difficulty;next.count=(uint16_t)draws;
 memcpy(next.before,seed,6);
 uint8_t current[6];memcpy(current,seed,6);
 Gk137StockRng independent={0};
 if(gk137_from_nes_bytes(&independent,seed)!=GK137_OK){
  memset(coeff,0,sizeof coeff);return GK288_PARITY;
 }
 for(unsigned i=0;i<draws;i++){
  Gk178RngResult source={0};uint16_t modeled=0u;
  if(gk178_step(current,coeff,&source)!=GK178_OK||
     gk137_draw15(&independent,&modeled)!=GK137_OK){
   memset(coeff,0,sizeof coeff);return GK288_PARITY;
  }
  uint8_t independent_bytes[6]={0};
  gk137_to_nes_bytes(&independent,independent_bytes);
  uint16_t observed_draw=(uint16_t)(((uint16_t)source.result_67<<8)|
                                    (uint16_t)source.result_66);
  if(memcmp(source.after,independent_bytes,6)!=0||
     modeled!=observed_draw||independent.draws!=i+1u){
   memset(coeff,0,sizeof coeff);return GK288_PARITY;
  }
  next.draw15[i]=observed_draw;
  next.rate_candidates[i]=(uint16_t)(50u+observed_draw%(94u+7u*difficulty));
  memcpy(current,source.after,6);
 }
 memcpy(next.after,current,6);
 next.authenticated_original=1u;
 next.conditional_stream=1u;
 /* Do not claim source seed provenance, startup chronology or original turns. */
 memset(coeff,0,sizeof coeff);
 memset(current,0,sizeof current);
 *out=next;return GK288_OK;
}
