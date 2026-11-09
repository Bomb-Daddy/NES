#include "gk112_stock_setup_deltas.h"
#include <string.h>
static const char *const field_names[GK112_METRIC_COUNT]={
 "Arms","Skill","Troops","Economy","Town","Defense","Masons",
 "Artisans","FoodWorkers","Gold","Morale","Tax","Undrafted",
 "Fur","Silk","Jewels","Metals","Medicines","Woodworks","Ceramics",
 "Glass","Textiles","Perfume","Food"
};
const char *gk112_field_name(unsigned field){
 return field<GK112_METRIC_COUNT?field_names[field]:NULL;
}
static uint16_t metric(const Gk1CountryTemplate *c,unsigned i){
 switch(i){
  case GK112_ARMS:return c->arms;
  case GK112_SKILL:return c->skill;
  case GK112_TROOPS:return c->troops;
  case GK112_ECONOMY:return c->economy;
  case GK112_TOWN:return c->town;
  case GK112_DEFENSE:return c->defense;
  case GK112_MASONS:return c->masons;
  case GK112_ARTISANS:return c->artisans;
  case GK112_FOOD_WORKERS:return c->food_workers;
  case GK112_GOLD:return c->gold;
  case GK112_MORALE:return c->morale;
  case GK112_TAX:return c->tax;
  case GK112_UNDRAFTED:return c->undrafted_reserve;
  default:return c->goods[i-GK112_FUR];
 }
}
Gk112Status gk112_compare_setup(const char *csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *stock,
 Gk112StockDeltas *out){
 if(!csv||!pre||!stock||!out)return GK112_ARGUMENT;
 /* Inherit the strict authentic original-record SHA/profile, unmodified
  * canonical 41-country CSV, all-country original identity/projection gates. */
 Gk111StockSpringAudit checked;
 Gk111Result r=gk111_compare_stock_spring_world(csv,pre,stock,&checked);
 if(r==GK111_SOURCE_REFUSED||r==GK111_TEMPLATE_DRIFT||r==GK111_FINANCE_REFUSED)
  return GK112_TEMPLATE_REFUSED;
 if(r!=GK111_OK)return GK112_STOCK_REFUSED;
 if(checked.country_count!=pre->country_count||
    checked.setup_resolved||checked.first_spring_ready||checked.orders_granted)
  return GK112_STOCK_REFUSED;
 Gk112StockDeltas next={0};next.profile=stock->profile;
 next.scenario_id=(uint8_t)pre->scenario_id;
 next.country_count=pre->country_count;
 for(unsigned c=0;c<next.country_count;c++){
  for(unsigned f=0;f<GK112_METRIC_COUNT;f++){
   int32_t d=(int32_t)metric(&stock->world.countries[c],f)-
             (int32_t)metric(&pre->countries[c],f);
   next.delta[c][f]=d;
   if(!d)continue;
   next.fields_changed_in_country[c]++;
   next.countries_changed[f]++;
   next.countries_increased[f]+=(uint8_t)(d>0);
   next.countries_decreased[f]+=(uint8_t)(d<0);
   next.signed_total[f]+=d;
   next.absolute_total[f]+=(uint32_t)(d<0?-d:d);
  }
 }
 *out=next;return GK112_OK;
}
Gk112Status gk112_compare_mongol_histories(const char *csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *fresh,
 const Gk79Checkpoint *archived,Gk112PairedHistories *out){
 if(!csv||!pre||!fresh||!archived||!out)return GK112_ARGUMENT;
 if(fresh->profile!=GK77_FRESH_MONGOL_WORKRAM||
    archived->profile!=GK77_ARCHIVED_MONGOL_SAVERAM||
    pre->scenario_id!=GK1_MONGOL_CONQUEST||
    fresh->year!=1175u||archived->year!=1175u||
    fresh->orders.season!=GK71_SPRING||archived->orders.season!=GK71_SPRING)
  return GK112_PROFILE_REFUSED;
 Gk112StockDeltas a,b;
 Gk112Status s=gk112_compare_setup(csv,pre,fresh,&a);
 if(s!=GK112_OK)return s;
 s=gk112_compare_setup(csv,pre,archived,&b);
 if(s!=GK112_OK)return s;
 if(a.country_count!=GK1_COUNTRIES_MONGOL||b.country_count!=GK1_COUNTRIES_MONGOL)
  return GK112_PROFILE_REFUSED;
 Gk112PairedHistories next={0};next.country_count=GK1_COUNTRIES_MONGOL;
 for(unsigned c=0;c<GK1_COUNTRIES_MONGOL;c++){
  for(unsigned f=0;f<GK112_METRIC_COUNT;f++){
   int32_t d=a.delta[c][f]-b.delta[c][f];
   next.fresh_minus_archived[c][f]=d;
   if(!d)continue;
   next.fields_changed_in_country[c]++;
   next.countries_changed[f]++;
   next.absolute_total[f]+=(uint32_t)(d<0?-d:d);
   if(!next.first_difference_country)next.first_difference_country=(uint8_t)(c+1u);
  }
 }
 *out=next;return GK112_OK;
}
