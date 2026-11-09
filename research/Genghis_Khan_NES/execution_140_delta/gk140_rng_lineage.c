#include "gk140_rng_lineage.h"
#include "gk137_stock_rng.h"
#include <stdint.h>
Gk140Result gk140_compare_mesen_seed_lineage(const Gk139MesenProof *earlier,
 const Gk139MesenProof *later,uint32_t maximum_draws,Gk140Lineage *out){
 if(!earlier||!later||!out||maximum_draws>65536u||
    earlier->cpu_cycles>later->cpu_cycles||
    !earlier->field_count||!later->field_count||
    earlier->country_bank==GK139_BANK_UNKNOWN||
    earlier->country_bank!=later->country_bank||
    earlier->seed48>UINT64_C(0xffffffffffff)||
    later->seed48>UINT64_C(0xffffffffffff)||
    (!(later->seed48&1u)&&earlier->seed48!=later->seed48))
  return GK140_LINEAGE_BAD_INPUT;
 /* Reject a malformed synthetic proof in which bytes and 48-bit value
  * disagree; archived proof is read-only but input can be tampered with. */
 uint64_t s1=0,s2=0;
 for(unsigned i=0;i<6u;i++){
  s1=(s1<<8u)|earlier->nes_seed[i];
  s2=(s2<<8u)|later->nes_seed[i];
 }
 if(s1!=earlier->seed48||s2!=later->seed48)
  return GK140_LINEAGE_BAD_INPUT;
 Gk140Lineage candidate={0};
 candidate.only_prng_is_unproven=1u;
 candidate.elapsed_cpu_cycles=later->cpu_cycles-earlier->cpu_cycles;
 Gk137StockRng rng={0};
 rng.seed48=earlier->seed48;
 /* A PRNG draw consumes at least one CPU cycle. Exclude any candidate
  * draw count greater than the actual observed elapsed CPU cycles. */
 uint64_t limit=maximum_draws;
 if(candidate.elapsed_cpu_cycles<limit)limit=candidate.elapsed_cpu_cycles;
 for(uint32_t i=0;i<=(uint32_t)limit;i++){
  candidate.tested_transitions++;
  if(rng.seed48==later->seed48){
   if(!candidate.compatible_counts)candidate.nearest_count=i;
   candidate.farthest_count=i;
   candidate.compatible_counts++;
  }
  if((uint64_t)i<limit){
   uint16_t ignored=0;
   if(gk137_draw15(&rng,&ignored)!=GK137_OK)
    return GK140_LINEAGE_BAD_INPUT;
  }
 }
 if(!candidate.compatible_counts)return GK140_LINEAGE_NO_PURE_CHAIN;
 *out=candidate;return GK140_LINEAGE_OK;
}
