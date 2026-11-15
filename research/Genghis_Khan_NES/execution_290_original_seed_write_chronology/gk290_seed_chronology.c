#include "gk290_seed_chronology.h"
#include "gk137_stock_rng.h"
#include "gk207_seed_write_watch.h"
#include <string.h>
Gk290Status gk290_verify_written_seed_timeline(const Gk290Event *ev,
 size_t count,Gk290Report *out){
 if(!ev||!out||count==0u)return GK290_ARGUMENT;
 if(count>GK290_MAX_EVENTS)return GK290_LIMIT;
 Gk290Report next={0};
 Gk207SeedWatch writers={0};
 uint8_t seed[6]={0}, written_mask=0u,started=0u;
 uint64_t prev_cycle=0u;
 for(size_t i=0;i<count;i++){
  const Gk290Event *e=&ev[i];
  if(!e->cycle||e->cycle<=prev_cycle)return GK290_CHRONOLOGY;
  prev_cycle=e->cycle;
  if(e->kind==GK290_WRITE){
   /* A claimed W must identify the actual CPU store and mapped ROM bank.
    * Do not silently accept a PPU/SRAM/other write or an unknown PC. */
   if(e->bank_known!=1u||e->bank>15u||e->pc<0x8000u||
      e->address<0x84u||e->address>0x89u||
      gk207_note_completed_seed_store(&writers,e->pc,e->address,
       e->value,1u,e->bank,e->cycle)!=GK207_RECORDED)
    return GK290_CHRONOLOGY;
   const uint8_t index=(uint8_t)(e->address-0x84u);
   seed[index]=e->value;
   written_mask=(uint8_t)(written_mask|(1u<<index));
   next.observed_seed_writes++;
   if(started)next.interleaved_seed_writes++;
  }else if(e->kind==GK290_CALL){
   if(e->pc!=0xc289u||e->bank_known!=1u||e->bank!=15u||
      e->draw15>0x7fffu)return GK290_CHRONOLOGY;
   if(written_mask!=0x3fu||!gk207_source_written_complete(&writers))
    return GK290_SEED_UNPROVEN;
   if(memcmp(seed,e->before,6u)!=0)return GK290_PARITY;
   if(!started){memcpy(next.first_explicit_seed,seed,6u);
    next.first_draw_cycle=e->cycle;
    next.six_source_writes_before_first_draw=1u;started=1u;}
   Gk137StockRng rng={0};uint16_t computed=0u;
   if(gk137_from_nes_bytes(&rng,seed)!=GK137_OK||
      gk137_draw15(&rng,&computed)!=GK137_OK)return GK290_PARITY;
   uint8_t after[6];gk137_to_nes_bytes(&rng,after);
   if(computed!=e->draw15||memcmp(after,e->after,6u)!=0)
    return GK290_PARITY;
   memcpy(seed,after,6u);
   next.validated_c289_calls++;
   next.last_draw_cycle=e->cycle;
  }else return GK290_CHRONOLOGY;
 }
 if(!started)return GK290_SEED_UNPROVEN;
 memcpy(next.final_seed,seed,6u);
 next.source_math_parity=1u;
 /* Source-executed arithmetic is NOT independent evidence the record
  * came from Mesen, nor does it assign the C289 call to a real Spring. */
 *out=next;
 return GK290_OK;
}
