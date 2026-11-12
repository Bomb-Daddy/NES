#include "gk206_rng_source_parity.h"
#include "gk137_stock_rng.h"
#include "gk178_source_rng.h"
#include "gk82_stock_cartridge.h"
#include <stdio.h>
#include <string.h>

#define GK206_PRG15_COEFFICIENT_OFFSET (16L+15L*16384L+0x2ecL)
#define GK206_SEED_CASES 512u
#define GK206_DRAWS_EACH 4u
/* Reproducible seed space includes zero, all ones, all upper/lower masks,
 * and varied states without requiring original gameplay RNG seeds. */
static uint64_t sample_seed(unsigned n){
 const uint64_t fixed[]={UINT64_C(0),UINT64_C(1),UINT64_C(2),
  UINT64_C(0xffffffffffff),UINT64_C(0x800000000000),
  UINT64_C(0x000000ffffff),UINT64_C(0xffffff000000),
  UINT64_C(0x123456789abc),UINT64_C(0x010203040506)};
 if(n<sizeof fixed/sizeof fixed[0])return fixed[n];
 uint64_t x=(uint64_t)n*UINT64_C(0x9e3779b97f4a7c15);
 x^=x>>30;x*=UINT64_C(0xbf58476d1ce4e5b9);
 x^=x>>27;x*=UINT64_C(0x94d049bb133111eb);
 return (x^(x>>31))&UINT64_C(0xffffffffffff);
}
static int checked_next(Gk137StockRng *arithmetic,
 const uint8_t coefficients[6]){
 uint8_t before[6],after[6];
 Gk178RngResult exact={0};uint16_t draw=0;
 gk137_to_nes_bytes(arithmetic,before);
 if(gk178_step(before,coefficients,&exact)!=GK178_OK)return 0;
 if(gk137_draw15(arithmetic,&draw)!=GK137_OK)return 0;
 gk137_to_nes_bytes(arithmetic,after);
 if(memcmp(before,exact.before,6u)!=0||
    memcmp(after,exact.after,6u)!=0||
    draw!=(uint16_t)(((uint16_t)exact.result_67<<8)|exact.result_66))
  return 0;
 return 1;
}
Gk206Status gk206_compare_coefficients(const uint8_t coefficients[6],
 Gk206Audit *out){
 if(!coefficients||!out)return GK206_BAD_ARGUMENT;
 Gk206Audit result={0};
 for(unsigned n=0;n<GK206_SEED_CASES;n++){
  const uint64_t seed=sample_seed(n);
  Gk137StockRng rng={seed,0u};
  result.initial_seeds++;
  for(unsigned j=0;j<GK206_DRAWS_EACH;j++){
   result.draw_pairs++;
   if(!checked_next(&rng,coefficients))result.mismatches++;
  }
  /* Rate formula is source-attested, but seed/number of prior draws is NOT.
   * Check each difficulty independently, never infer observed stock Rate. */
  for(unsigned difficulty=1u;difficulty<=5u;difficulty++){
   Gk137StockRng source={seed,0u};
   Gk137StockRng projected=source;
   uint16_t value=UINT16_MAX;
   if(!checked_next(&source,coefficients))result.mismatches++;
   if(gk137_spring_rate(&projected,(uint8_t)difficulty,&value)!=GK137_OK)
    result.mismatches++;
   /* The source-derived bounded rate uses one full 15-bit draw. */
   uint16_t next=(uint16_t)((source.seed48>>32u)&0x7fffu);
   if(value!=50u+(next%(94u+7u*difficulty))||
       projected.seed48!=source.seed48||projected.draws!=1u)
    result.mismatches++;
   result.spring_rate_pairs++;
  }
 }
 if(result.mismatches)return GK206_COEFFICIENT_MISMATCH;
 *out=result;return GK206_PASS;
}
Gk206Status gk206_compare_original_stock(const char *path,Gk206Audit *out){
 if(!path||!*path||!out)return GK206_BAD_ARGUMENT;
 Gk82RomBootstrap verified={0};
 if(gk82_probe_stock_rom(path,&verified)!=GK82_ROM_OK)
  return GK206_SOURCE_REFUSED;
 FILE *f=fopen(path,"rb");if(!f)return GK206_SOURCE_REFUSED;
 uint8_t coeff[6]={0};
 int good=fseek(f,GK206_PRG15_COEFFICIENT_OFFSET,SEEK_SET)==0&&
           fread(coeff,1,sizeof coeff,f)==sizeof coeff;
 int read_error=ferror(f);
 int close_error=fclose(f)!=0;
 int bad=read_error||close_error;
 if(!good||bad){memset(coeff,0,sizeof coeff);return GK206_SOURCE_REFUSED;}
 Gk206Audit candidate={0};
 Gk206Status result=gk206_compare_coefficients(coeff,&candidate);
 memset(coeff,0,sizeof coeff);
 if(result!=GK206_PASS)return result;
 candidate.source_authenticated=1u;
 candidate.original_first_spring_generated=0u;
 *out=candidate;return GK206_PASS;
}
