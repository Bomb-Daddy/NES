/* EXEC0288: authentic original source C289 multi-draw sequence, no ROM bytes
 * distributed and no fabricated Spring/turns. Private ROM is OPTIONAL.
 * Pass original ROM path as argv[2] for the separate private-source proof. */
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include "gk137_stock_rng.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"GK288 FAIL %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
int main(int argc,char **argv){
 unsigned checks=0;CK(argc==2||argc==3);
 const uint8_t initial[6]={0x12,0x25,0x38,0x4b,0x5e,0x71};
 Gk288Stream out;memset(&out,0xb7,sizeof out);
 Gk288Stream unchanged=out;
 CK(gk288_calculate_original_stream("",initial,1,1,&out)==GK288_ARGUMENT);
 CK(gk288_calculate_original_stream("/missing/private.nes",initial,0,1,&out)==GK288_ARGUMENT);
 CK(gk288_calculate_original_stream("/missing/private.nes",initial,6,1,&out)==GK288_ARGUMENT);
 CK(gk288_calculate_original_stream("/missing/private.nes",initial,1,0,&out)==GK288_ARGUMENT);
 CK(gk288_calculate_original_stream("/missing/private.nes",initial,1,65,&out)==GK288_ARGUMENT);
 CK(gk288_calculate_original_stream("/missing/private.nes",initial,1,1,&out)==GK288_ROM);
 CK(memcmp(&out,&unchanged,sizeof out)==0);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *app=calloc(1,sizeof *app),*prior=malloc(sizeof *prior);
 CK(dataset&&app&&prior);
 gk1_app_init(app);
 CK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(app,dataset));
 app->active_world=0u;app->rules_profile=0u;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[0]=1u;
 memcpy(prior,app,sizeof *prior);
 CK(gk1_app_original_rng_stream(app,"/missing/private.nes",initial,1,&out)==GK288_ROM);
 CK(!memcmp(app,prior,sizeof *app));
 CK(!memcmp(&out,&unchanged,sizeof out));
 app->worlds[0].countries[0].gold++;
 CK(gk1_app_original_rng_stream(app,"/missing/private.nes",initial,1,&out)==GK288_STAGE);
 app->worlds[0].countries[0].gold--;
 CK(!memcmp(app,prior,sizeof *app));
 app->rules_profile=1u;
 CK(gk1_app_original_rng_stream(app,"/missing/private.nes",initial,1,&out)==GK288_STAGE);
 app->rules_profile=0u;
 app->stock_checkpoint.valid=1u;
 CK(gk1_app_original_rng_stream(app,"/missing/private.nes",initial,1,&out)==GK288_STAGE);
 app->stock_checkpoint.valid=0u;
 if(argc==3&&argv[2][0]){
  CK(gk1_verified_original_csv(argv[1]));
  for(unsigned w=0;w<2;w++){
   app->active_world=(uint8_t)w;
   app->setup_stage[w]=GK53_SETUP_AWAIT_SOURCE;
   for(unsigned diff=1;diff<=5;diff++){
    app->setup_difficulty[w]=(uint8_t)diff;
    uint8_t seed[6];
    for(unsigned k=0;k<6;k++)seed[k]=(uint8_t)(initial[k]+w*37u+diff*13u);
    memcpy(prior,app,sizeof *prior);
    CK(gk1_app_original_rng_stream(app,argv[2],seed,64,&out)==GK288_OK);
    CK(!memcmp(app,prior,sizeof *app));
    CK(out.authenticated_original&&out.conditional_stream&&out.count==64u);
    CK(!out.actual_startup_seed_proven&&!out.first_spring_generated&&!out.orders_granted);
    CK(out.difficulty==diff&&memcmp(out.before,seed,6u)==0);
    Gk137StockRng r={0};CK(gk137_from_nes_bytes(&r,seed)==GK137_OK);
    for(unsigned i=0;i<64;i++){
     uint16_t val=0;
     CK(gk137_draw15(&r,&val)==GK137_OK);
     CK(out.draw15[i]==val);
     CK(out.rate_candidates[i]==50u+val%(94u+7u*diff));
    }
    uint8_t after[6];gk137_to_nes_bytes(&r,after);
    CK(memcmp(after,out.after,6u)==0);
    Gk288Stream replay={0};
    CK(gk1_app_original_rng_stream(app,argv[2],seed,64,&replay)==GK288_OK);
    CK(memcmp(&replay,&out,sizeof out)==0);
    Gk280RateCandidate first={0};
    CK(gk1_app_original_rate_candidate(app,argv[2],seed,&first)==GK280_OK);
    CK(first.raw_draw15==out.draw15[0]&&first.source_rate_candidate==out.rate_candidates[0]);
   }
  }
  /* Refuse a corrupt ROM without mutating a prior valid result. */
  unchanged=out;
  CK(gk1_app_original_rng_stream(app,"/missing/private.nes",initial,64,&out)==GK288_ROM);
  CK(memcmp(&out,&unchanged,sizeof out)==0);
 }
 printf("GK288 %u assertions PASS; private original C289 stream %s; first Spring NOT generated\n",checks,argc==3&&argv[2][0]?"SOURCE_EXECUTED":"NOT RUN");
 free(dataset);free(prior);free(app);return 0;
}
