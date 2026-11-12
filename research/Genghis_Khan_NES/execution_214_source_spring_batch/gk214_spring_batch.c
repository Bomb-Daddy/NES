#include "gk214_spring_batch.h"
#include <string.h>
Gk214Status gk214_project_spring_batch(const Gk1LoadedScenario *prepared,
 Gk214Batch *out){
 if(!prepared||!out||
    (prepared->scenario_id!=GK1_MONGOL_CONQUEST&&
     prepared->scenario_id!=GK1_WORLD_CONQUEST)||
    prepared->country_count!=(prepared->scenario_id==GK1_MONGOL_CONQUEST?
      GK1_COUNTRIES_MONGOL:GK1_COUNTRIES_WORLD))return GK214_BAD_INPUT;
 Gk214Batch next={0};next.after=*prepared;
 for(unsigned i=0;i<prepared->country_count;i++){
  const Gk1CountryTemplate *c=&prepared->countries[i];
  if(c->scenario_id!=(uint8_t)prepared->scenario_id||
     c->country_id!=i+1u||c->index!=i||
     c->specialty_index>=10u)return GK214_BAD_COUNTRY;
  Gk75SpringFields before={c->troops,c->town,c->masons,
   c->artisans,c->food_workers,c->goods[10],c->gold,
   c->economy,c->morale,c->tax};
  Gk75SpringResult result={0};
  if(gk75_spring_food_tax_pay(&before,&result)!=GK75_OK)
   return GK214_BAD_FINANCE;
  next.country[i]=result;
  Gk1CountryTemplate *target=&next.after.countries[i];
  target->troops=result.after.troops;
  target->town=result.after.town;
  target->masons=result.after.masons;
  target->artisans=result.after.artisans;
  target->food_workers=result.after.food_workers;
  target->gold=result.after.gold;
  target->goods[10]=result.after.food;
  /* The source-derived engine concerns only food, tax and wage. Other
   * derived fields (e.g. civilian draft pool, population, family) require
   * independent chronology. Do NOT recalculate/fabricate those fields. */
  next.gross_taxes+=result.gross_tax;
  next.payroll+=result.payroll;
  next.food_consumed+=result.food_consumed;
  next.troops_lost=(uint16_t)(next.troops_lost+result.troops_lost);
  next.starved_country_count+=(uint16_t)!!result.starved;
 }
 next.count=prepared->country_count;
 next.source_bounded_only=1u;
 next.generated_setup=next.generated_rng=next.orders_granted=0u;
 *out=next;
 return GK214_OK;
}
