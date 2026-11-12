#include "gk1_app_state.h"
#include "gk220_labor_history.h"
#include "gk1_pre_setup_store.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(c) do{if(!(c)){fprintf(stderr,"GK220 FAIL line %d: %s\n",__LINE__,#c);return __LINE__;}}while(0)
int main(int argc,char **argv){
 CK(argc==2);
 Gk1TemplateDataset data;
 CK(gk1_template_load(argv[1],&data)==GK1_LOAD_OK);
 Gk1App *app=calloc(1,sizeof *app);CK(app!=NULL);
 gk1_app_init(app);CK(gk1_app_install_templates(app,&data));
 unsigned checks=0u;
 for(unsigned world=0;world<2u;world++){
  Gk1LoadedScenario pristine=app->worlds[world];
  app->active_world=(uint8_t)world;
  app->screen=GK1_SCREEN_LABOR_PLAN;app->rules_profile=0;
  app->country_cursor[world]=0u;app->labor_from=0u;app->labor_to=1u;
  CK(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
  CK(app->labor_history.count==1u);
  CK(app->worlds[world].countries[0].troops==pristine.countries[0].troops-1u);
  CK(app->worlds[world].countries[0].town==pristine.countries[0].town+1u);
  CK(memcmp(&app->finance_source_baseline[world],&pristine,sizeof pristine)==0);
  CK(gk1_app_input(app,GK1_INPUT_DRAFT_UNDO_ONE)==1);
  CK(app->labor_history.count==0u);
  CK(memcmp(&pristine,&app->worlds[world],sizeof pristine)==0);
  CK(gk1_app_input(app,GK1_INPUT_DRAFT_UNDO_ONE)==1);
  CK(app->labor_history.count==0u);
  CK(memcmp(&pristine,&app->worlds[world],sizeof pristine)==0);
  ++checks;
 }
 /* Whole-state hostile gates: an unrelated Gold/identity mutation must never
  * be silently rolled back, nor count as a reversible labor draft. */
 app->active_world=0;app->country_cursor[0]=0;
 app->screen=GK1_SCREEN_LABOR_PLAN;app->labor_from=0;app->labor_to=1;
 CK(gk1_app_input(app,GK1_INPUT_CONFIRM));CK(app->labor_history.count==1u);
 Gk1LoadedScenario saved=app->worlds[0];
 app->worlds[0].countries[0].gold++;
 Gk1LoadedScenario tainted=app->worlds[0];
 Gk220History history=app->labor_history;
 CK(gk220_labor_undo(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,0)==GK220_SOURCE_DRIFT);
 CK(!memcmp(&tainted,&app->worlds[0],sizeof tainted));
 CK(!memcmp(&history,&app->labor_history,sizeof history));
 app->worlds[0]=saved;
 CK(gk220_labor_undo(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,1)==GK220_WRONG_COUNTRY);
 CK(!memcmp(&saved,&app->worlds[0],sizeof saved));
 CK(!memcmp(&history,&app->labor_history,sizeof history));
 CK(gk220_labor_undo(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,0)==GK220_OK);
 ++checks;
 /* LIFO sequence across two countries; undo must not touch a later country
  * unless the move is on top of the stack and in that same country. */
 for(unsigned i=0;i<2u;i++){
  app->country_cursor[0]=(uint8_t)i;
  CK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 }
 CK(app->labor_history.count==2u);
 CK(gk220_labor_undo(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,0)==GK220_WRONG_COUNTRY);
 for(int i=1;i>=0;i--){
  app->country_cursor[0]=(uint8_t)i;
  CK(gk1_app_input(app,GK1_INPUT_DRAFT_UNDO_ONE));
 }
 CK(app->labor_history.count==0u);
 CK(!memcmp(&app->worlds[0],&app->finance_source_baseline[0],sizeof app->worlds[0]));
 ++checks;
 /* Bounds and 64-stack rollback: no 65th commit even if source pool valid. */
 for(unsigned i=0;i<GK220_HISTORY_CAP;i++){
  app->labor_from=(uint8_t)(i%2u);app->labor_to=(uint8_t)(1u-i%2u);
  CK(gk1_app_input(app,GK1_INPUT_CONFIRM));
  CK(app->labor_history.count==i+1u);
 }
 CK(app->labor_history.count==GK220_HISTORY_CAP);
 Gk1LoadedScenario full=app->worlds[0];
 CK(gk220_labor_transfer(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,0,0,1)==GK220_FULL);
 CK(!memcmp(&full,&app->worlds[0],sizeof full));
 for(unsigned i=0;i<GK220_HISTORY_CAP;i++){
  CK(gk220_labor_undo(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,0)==GK220_OK);
 }
 CK(app->labor_history.count==0u);
 CK(!memcmp(&app->worlds[0],&app->finance_source_baseline[0],sizeof app->worlds[0]));
 ++checks;
 /* Existing versioned pre-setup save is unchanged. A restored draft cannot
  * contain a phantom undo from the previous in-memory runtime. */
 app->screen=GK1_SCREEN_LABOR_PLAN;app->labor_from=0;app->labor_to=1;
 CK(gk1_app_input(app,GK1_INPUT_CONFIRM));CK(app->labor_history.count==1u);
 uint8_t bytes[GK51_PRE_SETUP_BYTES];
 CK(gk51_pre_setup_encode(app,&data,bytes,sizeof bytes)==GK1_STORE_OK);
 CK(gk51_pre_setup_decode(bytes,sizeof bytes,&data,app)==GK1_STORE_OK);
 CK(app->labor_history.count==0u);
 CK(app->worlds[0].countries[0].troops==app->finance_source_baseline[0].countries[0].troops-1u);
 CK(!app->stock_checkpoint.valid);
 ++checks;
 /* Negative inputs have unchanged output and unchanged source worlds. */
 saved=app->worlds[0];history=app->labor_history;
 CK(gk220_labor_transfer(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,0,1,1)==GK220_INVALID);
 CK(gk220_labor_transfer(&app->finance_source_baseline[0],&app->worlds[0],
   &app->labor_history,0,255,0,1)==GK220_INVALID);
 CK(!memcmp(&saved,&app->worlds[0],sizeof saved));
 CK(!memcmp(&history,&app->labor_history,sizeof history));
 ++checks;
 printf("EXEC0220 source-bound portable undo, 14+27 countries, 64 LIFO edits, corruption/refusal, save/cold-load %u/7 suites PASS; no NES first Spring or orders invented\n",checks);
 free(app);return 0;
}
