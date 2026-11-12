#include "gk211_fresh_sequence.h"
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK211 line %u fail: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static unsigned checks;
int main(int argc,char **argv){
 Gk211Report report,blank;memset(&report,0xa7,sizeof report);blank=report;
 CHECK(gk211_verify_fresh_reset_sequence(NULL,&report)==GK211_ARGUMENT);
 CHECK(gk211_verify_fresh_reset_sequence("",&report)==GK211_ARGUMENT);
 CHECK(gk211_verify_fresh_reset_sequence("/GK211/no/fresh/stages",&report)==GK211_EVIDENCE_REFUSED);
 CHECK(!memcmp(&report,&blank,sizeof report));
 if(argc>=2){
  CHECK(gk211_verify_fresh_reset_sequence(argv[1],&report)==GK211_OK);
  CHECK(report.validated_stages==14u);
  CHECK(report.observed_setup_step==2u&&report.observed_food_doubling_step==9u);
  CHECK(report.observed_year_rate_step==10u&&report.observed_spring_economy_step==12u);
  CHECK(report.observed_order_grant_step==14u);
  CHECK(report.source_observation_only==1u&&!report.independent_initialization_implemented);
  CHECK(report.stage[0].countries==0u&&report.stage[1].countries==14u);
  CHECK(report.stage[7].food==129u&&report.stage[8].food==258u);
  CHECK(report.stage[9].year==1175u&&report.stage[9].rate==121u);
  CHECK(report.stage[10].gold==425u&&report.stage[11].gold==430u);
  CHECK(report.stage[10].food==258u&&report.stage[11].food==208u);
  CHECK(report.stage[10].troops==60u&&report.stage[11].troops==61u);
  CHECK(report.stage[12].orders==0u&&report.stage[13].orders==3u);
  for(unsigned i=1;i<14;i++){
   CHECK(report.stage[i].cpu_cycles>report.stage[i-1].cpu_cycles);
   CHECK(report.stage[i].frame>report.stage[i-1].frame);
  }
  Gk211Report snapshot=report;
  /* Missing original stage: atomic refusal, never synthetic replacement. */
  CHECK(gk211_verify_fresh_reset_sequence("/GK211/missing",&report)==GK211_EVIDENCE_REFUSED);
  CHECK(!memcmp(&report,&snapshot,sizeof report));
  if(argc>=4){
   CHECK(gk211_verify_fresh_reset_sequence(argv[3],&report)==GK211_EVIDENCE_REFUSED);
   CHECK(!memcmp(&report,&snapshot,sizeof report));
  }
  if(argc>=3){
   Gk1TemplateDataset ds={0};Gk1App *app=calloc(1,sizeof *app);
   CHECK(app!=NULL);CHECK(gk1_verified_original_csv(argv[2]));
   CHECK(gk1_template_load(argv[2],&ds)==GK1_LOAD_OK);
   gk1_app_init(app);CHECK(gk1_app_install_templates(app,&ds));
   Gk1App *copy=malloc(sizeof *app);CHECK(copy!=NULL);*copy=*app;
   CHECK(gk1_app_original_fresh_sequence(app,argv[2],argv[1],&report)==GK211_SOURCE_REFUSED);
   CHECK(!memcmp(&report,&snapshot,sizeof report));
   app->rules_profile=0u;*copy=*app;
   CHECK(gk1_app_original_fresh_sequence(app,argv[2],argv[1],&report)==GK211_OK);
   CHECK(!memcmp(app,copy,sizeof *app));
   app->worlds[0].countries[0].gold++;
   CHECK(gk1_app_original_fresh_sequence(app,argv[2],argv[1],&report)==GK211_SOURCE_REFUSED);
   CHECK(!memcmp(&report,&snapshot,sizeof report));
   free(copy);free(app);
  }
  printf("GK211 authenticated ORIGINAL-stock 14-stage chronology read; NOT new Mesen execution and NOT generalized first-Spring source: %u checks PASS\n",checks);
 }else printf("GK211 public negative and atomic refusal controls: %u checks PASS; original source not supplied\n",checks);
 return 0;
}
