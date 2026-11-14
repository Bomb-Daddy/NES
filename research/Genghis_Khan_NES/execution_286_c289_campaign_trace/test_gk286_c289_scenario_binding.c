/* EXEC0286: source-arithmetic trace claim cannot silently cross campaigns.
 * ROM-based generated fixture is SYNTHETIC. NOT a fresh Mesen execution,
 * not proof of historical source-CPU Rate causality or playable Spring. */
#include "gk282_stock_rng_trace.h"
#include "gk178_source_rng.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK286 FAIL line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static void hex6(const uint8_t b[6],char s[13]){
 static const char d[]="0123456789abcdef";
 for(unsigned i=0;i<6u;i++){s[2u*i]=d[b[i]>>4u];s[2u*i+1u]=d[b[i]&15u];}
 s[12]=0;
}
static int synthetic_trace(const char *rom,const char *path,const char *header){
 const uint8_t seed[6]={3,7,11,19,23,29};
 Gk178RngResult r={0};
 if(gk178_step_from_authenticated_stock(rom,seed,&r)!=GK178_OK)return 0;
 char before[13],after[13];hex6(seed,before);hex6(r.after,after);
 unsigned draw=((unsigned)r.result_67<<8u)|r.result_66;
 FILE *f=fopen(path,"wb");if(!f)return 0;
 fprintf(f,"%s\nC,1,100,%s,%s,%u\nV,2,130,%u\n",header,before,after,draw,
  50u+draw%101u);
 return fclose(f)==0;
}
int main(int argc,char **argv){
 unsigned checks=0;CHECK(argc==2);
 const char *rom=getenv("GK1_PRIVATE_ORIGINAL_ROM");if(!rom)rom="";
 Gk282Report out={0};
 if(!*rom){
  CHECK(gk282_check_c289_trace(rom,argv[1],&out)==GK282_ARGUMENT);
  CHECK(out.generated_first_spring==0u);
  puts("GK286 public no-private-ROM negative control PASS; no gameplay claim");
  return 0;
 }
 Gk1App *a=calloc(1,sizeof *a);CHECK(a!=NULL);
 gk1_app_init(a);a->rules_profile=0u;
 for(unsigned w=0;w<2u;w++){
  a->world_ready[w]=1u;a->worlds[w].phase=GK1_PRE_SETUP_TEMPLATE;
  a->setup_stage[w]=GK53_SETUP_AWAIT_SOURCE;a->setup_difficulty[w]=1u;
 }
 /* V1 archive has no Mongol-versus-World indicator. Retain arithmetic
  * validation but refuse attaching its provenance to either setup. */
 CHECK(synthetic_trace(rom,argv[1],"GK1_C289_TRACE_V1,archived-saveram"));
 CHECK(gk282_check_c289_trace(rom,argv[1],&out)==GK282_OK);
 CHECK(out.scenario==2u&&out.claimed_world==255u&&!out.explicit_world_claim);
 Gk282Report before=out;
 a->active_world=0u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_ARGUMENT);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 a->active_world=1u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_ARGUMENT);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 /* Explicit World archived history may NOT bind to the Mongol setup. */
 CHECK(synthetic_trace(rom,argv[1],"GK1_C289_TRACE_V2,archived-world-saveram"));
 CHECK(gk282_check_c289_trace(rom,argv[1],&out)==GK282_OK);
 CHECK(out.scenario==2u&&out.claimed_world==1u&&out.explicit_world_claim);
 before=out;a->active_world=0u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_ARGUMENT);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 a->active_world=1u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_OK);
 CHECK(out.claimed_world==1u&&out.selected_difficulty==1u);
 CHECK(out.generated_first_spring==0u&&out.observed_trace_claimed==1u);
 /* Archived Mongol is separate from archived World, even though both
  * reside in SaveRAM with the identical physical source-country offset. */
 CHECK(synthetic_trace(rom,argv[1],"GK1_C289_TRACE_V2,archived-mongol-saveram"));
 before=out;a->active_world=1u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_ARGUMENT);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 a->active_world=0u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_OK);
 CHECK(out.claimed_world==0u&&out.scenario==2u);
 /* Fresh WorkRAM is Mongol in authenticated source evidence. */
 CHECK(synthetic_trace(rom,argv[1],"GK1_C289_TRACE_V2,fresh-mongol-workram"));
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_OK);
 CHECK(out.claimed_world==0u&&out.scenario==1u);
 before=out;a->active_world=1u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_ARGUMENT);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 /* Legacy fresh-mongol header still has unambiguous original source. */
 CHECK(synthetic_trace(rom,argv[1],"GK1_C289_TRACE_V1,fresh-workram"));
 a->active_world=0u;
 CHECK(gk1_app_original_c289_trace(a,rom,argv[1],&out)==GK282_OK);
 CHECK(out.explicit_world_claim&&out.claimed_world==0u);
 /* Unsupported or malformed source claims fail without changing report. */
 before=out;
 CHECK(synthetic_trace(rom,argv[1],"GK1_C289_TRACE_V2,fresh-world-workram"));
 CHECK(gk282_check_c289_trace(rom,argv[1],&out)==GK282_TRACE);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 CHECK(synthetic_trace(rom,argv[1],"GK1_C289_TRACE_V2,archived-mongol-saveram,garbage"));
 CHECK(gk282_check_c289_trace(rom,argv[1],&out)==GK282_TRACE);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 CHECK(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&
       a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE&&
       a->stock_checkpoint.valid==0u);
 free(a);
 CHECK(remove(argv[1])==0);
 printf("GK286 campaign-trace provenance: %u synthetic-source assertions PASS; no Spring generated\n",checks);
 return 0;
}
