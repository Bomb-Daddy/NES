#include "gk319_cpu_trace_math.h"
#include "gk137_stock_rng.h"
#include "gk300_stock_nmi_seed_clock.h"
#include <string.h>

Gk319Status gk319_reconcile_cpu_trace_math(const Gk316Trace *trace,
 Gk319Receipt *out){
 if(!trace||!out) return GK319_ARGUMENT;
 if(trace->event_count==0u||trace->event_count>GK316_EVENT_LIMIT||
    trace->world>1u||trace->source_claim_only!=1u||
    trace->observed_stock_emulator||trace->first_spring_initialized||
    trace->player_orders_granted) return GK319_CLAIM;
 Gk319Receipt next={0};next.world=trace->world;
 uint8_t seed[6];memcpy(seed,trace->initial_seed,sizeof seed);
 uint8_t written_mask=0u;
 uint16_t seen_s=0u,seen_n=0u,seen_c=0u;
 uint64_t last_cycle=0u;
 for(unsigned i=0;i<trace->event_count;i++){
  const Gk316Event *ev=&trace->events[i];
  if(!ev->cycle||ev->cycle<=last_cycle)return GK319_CHRONOLOGY;
  last_cycle=ev->cycle;
  if(ev->kind==GK316_NMI){
   if(ev->pc!=0xc106u||ev->bank!=15u)return GK319_SOURCE;
   const uint8_t gate=ev->gate;
   Gk300NmiResult one={0};
   if(gk300_model_nmi_gate_history(seed,&gate,1u,&one)!=GK300_OK)
    return GK319_SOURCE;
   memcpy(seed,one.after,6u);
   next.nmis_enabled+=(uint16_t)one.enabled_nmis;
   next.nmis_gated+=(uint16_t)one.gated_nmis;
   seen_n++;
  } else if(ev->kind==GK316_STORE){
   if(ev->pc<0x8000u||ev->bank>15u||
      ev->target<0x84u||ev->target>0x89u)return GK319_SOURCE;
   const unsigned index=(unsigned)ev->target-0x84u;
   if(seed[index]!=ev->before)return GK319_PARITY;
   /* The recorder does not provide an opcode; the value must at least agree
    * with one observed A/X/Y register. Exact opcode/CPU-bank attestation
    * belongs to inherited GK304/GK312 and the PRIVATE original-ROM check. */
   if(ev->after!=ev->a&&ev->after!=ev->x&&ev->after!=ev->y)
    return GK319_SOURCE;
   seed[index]=ev->after;
   written_mask=(uint8_t)(written_mask|(uint8_t)(1u<<index));
   next.source_stores++;seen_s++;
  } else if(ev->kind==GK316_C289){
   if(ev->pc!=0xc289u||ev->bank!=15u||ev->draw15>32767u)
    return GK319_SOURCE;
   if(written_mask!=0x3fu)return GK319_SEED_UNPROVEN;
   if(memcmp(seed,ev->rng_before,6u)!=0)return GK319_PARITY;
   Gk137StockRng rng={0};uint16_t draw=0u;
   if(gk137_from_nes_bytes(&rng,seed)!=GK137_OK||
      gk137_draw15(&rng,&draw)!=GK137_OK)return GK319_SOURCE;
   uint8_t after[6];gk137_to_nes_bytes(&rng,after);
   if(draw!=ev->draw15||memcmp(after,ev->rng_after,6u)!=0)
    return GK319_PARITY;
   if(next.c289_calls==0u){
    next.first_draw_cycle=ev->cycle;
    memcpy(next.first_seed_at_call,seed,6u);
   }
   next.last_draw_cycle=ev->cycle;
   next.c289_calls++;seen_c++;
   memcpy(seed,after,6u);
  } else return GK319_SOURCE;
 }
 if(!seen_c)return GK319_SEED_UNPROVEN;
 if(seen_s!=trace->store_count||seen_n!=trace->nmi_count||
    seen_c!=trace->c289_count)return GK319_CLAIM;
 memcpy(next.final_seed,seed,6u);
 next.source_math_parity=1u;
 /* The source is an untrusted claim even when its *arithmetic* matches. */
 *out=next;
 return GK319_OK;
}
