#include "gk111_stock_spring_audit.h"
#include "gk1_app_state.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
#define CK(c) do{if(!(c)){fprintf(stderr,"GK111 FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static int check_one(const char *csv,const Gk1TemplateDataset *data,
  const char *private_raw,Gk77Profile profile){
 Gk1LoadedScenario pre={0};Gk1ScenarioId scenario=
   profile==GK77_ARCHIVED_WORLD_SAVERAM?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST;
 CK(gk1_open_loaded_scenario(data,scenario,&pre)==GK1_LOAD_OK);
 Gk79Checkpoint cp={0};
 CK(gk79_open_original(&pre,private_raw,profile,0,&cp)==GK79_OK);
 CK(gk85_raw_source_intact(&cp)&&gk88_stock_projection_intact(&cp));
 Gk111StockSpringAudit audit={0};
 Gk79Checkpoint original_cp=cp;Gk1LoadedScenario original_pre=pre;
 CK(gk111_compare_stock_spring_world(csv,&pre,&cp,&audit)==GK111_OK);
 CK(memcmp(&cp,&original_cp,sizeof cp)==0);
 CK(memcmp(&pre,&original_pre,sizeof pre)==0);
 CK(audit.country_count==(scenario==GK1_MONGOL_CONQUEST?14:27));
 CK(audit.profile==profile&&audit.setup_resolved==0&&audit.first_spring_ready==0&&audit.orders_granted==0);
 /* Verify production integration: the inherited Gk1App owns both
    pre-setup worlds AND the loaded authenticated stock continuation. */
 Gk1App app;gk1_app_init(&app);
 CK(gk1_app_install_templates(&app,data));
 app.stock_checkpoint=original_cp;
 Gk1App app_before=app;
 Gk111StockSpringAudit in_app={0};
 CK(gk1_app_stock_spring_audit(&app,csv,&in_app)==GK111_OK);
 CK(memcmp(&audit,&in_app,sizeof audit)==0);
 CK(memcmp(&app,&app_before,sizeof app)==0);
 unsigned wi=scenario==GK1_WORLD_CONQUEST?1u:0u;
 app.worlds[wi].countries[app.worlds[wi].country_count-1].skill++;
 Gk111StockSpringAudit integration_denied;
 memset(&integration_denied,0x17,sizeof integration_denied);
 Gk111StockSpringAudit integration_sentinel=integration_denied;
 CK(gk1_app_stock_spring_audit(&app,csv,&integration_denied)==GK111_TEMPLATE_DRIFT);
 CK(memcmp(&integration_denied,&integration_sentinel,sizeof integration_denied)==0);
 CK(audit.food_exact+audit.food_mismatch==audit.country_count);
 CK(audit.gold_exact+audit.gold_mismatch==audit.country_count);
 CK(audit.troops_exact+audit.troops_mismatch==audit.country_count);
 CK(audit.all_three_exact<=audit.country_count);
 Gk110SpringStage stage={0};
 CK(gk110_prepare_source_finance(csv,&pre,&stage)==GK110_OK);
 unsigned fe=0,ge=0,te=0,both=0,first=0;
 int32_t fs=0,gs=0,ts=0;unsigned fa=0,ga=0,ta=0;
 for(unsigned i=0;i<audit.country_count;i++){
  int32_t f=(int32_t)cp.world.countries[i].goods[10]-stage.country[i].after.food;
  int32_t g=(int32_t)cp.world.countries[i].gold-stage.country[i].after.gold;
  int32_t t=(int32_t)cp.world.countries[i].troops-stage.country[i].after.troops;
  CK(f==audit.food_residual[i]&&g==audit.gold_residual[i]&&t==audit.troops_residual[i]);
  fe+=(f==0);ge+=(g==0);te+=(t==0);both+=(f==0&&g==0&&t==0);
  fs+=f;gs+=g;ts+=t;fa+=f<0?-f:f;ga+=g<0?-g:g;ta+=t<0?-t:t;
  if(!first&&(f||g||t))first=i+1u;
 }
 CK(audit.food_exact==fe&&audit.gold_exact==ge&&audit.troops_exact==te);
 CK(audit.all_three_exact==both&&audit.first_difference_country==first);
 CK(audit.food_residual_total==fs&&audit.gold_residual_total==gs&&audit.troops_residual_total==ts);
 CK(audit.food_absolute_total==fa&&audit.gold_absolute_total==ga&&audit.troops_absolute_total==ta);
 /* Mutating a NONSELECTED original NES record is caught even with unchanged
    displayed country and no accidental success. */
 Gk111StockSpringAudit sentinel;memset(&sentinel,0x5b,sizeof sentinel);
 Gk111StockSpringAudit denied=sentinel;
 cp.stock_raw[cp.world.country_count-1][0]^=1u;
 CK(gk111_compare_stock_spring_world(csv,&pre,&cp,&denied)==GK111_STOCK_REFUSED);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 cp=original_cp;
 cp.world.countries[cp.world.country_count-1].gold++;
 CK(gk111_compare_stock_spring_world(csv,&pre,&cp,&denied)==GK111_STOCK_REFUSED);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 cp=original_cp;
 pre.countries[pre.country_count-1].gold++;
 CK(gk111_compare_stock_spring_world(csv,&pre,&cp,&denied)==GK111_TEMPLATE_DRIFT);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 pre=original_pre;
 pre.phase=(Gk1ScenarioPhase)2;
 CK(gk111_compare_stock_spring_world(csv,&pre,&cp,&denied)==GK111_TEMPLATE_DRIFT);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 pre=original_pre;
 CK(gk111_compare_stock_spring_world("/not-real-original.csv",&pre,&cp,&denied)==GK111_SOURCE_REFUSED);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 CK(gk111_compare_stock_spring_world(NULL,&pre,&cp,&denied)==GK111_ARGUMENT);
 CK(gk111_compare_stock_spring_world(csv,&pre,&cp,NULL)==GK111_ARGUMENT);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 /* A wrong STOCK profile must not be misapplied to another scenario. */
 cp=original_cp;cp.profile=(uint8_t)(profile==GK77_ARCHIVED_WORLD_SAVERAM?
   GK77_FRESH_MONGOL_WORKRAM:GK77_ARCHIVED_WORLD_SAVERAM);
 CK(gk111_compare_stock_spring_world(csv,&pre,&cp,&denied)==GK111_STOCK_REFUSED);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 printf("GK111 profile %u: countries %u full matches %u, FOOD %u GOLD %u TROOPS %u, absolute differences %u/%u/%u (unexplained, not formula)\n",
  (unsigned)profile,(unsigned)audit.country_count,(unsigned)audit.all_three_exact,
  (unsigned)audit.food_exact,(unsigned)audit.gold_exact,(unsigned)audit.troops_exact,
  (unsigned)audit.food_absolute_total,(unsigned)audit.gold_absolute_total,(unsigned)audit.troops_absolute_total);
 return 0;
}
int main(int argc,char **argv){
 CK(argc==2||argc==5);
 CK(gk1_verified_original_csv(argv[1]));
 Gk1TemplateDataset data;
 CK(gk1_template_load(argv[1],&data)==GK1_LOAD_OK);
 /* No private original ROM/checkpoint required for default public test. */
 Gk1LoadedScenario pre={0};
 CK(gk1_open_loaded_scenario(&data,GK1_MONGOL_CONQUEST,&pre)==GK1_LOAD_OK);
 Gk79Checkpoint fake={0};Gk111StockSpringAudit denied,sentinel;
 memset(&denied,0x6c,sizeof denied);sentinel=denied;
 CK(gk111_compare_stock_spring_world(argv[1],&pre,&fake,&denied)==GK111_STOCK_REFUSED);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 fake.valid=1;fake.source_verified=1;fake.profile=GK77_FRESH_MONGOL_WORKRAM;
 CK(gk111_compare_stock_spring_world(argv[1],&pre,&fake,&denied)==GK111_STOCK_REFUSED);
 CK(memcmp(&denied,&sentinel,sizeof denied)==0);
 if(argc==5){
  CK(check_one(argv[1],&data,argv[2],GK77_FRESH_MONGOL_WORKRAM)==0);
  CK(check_one(argv[1],&data,argv[3],GK77_ARCHIVED_MONGOL_SAVERAM)==0);
  CK(check_one(argv[1],&data,argv[4],GK77_ARCHIVED_WORLD_SAVERAM)==0);
 }else puts("GK111 public negative controls PASS; private 3-case original Mesen comparison NOT RUN");
 return 0;
}
