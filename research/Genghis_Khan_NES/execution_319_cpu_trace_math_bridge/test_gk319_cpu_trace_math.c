#include "gk319_cpu_trace_math.h"
#include "gk137_stock_rng.h"
#include "gk300_stock_nmi_seed_clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define CK(x) do{checks++;if(!(x)){fprintf(stderr,"GK319 FAILED line %u: %s\n",(unsigned)__LINE__,#x);return 2;}}while(0)
static int add_draw(Gk316Trace *t,uint64_t cycle,uint8_t seed[6]){
 Gk316Event *e=&t->events[t->event_count++];memset(e,0,sizeof *e);
 e->kind=GK316_C289;e->cycle=cycle;e->pc=0xc289u;e->bank=15u;
 memcpy(e->rng_before,seed,6u);
 Gk137StockRng r={0};uint16_t draw=0u;
 if(gk137_from_nes_bytes(&r,seed)!=GK137_OK||
    gk137_draw15(&r,&draw)!=GK137_OK)return 0;
 e->draw15=draw;gk137_to_nes_bytes(&r,e->rng_after);
 memcpy(seed,e->rng_after,6u);t->c289_count++;return 1;
}
static int add_nmi(Gk316Trace *t,uint64_t cycle,uint8_t gate,uint8_t seed[6]){
 Gk316Event *e=&t->events[t->event_count++];memset(e,0,sizeof *e);
 e->kind=GK316_NMI;e->cycle=cycle;e->pc=0xc106u;e->bank=15u;e->gate=gate;
 Gk300NmiResult modeled={0};
 if(gk300_model_nmi_gate_history(seed,&gate,1,&modeled)!=GK300_OK)return 0;
 memcpy(seed,modeled.after,6u);t->nmi_count++;return 1;
}
static int make_trace(Gk316Trace *t){
 memset(t,0,sizeof *t);t->source_claim_only=1u;t->world=0u;
 const uint8_t first[6]={0xfd,0xfe,0xff,0xfe,0x50,0x70};
 memcpy(t->initial_seed,first,6u);uint8_t seed[6];memcpy(seed,first,6u);
 if(!add_nmi(t,1u,0u,seed))return 0; /* wrap first word, second */
 for(unsigned i=0;i<6;i++){
  Gk316Event *e=&t->events[t->event_count++];memset(e,0,sizeof *e);
  e->kind=GK316_STORE;e->cycle=2u+i;
  e->pc=(uint16_t)(0x9000u+i);e->bank=2u;e->target=(uint8_t)(0x84u+i);
  e->before=seed[i];e->a=e->after=(uint8_t)(0x10u+i*3u);seed[i]=e->after;
  t->store_count++;
 }
 if(!add_nmi(t,8u,1u,seed))return 0;
 if(!add_draw(t,9u,seed))return 0;
 if(!add_nmi(t,10u,0u,seed))return 0; /* real NMI arithmetic interleaved */
 if(!add_draw(t,11u,seed))return 0;
 return 1;
}
int main(void){
 Gk316Trace *trace=malloc(sizeof *trace);
 Gk319Receipt out,old;
 CK(trace!=NULL);CK(make_trace(trace));
 memset(&out,0xa7,sizeof out);
 CK(gk319_reconcile_cpu_trace_math(trace,&out)==GK319_OK);
 CK(out.world==0u&&out.source_stores==6u&&out.nmis_enabled==2u&&out.nmis_gated==1u);
 CK(out.c289_calls==2u&&out.first_draw_cycle==9u&&out.last_draw_cycle==11u);
 CK(out.source_math_parity&& !out.observed_stock_emulator&&
    !out.first_spring_generated&&!out.player_orders_granted);
 old=out;
 /* Every rejection must leave a prior successful receipt byte-identical. */
 #define REJECT(expected) do{CK(gk319_reconcile_cpu_trace_math(trace,&out)==(expected));CK(memcmp(&out,&old,sizeof out)==0);}while(0)
 trace->events[0].gate=1u;REJECT(GK319_PARITY);trace->events[0].gate=0u;
 trace->events[0].pc=0xc105u;REJECT(GK319_SOURCE);trace->events[0].pc=0xc106u;
 trace->events[0].cycle=0u;REJECT(GK319_CHRONOLOGY);trace->events[0].cycle=1u;
 trace->events[1].before^=1u;REJECT(GK319_PARITY);trace->events[1].before^=1u;
 trace->events[1].after^=1u;REJECT(GK319_SOURCE);trace->events[1].after^=1u;
 trace->events[6].target=0x85u;REJECT(GK319_PARITY);trace->events[6].target=0x89u;
 trace->events[8].rng_before[0]^=1u;REJECT(GK319_PARITY);trace->events[8].rng_before[0]^=1u;
 trace->events[8].rng_after[1]^=1u;REJECT(GK319_PARITY);trace->events[8].rng_after[1]^=1u;
 trace->events[8].draw15^=1u;REJECT(GK319_PARITY);trace->events[8].draw15^=1u;
 trace->events[9].gate=1u;REJECT(GK319_PARITY);trace->events[9].gate=0u;
 trace->events[10].cycle=8u;REJECT(GK319_CHRONOLOGY);trace->events[10].cycle=11u;
 trace->store_count=5u;REJECT(GK319_CLAIM);trace->store_count=6u;
 trace->source_claim_only=0u;REJECT(GK319_CLAIM);trace->source_claim_only=1u;
 trace->observed_stock_emulator=1u;REJECT(GK319_CLAIM);trace->observed_stock_emulator=0u;
 trace->first_spring_initialized=1u;REJECT(GK319_CLAIM);trace->first_spring_initialized=0u;
 trace->player_orders_granted=1u;REJECT(GK319_CLAIM);trace->player_orders_granted=0u;
 trace->event_count=8u;REJECT(GK319_SEED_UNPROVEN);trace->event_count=11u;
 trace->events[1].kind=GK316_NMI;REJECT(GK319_SOURCE);trace->events[1].kind=GK316_STORE;
 CK(gk319_reconcile_cpu_trace_math(NULL,&out)==GK319_ARGUMENT);
 CK(gk319_reconcile_cpu_trace_math(trace,NULL)==GK319_ARGUMENT);
 CK(memcmp(&out,&old,sizeof out)==0);
 CK(gk319_reconcile_cpu_trace_math(trace,&out)==GK319_OK);
 printf("GK319 PASS %u C11 assertions: seed writes, zero-gated NMI+2, C289 exact draws, rollback; claims only, no Spring\n",checks);
 free(trace);return 0;
}
