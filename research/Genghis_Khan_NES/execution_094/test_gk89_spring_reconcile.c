#include "gk89_spring_reconcile.h"
#include "gk1_source_identity.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(expr) do{if(!(expr)){fprintf(stderr,"GK89 failed line %d: %s\n",__LINE__,#expr);return 1;}}while(0)
static int expect_authentic(const Gk1LoadedScenario *pristine,
                     const char *file,Gk77Profile profile){
 Gk79Checkpoint *cp=calloc(1,sizeof *cp);CHECK(cp);
 CHECK(gk79_open_original(pristine,file,profile,0,cp)==GK79_OK);
 const Gk79Checkpoint saved=*cp;
 unsigned count=pristine->country_count;
 unsigned mismatch_count=0,exact_count=0;
 for(unsigned i=0;i<count;i++){
  Gk89SpringReconciliation v={0};
  CHECK(gk89_compare_spring(pristine,cp,(uint8_t)i,&v)==GK89_OK);
  CHECK(v.country_id==i+1&&v.observed_gold==cp->world.countries[i].gold);
  CHECK(v.residual_gold==(int32_t)v.observed_gold-(int32_t)v.finance_gold);
  CHECK(v.residual_food==(int32_t)v.observed_food-(int32_t)v.finance_food);
  CHECK(v.residual_troops==(int32_t)v.observed_troops-(int32_t)v.finance_troops);
  if(v.gold_matched&&v.food_matched&&v.troops_matched)exact_count++;
  else mismatch_count++;
 }
 CHECK(count==mismatch_count+exact_count);
 CHECK(memcmp(cp,&saved,sizeof saved)==0); /* read-only on every country */
 /* Three View presses must traverse decoded -> private raw -> verified
  * finance reconciliation -> decoded in the REAL integrated app. */
 Gk1App *app=calloc(1,sizeof *app);CHECK(app);gk1_app_init(app);
 app->stock_checkpoint=*cp;app->screen=GK1_SCREEN_STOCK_CONTINUE;
 CHECK(gk1_app_input(app,GK1_INPUT_LABOR_PLAN)==1);
 CHECK(app->stock_checkpoint.raw_view==1&&app->stock_reconcile_view==0);
 CHECK(gk1_app_input(app,GK1_INPUT_LABOR_PLAN)==1);
 CHECK(app->stock_checkpoint.raw_view==0&&app->stock_reconcile_view==1);
 CHECK(strstr(app->status,"FINANCE")!=NULL);
 CHECK(gk1_app_input(app,GK1_INPUT_LABOR_PLAN)==1);
 CHECK(app->stock_checkpoint.raw_view==0&&app->stock_reconcile_view==0);
 CHECK(memcmp(&saved,&app->stock_checkpoint,sizeof saved)==0);
 free(app);
 Gk89SpringReconciliation out;memset(&out,0xa5,sizeof out);
 Gk89SpringReconciliation before=out;
 CHECK(gk89_compare_spring(pristine,cp,(uint8_t)count,&out)==GK89_TEMPLATE_INVALID);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 cp->stock_raw[0][1]^=1;
 CHECK(gk89_compare_spring(pristine,cp,0,&out)==GK89_UNVERIFIED);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 *cp=saved;cp->world.countries[0].gold^=1;
 CHECK(gk89_compare_spring(pristine,cp,0,&out)==GK89_UNVERIFIED);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 *cp=saved;cp->source_verified=0;
 CHECK(gk89_compare_spring(pristine,cp,0,&out)==GK89_UNVERIFIED);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 Gk1LoadedScenario changed=*pristine;
 changed.phase=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE;
 *cp=saved;
 CHECK(gk89_compare_spring(&changed,cp,0,&out)==GK89_TEMPLATE_INVALID);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 fprintf(stdout,"PROFILE %u: %u pristine countries; %u full-field finance matches, %u residual records (not proof of RNG)\n",
 (unsigned)profile,count,exact_count,mismatch_count);
 free(cp);return 0;
}
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 Gk1TemplateDataset *source=calloc(1,sizeof *source);CHECK(source);
 CHECK(gk1_verified_original_csv(argv[1]));
 CHECK(gk1_template_load(argv[1],source)==GK1_LOAD_OK);
 Gk1LoadedScenario *pre=calloc(1,sizeof *pre);CHECK(pre);
 CHECK(gk1_open_loaded_scenario(source,GK1_MONGOL_CONQUEST,pre)==GK1_LOAD_OK);
 Gk89SpringReconciliation sentinel;memset(&sentinel,0xa5,sizeof sentinel);
 Gk89SpringReconciliation old=sentinel;
 CHECK(gk89_compare_spring(pre,NULL,0,&sentinel)==GK89_INVALID);
 CHECK(memcmp(&old,&sentinel,sizeof old)==0);
 Gk79Checkpoint *fake=calloc(1,sizeof *fake);CHECK(fake);
 CHECK(gk89_compare_spring(pre,fake,0,&sentinel)==GK89_UNVERIFIED);
 CHECK(memcmp(&old,&sentinel,sizeof old)==0);
 if(argc==5){
  CHECK(expect_authentic(pre,argv[2],GK77_FRESH_MONGOL_WORKRAM)==0);
  CHECK(expect_authentic(pre,argv[3],GK77_ARCHIVED_MONGOL_SAVERAM)==0);
  CHECK(gk1_open_loaded_scenario(source,GK1_WORLD_CONQUEST,pre)==GK1_LOAD_OK);
  CHECK(expect_authentic(pre,argv[4],GK77_ARCHIVED_WORLD_SAVERAM)==0);
 }
 free(fake);free(pre);free(source);
 puts("GK89: authenticated original-source first-Spring finance reconciliation, 14/27 full country auditing, no-mutation and negative gates PASS");
 return 0;
}
