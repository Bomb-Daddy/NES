#include "gk282_stock_rng_trace.h"
#include "gk178_source_rng.h"
#include "gk1_app_state.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK285 line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static void h6(const uint8_t a[6],char out[13]){
 const char *hex="0123456789abcdef";
 for(unsigned j=0;j<6;j++){
  out[2*j]=hex[a[j]>>4u];out[2*j+1]=hex[a[j]&15u];
 }out[12]=0;
}
typedef struct {char before[13],after[13];uint16_t draw;} RngRow;
static int rows(const char *rom,RngRow r[2]){
 uint8_t seed[6]={3,7,11,19,23,29};
 /* Avoid a coincidental duplicate difficulty-1 Rate in this fixture. */
 for(unsigned tries=0;tries<64;tries++){
  uint8_t cur[6];memcpy(cur,seed,6);
  for(unsigned j=0;j<2;j++){
   Gk178RngResult out={0};
   if(gk178_step_from_authenticated_stock(rom,cur,&out)!=GK178_OK)return 0;
   h6(cur,r[j].before);h6(out.after,r[j].after);
   r[j].draw=(uint16_t)((out.result_67<<8u)|out.result_66);
   memcpy(cur,out.after,6);
  }
  if(r[0].draw%101u!=r[1].draw%101u)return 1;
  seed[0]++;
 }
 return 0;
}
static int write_trace(const char *path,const RngRow r[2],int archived,int bad){
 FILE *f=fopen(path,"wb");if(!f)return 0;
 fprintf(f,"GK1_C289_TRACE_V1,%s\n",archived?"archived-saveram":"fresh-workram");
 fprintf(f,"C,10,100,%s,%s,%u\n",r[0].before,r[0].after,r[0].draw);
 /* The first V has the NEXT C draw's difficulty-1 Rate. It must not match
  * a future event, even if the same number appears elsewhere in a trace. */
 fprintf(f,"V,11,110,%u\n",50u+r[1].draw%101u);
 fprintf(f,"V,12,120,%u\n",50u+r[0].draw%101u);
 fprintf(f,"C,13,130,%s,%s,%u\n",r[1].before,
  bad?"000000000000":r[1].after,r[1].draw);
 fprintf(f,"V,14,140,%u\n",50u+r[1].draw%101u);
 fprintf(f,"V,15,150,999\n");
 return fclose(f)==0;
}
int main(int argc,char **argv){
 unsigned checks=0;CHECK(argc==2);
 const char *rom=getenv("GK1_PRIVATE_ORIGINAL_ROM");
 const char *trace=argv[1];
 if(!rom)rom="";
 Gk282Report report={0},unchanged={0};
 if(!*rom){
  CHECK(gk282_check_c289_trace_for_difficulty(rom,trace,1u,&report)==GK282_ARGUMENT);
  CHECK(report.generated_first_spring==0u);
  puts("GK285 public no-private-ROM refusal PASS; no runtime claim");return 0;
 }
 RngRow r[2];CHECK(rows(rom,r));
 CHECK(write_trace(trace,r,0,0));
 CHECK(gk282_check_c289_trace_for_difficulty(rom,trace,1u,&report)==GK282_OK);
 CHECK(report.calls==2u&&report.rate_writes==4u&&report.selected_difficulty==1u);
 CHECK(report.selected_possible_pairs==2u&&report.selected_unique_writes==2u);
 CHECK(report.selected_ambiguous_writes==0u&&report.selected_unmatched_writes==2u);
 CHECK(report.selected_first_call==1u&&report.selected_last_call==2u);
 CHECK(report.generated_first_spring==0u&&report.observed_trace_claimed==1u);
 unchanged=report;
 CHECK(gk282_check_c289_trace_for_difficulty(rom,trace,6u,&report)==GK282_ARGUMENT);
 CHECK(memcmp(&unchanged,&report,sizeof report)==0);
 CHECK(write_trace(trace,r,0,1));
 CHECK(gk282_check_c289_trace_for_difficulty(rom,trace,1u,&report)==GK282_SOURCE_PARITY);
 CHECK(memcmp(&unchanged,&report,sizeof report)==0);
 CHECK(write_trace(trace,r,0,0));
 CHECK(gk282_check_c289_trace_for_difficulty(rom,trace,2u,&report)==GK282_OK);
 CHECK(report.selected_difficulty==2u&&report.generated_first_spring==0u);
 CHECK(report.selected_possible_pairs==0u&&report.selected_unmatched_writes==4u);
 CHECK(gk282_check_c289_trace(rom,trace,&report)==GK282_OK);
 CHECK(report.selected_difficulty==0u&&report.selected_possible_pairs==0u);
 /* Trace arithmetic comes from authenticated ROM; the native game's exact
  * setup-selected difficulty must constrain the C289 candidate projection. */
 Gk1App *app=calloc(1,sizeof *app);CHECK(app!=NULL);
 gk1_app_init(app);app->rules_profile=0u;
 app->world_ready[0]=1u;
 app->worlds[0].phase=GK1_PRE_SETUP_TEMPLATE;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[0]=1u;
 CHECK(gk1_app_original_c289_trace(app,rom,trace,&report)==GK282_OK);
 CHECK(report.selected_possible_pairs==2u&&report.selected_difficulty==1u);
 CHECK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&!app->stock_checkpoint.valid);
 Gk282Report prior=report;
 app->setup_difficulty[0]=0u;
 CHECK(gk1_app_original_c289_trace(app,rom,trace,&report)==GK282_ARGUMENT);
 CHECK(memcmp(&prior,&report,sizeof prior)==0);
 app->setup_difficulty[0]=1u;
 app->active_world=1u;
 app->world_ready[1]=1u;
 app->worlds[1].phase=GK1_PRE_SETUP_TEMPLATE;
 app->setup_stage[1]=GK53_SETUP_AWAIT_SOURCE;
 CHECK(gk1_app_original_c289_trace(app,rom,trace,&report)==GK282_ARGUMENT);
 CHECK(memcmp(&prior,&report,sizeof prior)==0);
 CHECK(write_trace(trace,r,1,0));
 CHECK(gk1_app_original_c289_trace(app,rom,trace,&report)==GK282_OK);
 CHECK(report.scenario==2u&&report.selected_difficulty==1u);
 CHECK(report.selected_possible_pairs==2u&&report.generated_first_spring==0u);
 /* Both fresh and archived are separate evidence; this cannot establish
  * original World ownership from the archived-only header. */
 free(app);
 CHECK(remove(trace)==0);
 printf("GK285 difficulty selected, future-call causality, rollback %u checks PASS; stock Spring NOT generated\n",checks);
 return 0;
}
