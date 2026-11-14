#include "gk280_original_rate_bridge.h"
#include "gk137_stock_rng.h"
#include <string.h>
/* No original PRG bytes/outputs embedded: runtime private ROM checksum and
 * fixed-bank coefficient bytes are checked by the inherited GK178 reader. */
Gk280Status gk280_calculate_rate_source(const char *rom,
 const uint8_t seed[6],uint8_t difficulty,Gk280RateCandidate *out){
 if(!rom||!*rom||!seed||!out||difficulty<1||difficulty>5)
  return GK280_ARGUMENT;
 Gk178RngResult original={0};
 if(gk178_step_from_authenticated_stock(rom,seed,&original)!=GK178_OK)
  return GK280_STOCK_ROM;
 Gk137StockRng modeled={0};uint16_t projected=0u;
 if(gk137_from_nes_bytes(&modeled,seed)!=GK137_OK||
    gk137_spring_rate(&modeled,difficulty,&projected)!=GK137_OK)
  return GK280_PARITY;
 uint8_t modeled_bytes[6];gk137_to_nes_bytes(&modeled,modeled_bytes);
 uint16_t source_draw=(uint16_t)((uint16_t)original.result_67<<8|original.result_66);
 if(memcmp(modeled_bytes,original.after,6u)!=0||
    modeled.draws!=1u||projected!=50u+source_draw%(94u+7u*difficulty))
  return GK280_PARITY;
 Gk280RateCandidate candidate={0};
 memcpy(candidate.source_seed,seed,6u);
 memcpy(candidate.next_seed,original.after,6u);
 candidate.difficulty=difficulty;
 candidate.source_rate_candidate=projected;
 candidate.raw_draw15=source_draw;
 candidate.authenticated_rom=1u;
 candidate.candidate_only=1u;
 /* No independent proof the source RNG has been called at THIS seed/step
  * during initial setup; never grant Spring/turns or claim its observed Rate. */
 *out=candidate;
 return GK280_OK;
}
