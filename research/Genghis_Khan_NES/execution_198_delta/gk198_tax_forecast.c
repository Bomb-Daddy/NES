#include "gk198_tax_forecast.h"
#include <string.h>
Gk198Result gk198_compare_fields(const Gk75SpringFields *fields,
 uint8_t proposed_rate,Gk198Forecast *out){
 if(!fields||!out||proposed_rate<1u||proposed_rate>100u||
    fields->tax<1u||fields->tax>100u)return GK198_INVALID;
 Gk198Forecast next={0};
 Gk75SpringFields changed=*fields;changed.tax=proposed_rate;
 if(gk75_spring_food_tax_pay(fields,&next.committed)!=GK75_OK||
    gk75_spring_food_tax_pay(&changed,&next.proposed)!=GK75_OK)
  return GK198_ARITHMETIC;
 next.committed_rate=(uint8_t)fields->tax;
 next.proposed_rate=proposed_rate;
 next.before_gold=fields->gold;next.before_food=fields->food;
 next.gold_delta=(int32_t)next.proposed.after.gold-
                 (int32_t)next.committed.after.gold;
 *out=next;return GK198_OK;
}
Gk198Result gk198_classic_tax_forecast(const Gk187ClassicAssign *classic,
 const Gk79Checkpoint *stock,uint8_t proposed_rate,Gk198Forecast *out){
 if(!classic||!stock||!out||proposed_rate<1u||proposed_rate>100u)
  return GK198_INVALID;
 if(!gk187_intact(classic,stock)||!classic->active||
    classic->source_profile!=stock->profile||
    classic->player_index>=classic->mutable_world.country_count||
    stock->orders.remaining!=3u)return GK198_SOURCE;
 const Gk1CountryTemplate *country=
  &classic->mutable_world.countries[classic->player_index];
 Gk75SpringFields f={country->troops,country->town,country->masons,
  country->artisans,country->food_workers,country->goods[10],country->gold,
  country->economy,country->morale,country->tax};
 Gk198Forecast next;
 Gk198Result r=gk198_compare_fields(&f,proposed_rate,&next);
 if(r!=GK198_OK)return r;
 next.action_count=classic->action_count;
 next.remaining=classic->budget.remaining;
 *out=next;
 return GK198_OK;
}
