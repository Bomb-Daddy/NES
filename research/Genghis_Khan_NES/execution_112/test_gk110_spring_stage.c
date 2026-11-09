#include "gk110_spring_stage.h"
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define CK(c) do{if(!(c)){fprintf(stderr,"GK110 line %u FAIL: %s\n",(unsigned)__LINE__,#c);return 1;}}while(0)
static int check_world(const char *csv,const Gk1TemplateDataset *data,Gk1ScenarioId id){
 Gk1LoadedScenario world;
 CK(gk1_open_loaded_scenario(data,id,&world)==GK1_LOAD_OK);
 Gk1LoadedScenario original=world;
 Gk110SpringStage output;
 memset(&output,0xa5,sizeof output);
 CK(gk110_prepare_source_finance(csv,&world,&output)==GK110_OK);
 CK(memcmp(&world,&original,sizeof world)==0);
 CK(output.scenario_id==(uint8_t)id);
 CK(output.country_count==(id==GK1_MONGOL_CONQUEST?14:27));
 CK(output.setup_resolved==0&&output.first_spring_ready==0&&output.orders_granted==0);
 CK(output.food_before>0&&output.food_consumed>0&&output.tax_total>0);
 uint32_t f0=0,f1=0,g0=0,g1=0,t0=0,t1=0,tot_tax=0,cons=0,pay=0;
 for(unsigned i=0;i<output.country_count;i++){
  const Gk1CountryTemplate *c=&world.countries[i];
  Gk75SpringResult direct={0};
  CK(gk75_template_projection(c,&direct)==GK75_OK);
  CK(memcmp(&direct,&output.country[i],sizeof direct)==0);
  f0+=c->goods[10];f1+=direct.after.food;g0+=c->gold;g1+=direct.after.gold;
  t0+=c->troops;t1+=direct.after.troops;
  tot_tax+=direct.gross_tax;cons+=direct.food_consumed;pay+=direct.payroll;
 }
 CK(output.food_before==f0&&output.food_after==f1);
 CK(output.gold_before==g0&&output.gold_after==g1);
 CK(output.troops_before==t0&&output.troops_after==t1);
 CK(output.tax_total==tot_tax&&output.food_consumed==cons&&output.payroll_total==pay);
 /* Failure must be atomic, including after 26 valid countries. */
 Gk110SpringStage guard;memset(&guard,0x6b,sizeof guard);
 Gk110SpringStage sentinel=guard;
 Gk1LoadedScenario drift=world;
 drift.countries[drift.country_count-1].lord_name[0]^=1;
 CK(gk110_prepare_source_finance(csv,&drift,&guard)==GK110_BASELINE_DRIFT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 drift=world;drift.countries[drift.country_count-1].gold++;
 CK(gk110_prepare_source_finance(csv,&drift,&guard)==GK110_BASELINE_DRIFT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 drift=world;drift.countries[0].goods[0]++;
 CK(gk110_prepare_source_finance(csv,&drift,&guard)==GK110_BASELINE_DRIFT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 drift=world;drift.phase=(Gk1ScenarioPhase)2;
 CK(gk110_prepare_source_finance(csv,&drift,&guard)==GK110_BASELINE_DRIFT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 drift=world;drift.country_count--;
 CK(gk110_prepare_source_finance(csv,&drift,&guard)==GK110_BASELINE_DRIFT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 drift=world;drift.countries[0].country_rom_offset++;
 CK(gk110_prepare_source_finance(csv,&drift,&guard)==GK110_BASELINE_DRIFT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 CK(gk110_prepare_source_finance("/invalid/original-country-identity.csv",&world,&guard)==GK110_SOURCE_REFUSED);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 CK(gk110_prepare_source_finance(NULL,&world,&guard)==GK110_ARGUMENT);
 CK(gk110_prepare_source_finance(csv,NULL,&guard)==GK110_ARGUMENT);
 CK(gk110_prepare_source_finance(csv,&world,NULL)==GK110_ARGUMENT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 /* Integration in the actual app, not a separate isolated transaction. */
 Gk1App app;gk1_app_init(&app);CK(gk1_app_install_templates(&app,data));
 app.active_world=(uint8_t)(id-1);
 Gk1App app_before=app;Gk110SpringStage integrated={0};
 CK(gk1_app_source_spring_stage(&app,csv,&integrated)==GK110_OK);
 CK(memcmp(&integrated,&output,sizeof integrated)==0);
 CK(memcmp(&app,&app_before,sizeof app)==0);
 CK(app.worlds[app.active_world].phase==GK1_PRE_SETUP_TEMPLATE);
 app.worlds[app.active_world].countries[0].food_workers++;
 CK(gk1_app_source_spring_stage(&app,csv,&guard)==GK110_BASELINE_DRIFT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 app.active_world=3;
 CK(gk1_app_source_spring_stage(&app,csv,&guard)==GK110_ARGUMENT);
 CK(memcmp(&guard,&sentinel,sizeof guard)==0);
 /* Controller navigation in the inherited game: reach both scenarios'
  * original Spring source-projection screen, no phase/order unlock. */
 Gk1App navigation;gk1_app_init(&navigation);
 CK(gk1_app_install_templates(&navigation,data));
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* title -> campaigns */
 if(id==GK1_WORLD_CONQUEST)CK(gk1_app_input(&navigation,GK1_INPUT_DOWN));
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* campaign -> roster */
 CK(navigation.active_world==(uint8_t)(id-1));
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* first selectable country */
 CK(gk1_app_input(&navigation,GK1_INPUT_SETUP));
 CK(navigation.screen==GK1_SCREEN_SETUP_WIZARD);
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* players */
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* difficulty */
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* lord */
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* automatic abilities */
 CK(navigation.setup_stage[id-1]==GK53_SETUP_AWAIT_SOURCE);
 CK(gk1_app_input(&navigation,GK1_INPUT_LABOR_PLAN)); /* VIEW manual rules */
 CK(navigation.screen==GK1_SCREEN_MANUAL_RULES);
 Gk1LoadedScenario both_worlds[2];memcpy(both_worlds,navigation.worlds,sizeof both_worlds);
 CK(gk1_app_input(&navigation,GK1_INPUT_CONFIRM)); /* original Spring archive */
 CK(navigation.screen==GK1_SCREEN_SPRING_ARCHIVE);
 CK(gk1_app_input(&navigation,GK1_INPUT_SETUP)); /* bounded finance stage */
 CK(navigation.spring_preview_mode==1&&navigation.spring_stock_mode==0);
 for(unsigned i=1;i<output.country_count;i++)
   CK(gk1_app_input(&navigation,GK1_INPUT_DOWN));
 CK(navigation.spring_archive_cursor==output.country_count-1);
 CK(gk1_app_source_spring_stage(&navigation,csv,&integrated)==GK110_OK);
 CK(integrated.country_count==output.country_count);
 CK(gk1_app_input(&navigation,GK1_INPUT_SETUP)); /* return archive: safe reset */
 CK(navigation.spring_preview_mode==0&&navigation.spring_archive_cursor==(output.country_count==14?13u:0u));
 CK(gk1_app_input(&navigation,GK1_INPUT_CANCEL));
 CK(navigation.screen==GK1_SCREEN_MANUAL_RULES);
 CK(memcmp(both_worlds,navigation.worlds,sizeof both_worlds)==0);
 CK(navigation.worlds[id-1].phase==GK1_PRE_SETUP_TEMPLATE);
 return 0;
}
int main(int argc,char **argv){
 CK(argc==2);
 CK(gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset all;
 CK(gk1_template_load(argv[1],&all)==GK1_LOAD_OK);
 CK(check_world(argv[1],&all,GK1_MONGOL_CONQUEST)==0);
 CK(check_world(argv[1],&all,GK1_WORLD_CONQUEST)==0);
 puts("GK110 authentic-source full 14+27 finance staging and app integration PASS; first playable Spring remains NOT IMPLEMENTED");
 return 0;
}
