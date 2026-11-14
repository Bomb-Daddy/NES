/* EXEC0280: exact stock original ROM source candidate RATE in actual native
 * C11 setup, not synthesized Spring or source ROM redistribution. */
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CK(c) do{if(!(c)){fprintf(stderr,"GK280 FAIL line %d: %s\n",__LINE__,#c);return 4;}checks++;}while(0)
int main(int argc,char **argv){
 unsigned checks=0;
 CK(argc==2||argc==3);
 Gk280RateCandidate x={0}; memset(&x,0xA5,sizeof x);
 Gk280RateCandidate saved=x;
 uint8_t seed[6]={0x01,0x23,0x45,0x67,0x89,0xab};
 CK(gk280_calculate_rate_source(NULL,seed,1,&x)==GK280_ARGUMENT);
 CK(gk280_calculate_rate_source(argv[1],seed,0,&x)==GK280_ARGUMENT);
 CK(gk280_calculate_rate_source(argv[1],seed,6,&x)==GK280_ARGUMENT);
 CK(gk280_calculate_rate_source("/nonexistent/stock.nes",seed,1,&x)==GK280_STOCK_ROM);
 CK(!memcmp(&x,&saved,sizeof x));
 Gk1App *app=calloc(1,sizeof *app),*before=malloc(sizeof *before);
 Gk1TemplateDataset *d=malloc(sizeof *d);
 CK(app&&before&&d);
 gk1_app_init(app);CK(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(app,d));
 app->rules_profile=0u;app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[0]=3u;app->active_world=0u;
 memcpy(before,app,sizeof *app);
 CK(gk1_app_original_rate_candidate(app,"/nonexistent/stock.nes",seed,&x)==GK280_STOCK_ROM);
 CK(!memcmp(&x,&saved,sizeof x));CK(!memcmp(before,app,sizeof *app));
 if(argc==3){
  /* Synthetic explicit seed trials with exact private original ROM, NEVER
   * a claimed observed startup seed, stock RATE or Spring chronology. */
  for(unsigned scenario=0;scenario<2;scenario++){
   app->active_world=(uint8_t)scenario;app->setup_stage[scenario]=GK53_SETUP_AWAIT_SOURCE;
   for(unsigned difficulty=1;difficulty<=5;difficulty++){
    app->setup_difficulty[scenario]=(uint8_t)difficulty;
    for(unsigned j=0;j<128;j++){
     for(unsigned k=0;k<6;k++)seed[k]=(uint8_t)(scenario*43u+j*5u+k*17u);
     memcpy(before,app,sizeof *app);
     CK(gk1_app_original_rate_candidate(app,argv[2],seed,&x)==GK280_OK);
     CK(!memcmp(before,app,sizeof *app));
     CK(x.authenticated_rom&&x.candidate_only&&!x.original_setup_proven&&!x.orders_granted);
     CK(x.difficulty==difficulty&&x.source_rate_candidate>=50u);
     CK(x.source_rate_candidate<50u+94u+7u*difficulty);
     CK(memcmp(x.source_seed,seed,6)==0);
     CK(x.source_rate_candidate==50u+x.raw_draw15%(94u+7u*difficulty));
    }
   }
  }
  saved=x;
  app->stock_checkpoint.valid=1u;
  CK(gk1_app_original_rate_candidate(app,argv[2],seed,&x)==GK280_STAGE);
  CK(!memcmp(&x,&saved,sizeof x));app->stock_checkpoint.valid=0u;
  app->rules_profile=1u;
  CK(gk1_app_original_rate_candidate(app,argv[2],seed,&x)==GK280_STAGE);
  CK(!memcmp(&x,&saved,sizeof x));app->rules_profile=0u;
  app->worlds[1].phase=(Gk1ScenarioPhase)255u;
  CK(gk1_app_original_rate_candidate(app,argv[2],seed,&x)==GK280_STAGE);
  CK(!memcmp(&x,&saved,sizeof x));
 }
 printf("GK280 %u checks PASS; original ROM Rate arithmetic %s; setup call chronology NOT PROVEN\n",checks,argc==3?"verified":"not run");
 free(d);free(before);free(app);return 0;
}
