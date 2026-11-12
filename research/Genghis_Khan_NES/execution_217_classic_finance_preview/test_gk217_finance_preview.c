#include "gk1_app_state.h"
#include "gk217_finance_preview.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;
 Gk1TemplateDataset data;
 if(gk1_template_load(argv[1],&data)!=GK1_LOAD_OK)return 3;
 Gk1App app;gk1_app_init(&app);
 if(!gk1_app_install_templates(&app,&data))return 4;
 unsigned cases=0;
 for(unsigned w=0;w<2;w++){
  const Gk1LoadedScenario original=app.worlds[w];
  Gk214Batch all;
  if(gk214_project_spring_batch(&app.worlds[w],&all)!=GK214_OK)return 5;
  for(unsigned i=0;i<original.country_count;i++){
   Gk217Preview view;
   if(gk217_preview_country(&app.worlds[w],i,&view)!=GK217_OK)return 6;
   if(view.country_count!=original.country_count||view.selected_country!=i||
      view.original_spring_generated||view.before_gold!=original.countries[i].gold||
      view.after_gold!=all.after.countries[i].gold||
      view.before_food!=original.countries[i].goods[10]||
      view.after_food!=all.after.countries[i].goods[10]||
      view.total_taxes!=all.gross_taxes||view.total_payroll!=all.payroll||
      view.total_food_consumed!=all.food_consumed)return 7;
   ++cases;
  }
  if(memcmp(&original,&app.worlds[w],sizeof original))return 8;
  Gk217Preview untouched;memset(&untouched,0xA5,sizeof untouched);
  Gk217Preview marker=untouched;
  if(gk217_preview_country(&app.worlds[w],original.country_count,&untouched)==GK217_OK||
     memcmp(&marker,&untouched,sizeof marker))return 9;
  Gk1LoadedScenario corrupt=original;
  corrupt.countries[original.country_count-1].country_id=0;
  if(gk217_preview_country(&corrupt,0,&untouched)==GK217_OK||
     memcmp(&marker,&untouched,sizeof marker))return 10;
  corrupt=original;corrupt.phase=(Gk1ScenarioPhase)2;
  if(gk217_preview_country(&corrupt,0,&untouched)==GK217_OK||
     memcmp(&marker,&untouched,sizeof marker))return 11;
 }
 /* Controller-only path, Classic vs Enhanced separation, actual D-pad scan,
  * immutable original world and no invented playable Spring/merchant offers. */
 app.screen=GK1_SCREEN_LABOR_PLAN;app.active_world=0;app.rules_profile=0;
 Gk1LoadedScenario before=app.worlds[0];
 if(!gk1_app_input(&app,GK1_INPUT_SETUP)||
    app.screen!=GK1_SCREEN_LABOR_PLAN)return 12;
 if(!gk1_app_input(&app,GK1_INPUT_SETUP)||
    app.screen!=GK1_SCREEN_SPRING_FINANCE_PREVIEW)return 12;
 if(!gk1_app_input(&app,GK1_INPUT_DOWN)||app.finance_cursor!=1u)return 13;
 if(!gk1_app_input(&app,GK1_INPUT_LEFT)||app.finance_cursor!=0u)return 14;
 if(!gk1_app_input(&app,GK1_INPUT_CANCEL)||
    app.screen!=GK1_SCREEN_LABOR_PLAN)return 15;
 if(memcmp(&before,&app.worlds[0],sizeof before)||
    app.stock_checkpoint.valid||app.setup_stage[0]==GK53_SETUP_AWAIT_SOURCE)return 16;
 app.active_world=1;app.rules_profile=0;app.screen=GK1_SCREEN_LABOR_PLAN;
 if(!gk1_app_input(&app,GK1_INPUT_SETUP)||
    app.screen!=GK1_SCREEN_LABOR_PLAN)return 17;
 if(!gk1_app_input(&app,GK1_INPUT_SETUP)||
    app.screen!=GK1_SCREEN_SPRING_FINANCE_PREVIEW)return 17;
 if(app.finance_cursor!=0u)return 18;
 if(!gk1_app_input(&app,GK1_INPUT_CANCEL))return 19;
 app.rules_profile=1;app.screen=GK1_SCREEN_LABOR_PLAN;
 if(!gk1_app_input(&app,GK1_INPUT_SETUP)||
    app.screen!=GK1_SCREEN_DEMOBILIZE)return 20;
 printf("EXEC0217 14+27=%u countries; Classic controller finance preview, source-bounded atomic refusal, Enhanced path preserved: PASS\n",cases);
 return cases==41u?0:21;
}
