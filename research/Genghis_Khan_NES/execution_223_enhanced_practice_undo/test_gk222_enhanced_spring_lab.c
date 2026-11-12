#include "gk222_enhanced_spring_lab.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ASSERT(q) do{if(!(q)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#q);return 1;}}while(0)
static int check(const Gk1LoadedScenario *source){
 Gk222Spring *d=calloc(1,sizeof *d);
 ASSERT(d!=NULL);
 Gk1LoadedScenario before=*source;
 Gk214Batch preview;
 ASSERT(gk214_project_spring_batch(source,&preview)==GK214_OK);
 ASSERT(gk222_open(source,0,d)==GK222_OK);
 ASSERT(d->active&&d->remaining==3u&&!d->ended);
 ASSERT(d->working.country_count==source->country_count);
 ASSERT(memcmp(&d->working,&preview.after,sizeof preview.after)==0);
 ASSERT(memcmp(source,&before,sizeof before)==0); /* never edit original */
 ASSERT(d->taxes==preview.gross_taxes);
 ASSERT(d->payroll==preview.payroll);
 ASSERT(d->food_used==preview.food_consumed);
 Gk222Spring snapshot=*d;
 ASSERT(gk222_apply(d,1,GK222_PASS)==GK222_WRONG_COUNTRY);
 ASSERT(memcmp(d,&snapshot,sizeof snapshot)==0);
 ASSERT(gk222_apply(d,0,(Gk222Action)99)==GK222_BAD_ACTION);
 ASSERT(memcmp(d,&snapshot,sizeof snapshot)==0);
 unsigned old_town=d->working.countries[0].town;
 unsigned old_food=d->working.countries[0].food_workers;
 if(old_town>0 && old_food<999){
   ASSERT(gk222_apply(d,0,GK222_ASSIGN_TOWN_TO_FOOD)==GK222_OK);
   ASSERT(d->working.countries[0].town==old_town-1);
   ASSERT(d->working.countries[0].food_workers==old_food+1);
   ASSERT(d->remaining==2);
 }else {
   ASSERT(gk222_apply(d,0,GK222_PASS)==GK222_OK);
   ASSERT(d->remaining==2);
 }
 ASSERT(gk222_apply(d,0,GK222_PASS)==GK222_OK);
 ASSERT(gk222_apply(d,0,GK222_PASS)==GK222_OK);
 ASSERT(d->remaining==0&&d->ended&&d->actions_committed==3);
 snapshot=*d;
 ASSERT(gk222_verify_receipts(d)==GK222_OK);
 ASSERT(gk222_undo_last(d)==GK222_OK);
 ASSERT(d->remaining==1u&&!d->ended&&d->actions_committed==2u);
 ASSERT(gk222_undo_last(d)==GK222_OK);
 ASSERT(d->remaining==2u&&d->actions_committed==1u);
 ASSERT(gk222_undo_last(d)==GK222_OK);
 ASSERT(d->remaining==3u&&d->actions_committed==0u);
 ASSERT(d->working.countries[0].town==old_town);
 ASSERT(d->working.countries[0].food_workers==old_food);
 Gk222Spring clean=*d;
 ASSERT(gk222_undo_last(d)==GK222_NO_HISTORY);
 ASSERT(memcmp(d,&clean,sizeof clean)==0);
 ASSERT(gk222_apply(d,0,GK222_PASS)==GK222_OK);
 ASSERT(gk222_apply(d,0,GK222_PASS)==GK222_OK);
 ASSERT(gk222_apply(d,0,GK222_PASS)==GK222_OK);
 snapshot=*d;
 Gk222Spring forged=*d;
 forged.remaining=1;ASSERT(gk222_verify_receipts(&forged)==GK222_CORRUPT_HISTORY);
 ASSERT(gk222_apply(&forged,0,GK222_PASS)==GK222_CORRUPT_HISTORY);
 ASSERT(gk222_undo_last(&forged)==GK222_CORRUPT_HISTORY);
 forged=*d;forged.working.countries[0].food_workers++;
 ASSERT(gk222_verify_receipts(&forged)==GK222_CORRUPT_HISTORY);
 forged=*d;forged.order_receipts[2]=99u;
 ASSERT(gk222_verify_receipts(&forged)==GK222_CORRUPT_HISTORY);
 ASSERT(gk222_apply(d,0,GK222_PASS)==GK222_NO_ORDERS);
 ASSERT(memcmp(d,&snapshot,sizeof snapshot)==0);
 ASSERT(memcmp(source,&before,sizeof before)==0);
 ASSERT(gk222_open(source,source->country_count,d)==GK222_BAD_PLAYER);
 ASSERT(memcmp(d,&snapshot,sizeof snapshot)==0);
 Gk1LoadedScenario broken=*source;broken.phase=(Gk1ScenarioPhase)2;
 ASSERT(gk222_open(&broken,0,d)==GK222_BAD_SOURCE);
 ASSERT(memcmp(d,&snapshot,sizeof snapshot)==0);
 broken=*source;broken.countries[3].country_id=0;
 ASSERT(gk222_open(&broken,0,d)==GK222_BAD_SOURCE);
 ASSERT(memcmp(d,&snapshot,sizeof snapshot)==0);
 free(d);return 0;
}
int main(int argc,char **argv){
 ASSERT(argc==2);
 Gk1TemplateDataset all;
 ASSERT(gk1_template_load(argv[1],&all)==GK1_LOAD_OK);
 Gk1LoadedScenario m,w;
 ASSERT(gk1_open_loaded_scenario(&all,GK1_MONGOL_CONQUEST,&m)==GK1_LOAD_OK);
 ASSERT(gk1_open_loaded_scenario(&all,GK1_WORLD_CONQUEST,&w)==GK1_LOAD_OK);
 ASSERT(check(&m)==0);ASSERT(check(&w)==0);
 Gk1App *app=calloc(1,sizeof *app);ASSERT(app);
 gk1_app_init(app);
 ASSERT(gk1_app_install_templates(app,&all));
 app->rules_profile=0; /* Explicit Classic negative control (default is Enhanced). */
 /* No forged Classic game launch: refusal must preserve state. */
 Gk1App *snapshot=malloc(sizeof *snapshot);ASSERT(snapshot);
 *snapshot=*app;
 ASSERT(gk1_app_enhanced_spring_lab_open(app)!=GK222_OK);
 ASSERT(memcmp(app,snapshot,sizeof *app)==0);
 app->rules_profile=1;app->active_world=0;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_candidate[0]=0;
 app->setup_ability_method[0]=1;
 ASSERT(gk1_app_enhanced_spring_lab_open(app)==GK222_OK);
 ASSERT(app->screen==GK1_SCREEN_ENHANCED_SPRING_LAB);
 ASSERT(app->port_spring.active&&app->port_spring.remaining==3);
 ASSERT(gk1_app_input(app,GK1_INPUT_CONFIRM));
 ASSERT(app->port_spring.remaining==2);
 ASSERT(gk1_app_input(app,GK1_INPUT_RIGHT));
 ASSERT(gk1_app_input(app,GK1_INPUT_CONFIRM));
 ASSERT(app->port_spring.remaining<=2);
 ASSERT(gk1_app_input(app,GK1_INPUT_DRAFT_UNDO_ONE));
 ASSERT(app->port_spring.actions_committed==1u);
 ASSERT(app->port_spring.remaining==2u);
 ASSERT(gk1_app_input(app,GK1_INPUT_CANCEL));
 ASSERT(app->screen==GK1_SCREEN_SETUP_WIZARD);
 free(snapshot);free(app);
 puts("GK222 both-country budget, rollback, profile isolation and controller PASS");return 0;
}
