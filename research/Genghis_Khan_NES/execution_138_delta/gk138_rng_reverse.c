#include "gk138_rng_reverse.h"
#include <limits.h>
#define GK138_MASK UINT64_C(0xffffffffffff)
/* Derived by extended Euclid: 0xe0900e0187c5 * 0xd8e495025f0d == 1 mod 2^48. */
#define GK138_INVERSE UINT64_C(0xd8e495025f0d)
#define GK138_MULT UINT64_C(0xe0900e0187c5)
static uint64_t prior_odd(uint64_t newer){
 return ((UINT64_C(0)-newer)*GK138_INVERSE)&GK138_MASK;
}
Gk138Result gk138_reverse(uint64_t last_state,uint32_t draws_back,
                         Gk138Reverse *out){
 if(!out||last_state>GK138_MASK||draws_back>65536u||
    (draws_back && !(last_state&1u)))return GK138_BAD_INPUT;
 Gk138Reverse candidate={last_state,last_state,draws_back};
 /* Every source output is odd; all intermediate states except the unknown
  * original initial state are therefore uniquely odd. */
 for(uint32_t i=0;i<draws_back;i++){
  uint64_t p=prior_odd(candidate.candidate_odd);
  /* Fail closed if the alleged original step could not actually produce
   * this output; the assertion is also a regression gate on inverse value. */
  if(!(p&1u)||(((UINT64_C(0)-(p*GK138_MULT))&GK138_MASK)!=candidate.candidate_odd))
   return GK138_BAD_INPUT;
  candidate.candidate_odd=p;
 }
 if(draws_back)candidate.candidate_even=candidate.candidate_odd&~UINT64_C(1);
 *out=candidate;
 return GK138_OK;
}
Gk138Result gk138_scan_spring_rate(uint64_t post_state,uint8_t difficulty,
                                   uint16_t observed_rate,
                                   uint32_t oldest_search,Gk138RateScan *out){
 if(!out||post_state>GK138_MASK||!(post_state&1u)||
    difficulty<1u||difficulty>5u||oldest_search==0u||oldest_search>65536u||
    observed_rate<50u||observed_rate>(uint16_t)(143u+7u*difficulty))
  return GK138_BAD_INPUT;
 const uint16_t bound=(uint16_t)(94u+7u*difficulty);
 Gk138RateScan candidate={0,UINT32_MAX,0};
 for(uint32_t i=0;i<oldest_search;i++){
  uint16_t rate=(uint16_t)(50u+(((post_state>>32u)&0x7fffu)%bound));
  if(rate==observed_rate){
   if(candidate.matches==0)candidate.nearest_offset=i;
   candidate.matches++;
  }
  candidate.inspected++;
  uint64_t p=prior_odd(post_state);
  if(!(p&1u)||(((UINT64_C(0)-(p*GK138_MULT))&GK138_MASK)!=post_state))
   return GK138_BAD_INPUT;
  post_state=p;
 }
 *out=candidate;
 return GK138_OK;
}
