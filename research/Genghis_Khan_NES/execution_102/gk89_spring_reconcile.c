#include "gk89_spring_reconcile.h"
#include <string.h>
Gk89Status gk89_compare_spring(const Gk1LoadedScenario *baseline,
                              const Gk79Checkpoint *stock,
                              uint8_t index,Gk89SpringReconciliation *out){
 if(!baseline||!stock||!out)return GK89_INVALID;
 /* This gate prevents synthetic fixtures, forged caller-controlled digests,
  * partial country sets, and altered decoded fields from passing as NES. */
 if(!stock->valid||stock->source_verified!=1u||
    !gk85_raw_source_intact(stock))return GK89_UNVERIFIED;
 const unsigned count=stock->world.country_count;
 if((count!=14u&&count!=27u)||index>=count||
    baseline->phase!=GK1_PRE_SETUP_TEMPLATE||
    baseline->scenario_id!=stock->world.scenario_id||
    baseline->country_count!=count)return GK89_TEMPLATE_INVALID;
 const Gk1CountryTemplate *src=&baseline->countries[index];
 const Gk1CountryTemplate *actual=&stock->world.countries[index];
 if(src->scenario_id!=stock->world.scenario_id||
    src->country_id!=index+1u||src->index!=index||
    actual->country_id!=index+1u||actual->index!=index)
  return GK89_TEMPLATE_INVALID;
 Gk75SpringResult finance;
 if(gk75_template_projection(src,&finance)!=GK75_OK)
  return GK89_FINANCE_UNAVAILABLE;
 Gk89SpringReconciliation next={0};
 next.country_id=(uint8_t)(index+1u);
 next.template_food=src->goods[10];
 next.template_gold=src->gold;
 next.template_troops=src->troops;
 next.finance_food=finance.after.food;
 next.finance_gold=finance.after.gold;
 next.finance_troops=finance.after.troops;
 next.observed_food=actual->goods[10];
 next.observed_gold=actual->gold;
 next.observed_troops=actual->troops;
 next.residual_food=(int32_t)next.observed_food-(int32_t)next.finance_food;
 next.residual_gold=(int32_t)next.observed_gold-(int32_t)next.finance_gold;
 next.residual_troops=(int32_t)next.observed_troops-(int32_t)next.finance_troops;
 next.food_matched=(uint8_t)(next.residual_food==0);
 next.gold_matched=(uint8_t)(next.residual_gold==0);
 next.troops_matched=(uint8_t)(next.residual_troops==0);
 /* All checks precede write; caller's output is byte-identical on refusal. */
 *out=next;
 return GK89_OK;
}
