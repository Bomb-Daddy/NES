#include "gk312_stock_event_pc.h"
#include <string.h>
/* The known authentic fixed-bank NMI $008A gate LDA instruction is at
 * $C106; the original PRG RNG subroutine entry is $C289. These are PC
 * metadata, not copied NES code or a claim that the instruction ran. */
Gk312Status gk312_verify_original_event_pc(const char *rom,
 const uint8_t seed[6],const Gk302SeedEvent *events,size_t count,
 const Gk304Registers *registers,size_t nr,
 const Gk312PcWitness *pcs,size_t np,Gk312Report *out){
 if(!rom||!*rom||!seed||!events||!count||count>GK302_MAX_EVENTS||
    !pcs||np!=count||!out||!registers||!nr||nr>GK304_MAX_STORES)
    return GK312_ARGUMENT;
 Gk312Report result={0};
 size_t store_ix=0;
 for(size_t i=0;i<count;i++){
  const Gk302SeedEvent *e=&events[i];
  const Gk312PcWitness *p=&pcs[i];
  if(!e->cpu_cycle||p->cpu_cycle!=e->cpu_cycle)
   return GK312_ORIGIN;
  switch(e->kind){
   case GK302_NMI:
    if(p->observed_pc!=0xc106u||p->mapped_prg_bank!=15u)
     return GK312_ORIGIN;
    result.nmi_pc_witnesses++;break;
   case GK302_C289:
    if(p->observed_pc!=0xc289u||p->mapped_prg_bank!=15u)
     return GK312_ORIGIN;
    result.c289_pc_witnesses++;break;
   case GK302_STORE:
    if(store_ix>=nr||p->observed_pc!=e->source_pc||
       p->mapped_prg_bank!=e->bank||
       p->cpu_cycle!=registers[store_ix].cpu_cycle||
       p->observed_pc!=registers[store_ix].pc||
       p->mapped_prg_bank!=registers[store_ix].mapped_prg_bank)
       return GK312_ORIGIN;
    store_ix++;result.store_pc_witnesses++;break;
   default:return GK312_ORIGIN;
  }
 }
 if(store_ix!=nr)return GK312_ORIGIN;
 Gk304Status status=gk304_verify_store_values(rom,seed,events,count,
                                             registers,nr,&result.replay);
 if(status!=GK304_OK)return GK312_SOURCE;
 result.strict_cpu_origin_claims_checked=1u;
 /* Explicitly leave observed_stock_runtime / generated_first_spring /
  * playable_orders_granted zero. Never mint source claims into gameplay. */
 *out=result;
 return GK312_OK;
}
