#include "gk206_rng_source_parity.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
static unsigned checks;
#define CHECK(p) do{checks++;if(!(p)){fprintf(stderr,"ASSERT line %d: %s\n",__LINE__,#p);abort();}}while(0)
int main(void){
 Gk206Audit a={0}, before=a;
 CHECK(gk206_compare_original_stock(NULL,&a)==GK206_BAD_ARGUMENT);
 CHECK(gk206_compare_original_stock("",&a)==GK206_BAD_ARGUMENT);
 CHECK(gk206_compare_original_stock("/does-not-exist.nes",&a)==GK206_SOURCE_REFUSED);
 CHECK(!memcmp(&a,&before,sizeof a));
 CHECK(gk206_compare_coefficients(NULL,&a)==GK206_BAD_ARGUMENT);
 CHECK(gk206_compare_coefficients((const uint8_t[6]){0},NULL)==GK206_BAD_ARGUMENT);
 /* This deliberately INCORRECT six-byte coefficient array must not silently
  * match the independent fixed-multiplier arithmetic. */
 CHECK(gk206_compare_coefficients((const uint8_t[6]){0},&a)==GK206_COEFFICIENT_MISMATCH);
 CHECK(!memcmp(&a,&before,sizeof a));
 const char *private_rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(private_rom&&*private_rom){
  CHECK(gk206_compare_original_stock(private_rom,&a)==GK206_PASS);
  CHECK(a.source_authenticated==1&&a.original_first_spring_generated==0);
  CHECK(a.initial_seeds==512u&&a.draw_pairs==2048u&&a.spring_rate_pairs==2560u);
  CHECK(a.mismatches==0u);
  Gk206Audit old=a;
  CHECK(gk206_compare_original_stock("/does-not-exist.nes",&a)==GK206_SOURCE_REFUSED);
  CHECK(!memcmp(&a,&old,sizeof a));
 }
 printf("GK206 SOURCE RNG CONTRACT PASS (%u assertions, private-verified=%u, pairs=%u)\n",
  checks,a.source_authenticated,a.draw_pairs+a.spring_rate_pairs);
 return 0;
}
