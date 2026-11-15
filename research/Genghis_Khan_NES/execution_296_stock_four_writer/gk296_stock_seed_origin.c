#include "gk296_stock_seed_origin.h"
#include "gk137_stock_rng.h"
#include <string.h>
Gk296Status gk296_check_four_writer_first_draw(
 const char *rom,const Gk296Witness *writes,size_t count,
 const uint8_t initial[6],const uint8_t claimed_pre[6],
 const uint8_t claimed_post[6],uint16_t claimed_draw15,
 Gk296Report *out){
 if(!rom||!*rom||!writes||!initial||!claimed_pre||!claimed_post||!out||
    count<4u||count>GK294_MAX_WRITES||claimed_draw15>0x7fffu)
   return GK296_ARGUMENT;
 Gk294Write sources[GK294_MAX_WRITES];uint8_t shadow[6];
 memcpy(shadow,initial,sizeof shadow);
 uint8_t mask=0u;
 uint64_t prev_cycle=0u;
 for(size_t i=0;i<count;i++){
  const Gk296Witness *w=&writes[i];
  if(w->source.cpu_cycle<=prev_cycle)return GK296_CHRONOLOGY;
  prev_cycle=w->source.cpu_cycle;
  if(w->source.destination_zp<0x84u||w->source.destination_zp>0x87u)
   return GK296_MASK; /* '$88/$89 actually written' is different provenance */
  const unsigned j=(unsigned)(w->source.destination_zp-0x84u);
  if(w->before!=shadow[j])return GK296_SEED;
  shadow[j]=w->committed;
  mask|=(uint8_t)(1u<<j);
  sources[i]=w->source;
 }
 if(mask!=0x0fu)return GK296_MASK;
 if(memcmp(shadow,claimed_pre,6u)!=0)return GK296_SEED;
 Gk294SourceProof proof={0};
 if(gk294_attest_seed_writers_masked(rom,sources,count,0x0fu,&proof)!=GK294_OK)
  return GK296_WRITER_SOURCE;
 Gk137StockRng rng={0};uint16_t draw=0u;uint8_t calculated[6];
 if(gk137_from_nes_bytes(&rng,claimed_pre)!=GK137_OK||
    gk137_draw15(&rng,&draw)!=GK137_OK)return GK296_C289;
 gk137_to_nes_bytes(&rng,calculated);
 if(draw!=claimed_draw15||memcmp(claimed_post,calculated,6u)!=0)
  return GK296_C289;
 Gk296Report next={0};
 next.source_writer_mask=0x0fu;
 next.unknown_poweron_mask=0x30u;
 next.original_rom_sha256_verified=proof.original_rom_sha256_verified;
 next.original_source_static_only=1u;
 next.first_c289_math_parity=1u;
 next.first_draw15=draw;
 *out=next;return GK296_OK;
}
