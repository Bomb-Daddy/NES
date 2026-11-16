#include "gk331_stock_offer_rng.h"
#include "gk82_stock_cartridge.h"
#include "gk178_source_rng.h"
#include <stdio.h>
#include <string.h>

#define GK331_BANK1_OFFSET (16L + 16384L)
#define GK331_FIRST_TABLE (GK331_BANK1_OFFSET + 0x2fb2L)
#define GK331_SELL_WORDS 11u
#define GK331_BUY_WORDS 17u
#define GK331_TOTAL_WORDS (3u*(GK331_SELL_WORDS+GK331_BUY_WORDS))
#define GK331_FIXED_COEFF (16L + 15L*16384L + 0x02ecL)

static unsigned next(const uint8_t coeff[6],uint8_t seed[6],
                     uint16_t modulus,uint16_t *draws){
 Gk178RngResult r={0};
 if(gk178_step(seed,coeff,&r)!=GK178_OK)return 0xffffu;
 memcpy(seed,r.after,6u);(*draws)++;
 return (((unsigned)r.result_67<<8u)|r.result_66)%modulus;
}

Gk331Status gk331_offer_masks(const char *rom,
                              const uint8_t seed[6],Gk331Offers *out){
 if(!rom||!*rom||!seed||!out)return GK331_BAD;
 Gk82RomBootstrap boot={0};
 if(gk82_probe_stock_rom(rom,&boot)!=GK82_ROM_OK)return GK331_UNVERIFIED_ROM;
 FILE *fp=fopen(rom,"rb");if(!fp)return GK331_UNVERIFIED_ROM;
 uint8_t data[GK331_TOTAL_WORDS*2u]={0};
 uint8_t coeff[6]={0};
 int ok=fseek(fp,GK331_FIRST_TABLE,SEEK_SET)==0&&
  fread(data,1,sizeof data,fp)==sizeof data&&
  fseek(fp,GK331_FIXED_COEFF,SEEK_SET)==0&&
  fread(coeff,1,sizeof coeff,fp)==sizeof coeff;
 int bad=ferror(fp)||fclose(fp)!=0;
 if(!ok||bad)return GK331_UNVERIFIED_ROM;
 /* Exact stock ROM: bank1 CPU $AFB2..$B059 has sequential 16-bit little-
  * endian offer probability words for three merchant SELL rows of eleven,
  * followed by three BUY rows of seventeen. Unknown/mutated code REFUSED. */
 uint8_t sellprob[3][11],buyprob[3][17];
 for(unsigned merchant=0;merchant<3;merchant++){
  for(unsigned i=0;i<11;i++){
   unsigned pos=2u*(merchant*11u+i);
   if(data[pos+1]!=0u||data[pos]>100u)return GK331_SOURCE_MISMATCH;
   sellprob[merchant][i]=data[pos];
  }
 }
 for(unsigned merchant=0;merchant<3;merchant++){
  for(unsigned i=0;i<17;i++){
   unsigned pos=2u*(33u+merchant*17u+i);
   if(data[pos+1]!=0u||data[pos]>100u)return GK331_SOURCE_MISMATCH;
   buyprob[merchant][i]=data[pos];
  }
 }
 Gk331Offers result={0};uint8_t state[6];memcpy(state,seed,6u);
 for(unsigned merchant=0;merchant<3;merchant++){
  unsigned forced=next(coeff,state,11u,&result.draws);
  if(forced>=11u)return GK331_SOURCE_MISMATCH;
  for(unsigned i=0;i<11;i++){
   if(i==forced)result.sell[merchant][i]=1;
   else{
    unsigned roll=next(coeff,state,100u,&result.draws);
    if(roll>=100u)return GK331_SOURCE_MISMATCH;
    result.sell[merchant][i]=(uint8_t)(roll<sellprob[merchant][i]);
   }
  }
  forced=next(coeff,state,17u,&result.draws);
  if(forced>=17u)return GK331_SOURCE_MISMATCH;
  for(unsigned i=0;i<17;i++){
   if(i==forced)result.buy[merchant][i]=1;
   else{
    unsigned roll=next(coeff,state,100u,&result.draws);
    if(roll>=100u)return GK331_SOURCE_MISMATCH;
    result.buy[merchant][i]=(uint8_t)(roll<buyprob[merchant][i]);
   }
  }
 }
 if(result.draws!=84u)return GK331_SOURCE_MISMATCH;
 memcpy(result.final_seed,state,6u);
 result.source_verified=1u;result.offers_generated=1u;
 result.merchant_presence_known=0u; /* CRITICAL: live scheduler still unknown */
 *out=result;return GK331_OK;
}
