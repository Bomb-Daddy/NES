#include "gk203_rng_selector_writers.h"
#include "gk202_irq_rng_dispatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HEADER 16u
#define BANK_BYTES 16384u
#define BANK_COUNT 16u
static uint16_t target_from_table(const Gk202Proof *p,uint8_t index){
 return index<GK202_TABLE_CAP?p->entry_targets[index]:0u;
}
Gk203Status gk203_from_bytes(const uint8_t *rom,size_t length,Gk203Proof *out){
 if(!rom||!out)return GK203_ARGUMENT;
 Gk202Proof prior={0};
 if(gk202_analyze_authenticated_bytes(rom,length,&prior)!=GK202_OK)
  return GK203_SOURCE_REFUSED;
 Gk203Proof result={0};
 result.authenticated_stock=1u;result.analysis_only=1u;
 result.rng_slot=prior.rng_slot;
 /* Count source *byte patterns*, not disassembled reachable CPU writes.
  * NES bank switching and indirect stores make this a lower-bound on paths,
  * not a proof that RNG cannot be selected by game initialization. */
 for(unsigned bank=0u;bank<BANK_COUNT;bank++){
  const uint8_t *segment=rom+HEADER+(size_t)bank*BANK_BYTES;
  for(size_t i=0;i+3u<BANK_BYTES;i++){
   if(segment[i]!=0xa9u||segment[i+2u]!=0x85u||segment[i+3u]!=0x50u)continue;
   if(segment[i+1u]==prior.rng_slot)result.raw_immediate_rng_slot_writes++;
   else result.raw_immediate_other_slot_writes++;
  }
 }
 /* Only two candidate sites in the exact authenticated fixed bank match
  * this whole local instruction structure. This is stronger than a bare
  * two-byte match but remains static, not a runtime BRK trace. */
 const uint8_t *fixed=rom+HEADER+15u*BANK_BYTES;
 for(size_t i=0;i+10u<BANK_BYTES;i++){
  if(fixed[i]!=0xa9u||fixed[i+2u]!=0x85u||fixed[i+3u]!=0x50u||
     fixed[i+4u]!=0xa9u||fixed[i+6u]!=0x85u||fixed[i+7u]!=0x52u||
     fixed[i+8u]!=0x20u||fixed[i+9u]!=0x76u||fixed[i+10u]!=0xefu)
   continue;
  if(result.fixed_caller_count>=GK203_FIXED_SITES)return GK203_SOURCE_REFUSED;
  Gk203Site *site=&result.fixed[result.fixed_caller_count++];
  site->pc=(uint16_t)(0xc000u+i);
  site->selector=fixed[i+1u];site->subcommand=fixed[i+5u];
  site->dispatched_target=target_from_table(&prior,site->selector);
  site->dispatch_slot_is_rng=(uint8_t)(site->selector==prior.rng_slot);
  if(site->dispatch_slot_is_rng)result.fixed_rng_caller_count++;
 }
 /* The known authenticated source contains two fixed trampoline callsites
  * at $EF3D and $EF4B with selector $17 (23, NOT RNG slot17 decimal).
  * Fail closed if any expected structure is absent. We do not hardcode an
  * unexplained Gold/Rate or claim the absence of other indirect writers. */
 if(result.fixed_caller_count!=2u||
    result.fixed[0].pc!=0xef3du||result.fixed[1].pc!=0xef4bu||
    result.fixed[0].selector!=23u||result.fixed[1].selector!=23u||
    result.fixed[0].subcommand!=5u||result.fixed[1].subcommand!=4u||
    result.fixed[0].dispatched_target!=0xc419u||
    result.fixed[1].dispatched_target!=0xc419u||
    result.fixed_rng_caller_count!=0u)
  return GK203_SOURCE_REFUSED;
 *out=result;return GK203_OK;
}
Gk203Status gk203_from_file(const char *name,Gk203Proof *out){
 if(!name||!*name||!out)return GK203_ARGUMENT;
 FILE *f=fopen(name,"rb");if(!f)return GK203_SOURCE_REFUSED;
 uint8_t *rom=malloc(GK202_STOCK_ROM_BYTES+1u);
 if(!rom){fclose(f);return GK203_SOURCE_REFUSED;}
 const size_t n=fread(rom,1,GK202_STOCK_ROM_BYTES+1u,f);
 const int failed=ferror(f)||fclose(f)!=0;
 Gk203Status result=(!failed&&n==GK202_STOCK_ROM_BYTES)?
  gk203_from_bytes(rom,n,out):GK203_SOURCE_REFUSED;
 memset(rom,0,GK202_STOCK_ROM_BYTES+1u);free(rom);return result;
}
