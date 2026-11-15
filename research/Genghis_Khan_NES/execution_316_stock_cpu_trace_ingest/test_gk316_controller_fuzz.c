/* EXEC0316: Stateful controller/campaign invariants, with reproducible PRNG.
 * Exercises the exact inherited native Gk1App; not a new strategy prototype. */
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static uint32_t rng = UINT32_C(0x316f16a9);
static uint32_t next32(void){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
#define CHECK(cond) do {if(!(cond)){fprintf(stderr,"GK316 FAIL step=%u seed=%08x %s:%d %s\n",step,rng,__FILE__,__LINE__,#cond);return 4;}}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 2;
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *app=malloc(sizeof *app);
 if(!dataset||!app)return 2;
 if(gk1_template_load(argv[1],dataset)!=GK1_LOAD_OK)return 3;
 gk1_app_init(app);
 if(!gk1_app_install_templates(app,dataset))return 3;
 Gk1LoadedScenario original0=app->finance_source_baseline[0];
 Gk1LoadedScenario original1=app->finance_source_baseline[1];
 const unsigned iterations=300000;
 for(unsigned step=0;step<iterations;step++){
  Gk1Input input=(Gk1Input)(1u+(next32()%10u));
  (void)gk1_app_input(app,input);
  CHECK(app->rules_profile<=1);
  CHECK(memcmp(&original0,&app->finance_source_baseline[0],sizeof original0)==0);
  CHECK(memcmp(&original1,&app->finance_source_baseline[1],sizeof original1)==0);
  CHECK(app->active_world<2);
  CHECK(app->world_ready[0] && app->world_ready[1]);
  for(unsigned w=0;w<2;w++){
   CHECK(app->worlds[w].phase==GK1_PRE_SETUP_TEMPLATE);
   CHECK(app->worlds[w].country_count==(w==0?14:27));
   CHECK(app->finance_source_baseline[w].country_count==(w==0?14:27));
   CHECK(app->setup_candidate[w]==255 || app->setup_candidate[w] < app->worlds[w].country_count);
   CHECK(app->setup_stage[w]<=GK53_SETUP_AWAIT_SOURCE);
   CHECK(app->country_cursor[w]<app->worlds[w].country_count);
   CHECK(app->setup_player_count[w]>=1 && app->setup_player_count[w]<=(w==0?1:4));
   CHECK(app->setup_difficulty[w]>=1 && app->setup_difficulty[w]<=5);
   for(unsigned i=0;i<4;i++){
    uint8_t lord=app->setup_rulers[w][i];
    CHECK(lord==255 || gk53_setup_allowed_lord((uint8_t)w,lord));
    for(unsigned j=0;j<i;j++) CHECK(lord==255 || lord!=app->setup_rulers[w][j]);
   }
  }
  CHECK(!app->stock_checkpoint.valid);
  CHECK(app->stock_checkpoint.pending_pass==0);
  CHECK(app->spring_stock_count==0);
  CHECK(app->screen != GK1_SCREEN_STOCK_CONTINUE);
  CHECK(app->screen != GK1_SCREEN_STOCK_COMMANDS);
  CHECK(app->screen != GK1_SCREEN_STOCK_TRADE_REFERENCE);
  CHECK(app->status[sizeof(app->status)-1]==0 || memchr(app->status,0,sizeof app->status)!=NULL);
 }
 printf("GK316 PASS: %u hostile controller inputs; Classic/Enhanced PRE_SETUP never falsely unlocks original Spring\n",iterations);
 free(app);free(dataset);return 0;
}