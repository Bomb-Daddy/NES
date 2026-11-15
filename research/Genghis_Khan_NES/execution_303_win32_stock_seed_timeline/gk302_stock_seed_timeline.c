#include "gk302_stock_seed_timeline.h"
#include "gk294_seed_writer_attest.h"
#include "gk300_stock_nmi_seed_clock.h"
#include "gk137_stock_rng.h"
#include <string.h>

Gk302Status gk302_replay_stock_seed_timeline(const char *rom,
 const uint8_t initial[6],const Gk302SeedEvent *events,size_t count,
 Gk302SeedResult *out){
 if(!rom||!*rom||!initial||!events||!out||count==0u||
    count>GK302_MAX_EVENTS)return GK302_ARGUMENT;
 /* Exact original stock identity + source NMI opcode independently checked
  * once. No unverified emulator frame count is treated as an NMI. */
 const uint8_t suppressed[1]={1u};
 Gk300NmiResult attest={0};
 if(gk300_authentic_nmi_gate_history(rom,initial,suppressed,1u,&attest)!=GK300_OK||
    !attest.original_rom_sha256_verified)return GK302_ROM;
 Gk302SeedResult result={0};
 memcpy(result.initial,initial,6u);memcpy(result.final,initial,6u);
 Gk294Write batch[GK294_MAX_WRITES];size_t batched=0u;
 uint8_t targets=0u,all_targets=0u;
 uint64_t prev=0u;
 for(size_t i=0;i<count;i++){
  const Gk302SeedEvent *e=&events[i];
  if(e->cpu_cycle==0u || e->cpu_cycle<=prev)return GK302_ORDER;
  prev=e->cpu_cycle;
  if(e->kind==GK302_STORE){
   if(e->target<0x84u||e->target>0x89u)return GK302_VALUE;
   unsigned j=(unsigned)e->target-0x84u;
   if(result.final[j]!=e->before)return GK302_VALUE;
   Gk294Write one={e->cpu_cycle,e->source_pc,e->bank,e->target};
   batch[batched++]=one;
   targets|=(uint8_t)(1u<<j);all_targets|=(uint8_t)(1u<<j);
   result.final[j]=e->after;result.seed_stores++;
   /* Authenticate original mapped 6502 writer source, not merely target or
    * claimed value. More than 64 writes are split into exact-source batches. */
   if(batched==GK294_MAX_WRITES){
    Gk294SourceProof p={0};
    if(gk294_attest_seed_writers_masked(rom,batch,batched,targets,&p)!=GK294_OK||
       !p.original_rom_sha256_verified)return GK302_OPCODE;
    batched=0u;targets=0u;
   }
  }else if(e->kind==GK302_NMI){
   Gk300NmiResult step={0};
   if(gk300_model_nmi_gate_history(result.final,&e->gate,1u,&step)!=GK300_OK)
    return GK302_VALUE;
   memcpy(result.final,step.after,6u);
   if(e->gate)result.nmi_gated++;else result.nmi_enabled++;
  }else if(e->kind==GK302_C289){
   if(memcmp(e->rng_before,result.final,6u)!=0||e->draw15>32767u)
    return GK302_VALUE;
   Gk137StockRng rng={0};uint16_t draw=0u;uint8_t post[6];
   if(gk137_from_nes_bytes(&rng,result.final)!=GK137_OK||
      gk137_draw15(&rng,&draw)!=GK137_OK)return GK302_RNG;
   gk137_to_nes_bytes(&rng,post);
   if(draw!=e->draw15||memcmp(post,e->rng_after,6u)!=0)
    return GK302_RNG;
   memcpy(result.final,post,6u);result.c289_calls++;
  }else return GK302_VALUE;
 }
 if(batched){
  Gk294SourceProof p={0};
  if(gk294_attest_seed_writers_masked(rom,batch,batched,targets,&p)!=GK294_OK||
     !p.original_rom_sha256_verified)return GK302_OPCODE;
 }
 result.event_count=(uint16_t)count;
 result.source_store_targets_mask=all_targets;
 result.original_rom_sha256_verified=1u;
 result.source_claims_only=1u;
 /* Both remain ZERO: validated arithmetic from witness claims cannot replace
  * independent live Mesen capture or execute country setup / season rules. */
 *out=result;return GK302_OK;
}
