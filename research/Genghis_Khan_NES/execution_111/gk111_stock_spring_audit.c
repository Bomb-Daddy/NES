#include "gk111_stock_spring_audit.h"
#include "gk1_source_identity.h"
#include <string.h>
static uint32_t abs32(int32_t n){return (uint32_t)(n<0?-n:n);}
Gk111Result gk111_compare_stock_spring_world(const char *csv,
 const Gk1LoadedScenario *pristine,const Gk79Checkpoint *stock,
 Gk111StockSpringAudit *out){
 if(!csv||!pristine||!stock||!out)return GK111_ARGUMENT;
 if(!gk1_verified_original_csv(csv))return GK111_SOURCE_REFUSED;
 Gk110SpringStage stage;
 Gk110Result st=gk110_prepare_source_finance(csv,pristine,&stage);
 if(st==GK110_SOURCE_REFUSED)return GK111_SOURCE_REFUSED;
 if(st==GK110_BASELINE_DRIFT)return GK111_TEMPLATE_DRIFT;
 if(st!=GK110_OK)return GK111_FINANCE_REFUSED;
 unsigned scenario=pristine->scenario_id,count=pristine->country_count;
 if(!stock->valid||stock->source_verified!=1u||
    !gk85_raw_source_intact(stock)||!gk88_stock_projection_intact(stock)||
    !gk77_known_source_digest((Gk77Profile)stock->profile,stock->raw_sha256)||
    stock->world.scenario_id!=pristine->scenario_id||
    stock->world.country_count!=count||
    stock->world.phase==GK1_PRE_SETUP_TEMPLATE||
    (stock->profile==GK77_ARCHIVED_WORLD_SAVERAM)!=(scenario==GK1_WORLD_CONQUEST)||
    (stock->profile!=GK77_FRESH_MONGOL_WORKRAM&&
     stock->profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     stock->profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK111_STOCK_REFUSED;
 Gk111StockSpringAudit v={0};
 v.profile=stock->profile;v.scenario_id=(uint8_t)scenario;v.country_count=(uint8_t)count;
 for(unsigned i=0;i<count;i++){
  const Gk1CountryTemplate *actual=&stock->world.countries[i];
  const Gk75SpringResult *projected=&stage.country[i];
  if(actual->country_id!=i+1u||actual->index!=i||
     actual->scenario_id!=scenario)return GK111_STOCK_REFUSED;
  int32_t f=(int32_t)actual->goods[10]-(int32_t)projected->after.food;
  int32_t g=(int32_t)actual->gold-(int32_t)projected->after.gold;
  int32_t t=(int32_t)actual->troops-(int32_t)projected->after.troops;
  v.food_residual[i]=f;v.gold_residual[i]=g;v.troops_residual[i]=t;
  v.food_residual_total+=f;v.gold_residual_total+=g;v.troops_residual_total+=t;
  v.food_absolute_total+=abs32(f);v.gold_absolute_total+=abs32(g);
  v.troops_absolute_total+=abs32(t);
  v.food_exact+=(uint8_t)(f==0);v.gold_exact+=(uint8_t)(g==0);
  v.troops_exact+=(uint8_t)(t==0);v.all_three_exact+=(uint8_t)(f==0&&g==0&&t==0);
  v.food_mismatch+=(uint8_t)(f!=0);v.gold_mismatch+=(uint8_t)(g!=0);
  v.troops_mismatch+=(uint8_t)(t!=0);
  if(!v.first_difference_country&&(f||g||t))v.first_difference_country=(uint8_t)(i+1u);
 }
 /* No phase/order grant and no mutation of source or captured world. */
 *out=v;
 return GK111_OK;
}
