#include "gk110_spring_stage.h"
#include "gk1_source_identity.h"
#include <string.h>
Gk110Result gk110_prepare_source_finance(const char *verified_csv,
 const Gk1LoadedScenario *template_world,Gk110SpringStage *out){
 if(!verified_csv||!template_world||!out)return GK110_ARGUMENT;
 /* Scenario equality includes ALL records, source address labels, names,
  * specialty and historical metadata—not only displayed finance columns.
  * No partial mutated country view can be upgraded to a seasonal state. */
 if(!gk1_verified_original_csv(verified_csv))return GK110_SOURCE_REFUSED;
 if(template_world->scenario_id!=GK1_MONGOL_CONQUEST&&
    template_world->scenario_id!=GK1_WORLD_CONQUEST)return GK110_BASELINE_DRIFT;
 Gk1TemplateDataset original;
 if(gk1_template_load(verified_csv,&original)!=GK1_LOAD_OK)
  return GK110_SOURCE_REFUSED;
 Gk1LoadedScenario canonical;
 if(gk1_open_loaded_scenario(&original,template_world->scenario_id,
      &canonical)!=GK1_LOAD_OK ||
    memcmp(&canonical,template_world,sizeof canonical)!=0 ||
    template_world->phase!=GK1_PRE_SETUP_TEMPLATE ||
    template_world->country_count!=(template_world->scenario_id==GK1_MONGOL_CONQUEST?14u:27u))
  return GK110_BASELINE_DRIFT;
 Gk110SpringStage staged={0};
 staged.scenario_id=(uint8_t)template_world->scenario_id;
 staged.country_count=template_world->country_count;
 for(unsigned i=0;i<staged.country_count;i++){
  const Gk1CountryTemplate *c=&canonical.countries[i];
  Gk75SpringResult *v=&staged.country[i];
  if(gk75_template_projection(c,v)!=GK75_OK)return GK110_NUMERIC_REFUSED;
  staged.food_before+=c->goods[10];staged.food_after+=v->after.food;
  staged.gold_before+=c->gold;staged.gold_after+=v->after.gold;
  staged.troops_before+=c->troops;staged.troops_after+=v->after.troops;
  staged.tax_total+=v->gross_tax;staged.food_consumed+=v->food_consumed;
  staged.payroll_total+=v->payroll;
 }
 /* Unresolved setup, age/growth/sickness, difficulty, random events and
  * AI cannot be silently treated as finished. All three gates remain zero. */
 *out=staged;
 return GK110_OK;
}
