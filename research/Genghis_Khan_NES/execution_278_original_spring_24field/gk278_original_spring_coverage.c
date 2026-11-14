#include "gk278_original_spring_coverage.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static const char *const field_names[GK278_FIELD_COUNT]={
 "Arms","Skill","Troops","Economy","Town","Defense","Masons",
 "Artisans","FoodWorkers","Gold","Morale","Tax","Undrafted",
 "Fur","Silk","Jewels","Metals","Medicines","Woodworks",
 "Ceramics","Glass","Textiles","Perfume","Food"
};
const char *gk278_field_name(Gk278Field field){
 return (unsigned)field<GK278_FIELD_COUNT?field_names[field]:NULL;
}
static uint16_t direct_value(const Gk1CountryTemplate *c,unsigned ix){
 switch(ix){
 case GK278_ARMS:return c->arms;
 case GK278_SKILL:return c->skill;
 case GK278_TROOPS:return c->troops;
 case GK278_ECONOMY:return c->economy;
 case GK278_TOWN:return c->town;
 case GK278_DEFENSE:return c->defense;
 case GK278_MASONS:return c->masons;
 case GK278_ARTISANS:return c->artisans;
 case GK278_FOOD_WORKERS:return c->food_workers;
 case GK278_GOLD:return c->gold;
 case GK278_MORALE:return c->morale;
 case GK278_TAX:return c->tax;
 case GK278_UNDRAFTED:return c->undrafted_reserve;
 default:return c->goods[ix-GK278_FUR];
 }
}
Gk278Status gk278_compare_original_spring_coverage(const char *csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *original,Gk278Coverage *out){
 if(!csv||!*csv||!pre||!original||!out)return GK278_ARGUMENT;
 /* Inherit the complete existing canonical-template, original 83-byte raw
  * source SHA, RAM-bank profile, season/year/3-order and source-projection
  * validation. Never weaken these guards to obtain a better-looking count. */
 Gk215Gap *gate=(Gk215Gap*)malloc(sizeof *gate);
 if(!gate)return GK278_ARGUMENT;
 Gk215Status check=gk215_compare_original_spring(csv,pre,original,gate);
 free(gate);
 if(check!=GK215_OK)return check==GK215_TEMPLATE_REFUSED?GK278_TEMPLATE:
  check==GK215_SOURCE_REFUSED?GK278_SOURCE:
  check==GK215_FINANCE_REFUSED?GK278_PROJECTION:GK278_ARGUMENT;
 Gk214Batch *baseline=(Gk214Batch*)malloc(sizeof *baseline);
 if(!baseline)return GK278_ARGUMENT;
 if(gk214_project_spring_batch(pre,baseline)!=GK214_OK){
  free(baseline);return GK278_PROJECTION;
 }
 Gk278Coverage next={0};
 next.profile=original->profile;next.scenario=(uint8_t)pre->scenario_id;
 next.country_count=pre->country_count;next.source_observation_only=1u;
 for(unsigned f=0;f<GK278_FIELD_COUNT;f++){
  next.field[f].minimum_residual=INT32_MAX;
  next.field[f].maximum_residual=INT32_MIN;
  for(unsigned i=0;i<pre->country_count;i++){
   const Gk1CountryTemplate *actual=&original->world.countries[i];
   const Gk1CountryTemplate *projected=&baseline->after.countries[i];
   int32_t delta=(int32_t)direct_value(actual,f)-direct_value(projected,f);
   Gk278FieldGap *g=&next.field[f];
   if(delta<g->minimum_residual)g->minimum_residual=delta;
   if(delta>g->maximum_residual)g->maximum_residual=delta;
   g->signed_residual+=delta;
   if(delta){g->unequal_countries++;next.unequal_cells++;
     next.unequal_fields_per_country[i]++;}
  }
 }
 next.compared_cells=(uint16_t)(pre->country_count*GK278_FIELD_COUNT);
 next.exact_cells=(uint16_t)(next.compared_cells-next.unequal_cells);
 for(unsigned i=0;i<pre->country_count;i++)
  if(next.unequal_fields_per_country[i])next.countries_with_differences++;
 /* Source credentials do not authorize independently generated values. */
 free(baseline);*out=next;return GK278_OK;
}
