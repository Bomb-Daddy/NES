#include "gk78_spring_world.h"
#include <string.h>
#include <limits.h>

static int valid_country(const Gk1CountryTemplate *c,unsigned scenario,unsigned index){
 if(c->scenario_id!=scenario || c->country_id!=index+1u||c->index!=index ||
    c->tax>100 || c->eligibility_percent>100 || c->extra_tax_used>1)return 0;
 unsigned pop=(unsigned)c->troops+c->town+c->masons+c->artisans+c->food_workers;
 return pop<=999 && c->drafted_population==pop &&
        pop+c->undrafted_reserve==c->combined_population &&
        c->eligible_draft_count==(unsigned)c->undrafted_reserve*c->eligibility_percent/100u;
}
Gk78Result gk78_apply_spring_finance(Gk1App *app,uint8_t index,
                                    Gk78SpringReceipt *receipt){
 if(!app||!receipt||index>1||!app->world_ready[index])return GK78_BAD_INPUT;
 const Gk1LoadedScenario *original=&app->worlds[index];
 unsigned scenario=index==0?GK1_MONGOL_CONQUEST:GK1_WORLD_CONQUEST;
 unsigned count=index==0?14u:27u;
 if((unsigned)original->scenario_id!=scenario||original->country_count!=count)
  return GK78_BAD_INPUT;
 if((unsigned)original->phase!=GK71_RESERVED_VERIFIED_PLAYABLE_PHASE)
  return GK78_NOT_INITIALIZED;
 Gk1LoadedScenario stage=*original;Gk78SpringReceipt next={0};
 next.countries=(uint16_t)count;
 for(unsigned i=0;i<count;i++){
  Gk1CountryTemplate *c=&stage.countries[i];
  if(!valid_country(c,scenario,i))return GK78_INCONSISTENT_COUNTRY;
  Gk75SpringResult outcome;
  if(gk75_template_projection(c,&outcome)!=GK75_OK)
   return GK78_SOURCE_FORMULA_REJECTED;
  /* Rollback already occurs because stage is private. Numeric totals are
   * bounded as a real-world condition, not silently wrapped. */
  unsigned aggregate[]={
     next.food_used+outcome.food_consumed,
     next.tax_gross+outcome.gross_tax,
     next.payroll+outcome.payroll,
     next.troops_lost+outcome.troops_lost,
     next.starved_countries+outcome.starved
  };
  for(unsigned j=0;j<5;j++)if(aggregate[j]>UINT16_MAX)return GK78_SOURCE_FORMULA_REJECTED;
  next.food_used=(uint16_t)aggregate[0];next.tax_gross=(uint16_t)aggregate[1];
  next.payroll=(uint16_t)aggregate[2];next.troops_lost=(uint16_t)aggregate[3];
  next.starved_countries=(uint16_t)aggregate[4];
  next.source_food_before[i]=c->goods[10];next.source_gold_before[i]=c->gold;
  c->troops=outcome.after.troops;c->town=outcome.after.town;
  c->masons=outcome.after.masons;c->artisans=outcome.after.artisans;
  c->food_workers=outcome.after.food_workers;c->goods[10]=outcome.after.food;
  c->gold=outcome.after.gold;
  unsigned drafted=(unsigned)c->troops+c->town+c->masons+c->artisans+c->food_workers;
  c->drafted_population=(uint16_t)drafted;
  c->combined_population=(uint16_t)(drafted+c->undrafted_reserve);
  if(!valid_country(c,scenario,i))return GK78_INCONSISTENT_COUNTRY;
  next.new_food[i]=c->goods[10];next.new_gold[i]=c->gold;
  next.new_troops[i]=c->troops;
 }
 /* No spring advance, no unverified common economy/AI/ability/RNG pass.
  * This atomic transaction is only one verified part of the Spring chain. */
 app->worlds[index]=stage;
 *receipt=next;
 return GK78_OK;
}
