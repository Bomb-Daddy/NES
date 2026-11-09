#include "gk140_rng_lineage.h"
#include "gk1_app_state.h"
#include "gk137_stock_rng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static unsigned checks;
#define T(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %s at %d\n",#x,__LINE__);exit(1);}}while(0)
static uint64_t forward_oracle(uint64_t seed){
 const uint64_t mask=UINT64_C(0xffffffffffff), mult=UINT64_C(0xe0900e0187c5);
 return (UINT64_C(0)-((seed|1u)*mult))&mask;
}
static Gk139MesenProof proof(uint64_t seed,uint64_t cycles){
 Gk139MesenProof p={0};p.seed48=seed;p.cpu_cycles=cycles;
 p.program_counter=0xEFC9u;p.field_count=253u;p.country_bank=GK139_BANK_SAVE;
 for(unsigned i=0;i<6u;i++)p.nes_seed[i]=(uint8_t)(seed>>(8u*(5u-i)));
 return p;
}
int main(void){
 Gk140Lineage out;
 Gk139MesenProof x=proof(UINT64_C(0x6beec77c067b),107624250u), y=x;
 y.cpu_cycles+=1000u;
 memset(&out,0xa5,sizeof out);
 T(gk140_compare_mesen_seed_lineage(&x,&y,5u,&out)==GK140_LINEAGE_OK);
 T(out.compatible_counts==1u&&out.nearest_count==0u&&out.farthest_count==0u);
 T(out.tested_transitions==6u&&out.only_prng_is_unproven==1u);
 T(out.elapsed_cpu_cycles==1000u);
 /* Independent C11 arithmetic oracle. Detect off-by-one, endian errors,
  * unknown low bit, and false zero-draw identification. */
 for(uint32_t trial=0;trial<100u;trial++){
  uint64_t seed=((UINT64_C(0xabcdaabbccdd)+((uint64_t)trial<<18u))|
    UINT64_C(1))&UINT64_C(0xffffffffffff);
  uint64_t after=seed;
  for(uint32_t n=0;n<200u;n++){
   x=proof(seed,500000u);y=proof(after,600000u);
   memset(&out,0x5a,sizeof out);
   T(gk140_compare_mesen_seed_lineage(&x,&y,n,&out)==GK140_LINEAGE_OK);
   T(out.nearest_count==n);
   T(out.tested_transitions==n+1u);
   T(out.only_prng_is_unproven==1u);
   after=forward_oracle(after);
  }
 }
 /* An even seed is legal for a zero-draw interval but no nonzero
  * source RNG transition can end at an even seed. */
 x=proof(UINT64_C(0x123456789ab0),100u);
 y=proof(x.seed48,101u);
 T(gk140_compare_mesen_seed_lineage(&x,&y,2u,&out)==GK140_LINEAGE_OK);
 T(out.nearest_count==0u&&out.compatible_counts==1u);
 x=proof(UINT64_C(0x123456789ab1),100u);
 y=proof(forward_oracle(x.seed48),100u);
 memset(&out,0xa1,sizeof out);Gk140Lineage unchanged_cycles=out;
 T(gk140_compare_mesen_seed_lineage(&x,&y,5u,&out)==GK140_LINEAGE_NO_PURE_CHAIN);
 T(memcmp(&out,&unchanged_cycles,sizeof out)==0);
 x=proof(UINT64_C(0x6beec77c067b),100u);

 y=proof(UINT64_C(0xa3e5b54eef81),150u);
 memset(&out,0xc3,sizeof out);Gk140Lineage unchanged=out;
 T(gk140_compare_mesen_seed_lineage(&x,&y,0u,&out)==GK140_LINEAGE_NO_PURE_CHAIN);
 T(memcmp(&out,&unchanged,sizeof out)==0);
 T(gk140_compare_mesen_seed_lineage(&x,&y,65537u,&out)==GK140_LINEAGE_BAD_INPUT);
 T(memcmp(&out,&unchanged,sizeof out)==0);
 y.cpu_cycles=99u;
 T(gk140_compare_mesen_seed_lineage(&x,&y,0u,&out)==GK140_LINEAGE_BAD_INPUT);
 T(memcmp(&out,&unchanged,sizeof out)==0);
 y.cpu_cycles=150u;y.country_bank=GK139_BANK_WORK;
 T(gk140_compare_mesen_seed_lineage(&x,&y,0u,&out)==GK140_LINEAGE_BAD_INPUT);
 y.country_bank=GK139_BANK_SAVE;y.seed48++;
 T(gk140_compare_mesen_seed_lineage(&x,&y,0u,&out)==GK140_LINEAGE_BAD_INPUT);
 T(memcmp(&out,&unchanged,sizeof out)==0);
 y=proof(x.seed48,150u);y.seed48^=1u;
 T(gk140_compare_mesen_seed_lineage(&x,&y,0u,&out)==GK140_LINEAGE_BAD_INPUT);
 T(memcmp(&out,&unchanged,sizeof out)==0);
 T(gk140_compare_mesen_seed_lineage(NULL,&y,0u,&out)==GK140_LINEAGE_BAD_INPUT);
 T(gk140_compare_mesen_seed_lineage(&x,&y,0u,NULL)==GK140_LINEAGE_BAD_INPUT);
 /* Same real application setup guard: never silently promote to PLAYABLE. */
 Gk1App *app=(Gk1App*)calloc(1,sizeof *app);T(app!=NULL);
 gk1_app_init(app);app->active_world=0u;
 app->setup_difficulty[0]=3u;app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 y=proof(x.seed48,150u);
 Gk1App *before=(Gk1App*)malloc(sizeof *before);T(before!=NULL);
 memcpy(before,app,sizeof *before);
 T(gk1_app_source_rng_lineage(app,&x,&y,0u,&out)==GK140_LINEAGE_OK);
 T(memcmp(before,app,sizeof *app)==0);
 app->setup_stage[0]=GK53_SETUP_PLAYERS;
 memcpy(before,app,sizeof *before);memset(&out,0x4b,sizeof out);unchanged=out;
 T(gk1_app_source_rng_lineage(app,&x,&y,0u,&out)==GK140_LINEAGE_BAD_INPUT);
 T(memcmp(before,app,sizeof *app)==0&&memcmp(&out,&unchanged,sizeof out)==0);
 free(before);free(app);
 printf("GK140 independent PRNG interval compatibility PASS %u assertions\n",checks);
 return 0;
}
