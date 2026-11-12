#include "gk215_original_spring_gap.h"
#include "gk1_source_identity.h"
#include "../execution_049/gk1_scenario_factory.h"
#include <stdlib.h>
#include <string.h>

Gk215Status gk215_compare_original_spring(const char *csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *stock,Gk215Gap *out){
 if(!csv||!*csv||!pre||!stock||!out)return GK215_BAD_ARGUMENT;
 if(!gk1_verified_original_csv(csv))return GK215_TEMPLATE_REFUSED;
 /* No caller-supplied rewritten PRE_SETUP templates are trusted. Reload
  * the identity-checked canonical CSV, then compare all world fields. */
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1LoadedScenario *clean=malloc(sizeof *clean);
 Gk214Batch *projected=malloc(sizeof *projected);
 if(!dataset||!clean||!projected){free(dataset);free(clean);free(projected);
   return GK215_BAD_ARGUMENT;}
 Gk215Status status=GK215_TEMPLATE_REFUSED;
 if(gk1_template_load(csv,dataset)!=GK1_LOAD_OK||
    (pre->scenario_id!=GK1_MONGOL_CONQUEST&&
     pre->scenario_id!=GK1_WORLD_CONQUEST)||
    gk1_open_loaded_scenario(dataset,pre->scenario_id,clean)!=GK1_LOAD_OK||
    memcmp(clean,pre,sizeof *clean)!=0)goto done;
 if(stock->valid!=1u||stock->source_verified!=1u||
    !gk85_raw_source_intact(stock)||!gk88_stock_projection_intact(stock)||
    stock->world.scenario_id!=pre->scenario_id||
    stock->world.country_count!=pre->country_count||
    stock->world.phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE||
    stock->orders.playable!=1u||stock->orders.remaining!=3u||
    stock->orders.ended||stock->orders.season!=GK71_SPRING||
    stock->year!=(pre->scenario_id==GK1_MONGOL_CONQUEST?1175u:1206u)||
    (stock->profile==GK77_ARCHIVED_WORLD_SAVERAM)!=
       (pre->scenario_id==GK1_WORLD_CONQUEST)){
  status=GK215_SOURCE_REFUSED;goto done;
 }
 status=GK215_FINANCE_REFUSED;
 if(gk214_project_spring_batch(clean,projected)!=GK214_OK)goto done;
 Gk215Gap result={0};
 result.profile=stock->profile;result.scenario=(uint8_t)pre->scenario_id;
 result.country_count=(uint8_t)pre->country_count;
 result.source_observation_only=1u;
 for(unsigned i=0;i<pre->country_count;i++){
  const Gk1CountryTemplate *p=&projected->after.countries[i];
  const Gk1CountryTemplate *r=&stock->world.countries[i];
  Gk215CountryGap *diff=result.country+i;
  diff->gold=(int32_t)r->gold-p->gold;
  diff->food=(int32_t)r->goods[10]-p->goods[10];
  diff->troops=(int32_t)r->troops-p->troops;
  diff->town=(int32_t)r->town-p->town;
  diff->masons=(int32_t)r->masons-p->masons;
  diff->artisans=(int32_t)r->artisans-p->artisans;
  diff->food_workers=(int32_t)r->food_workers-p->food_workers;
  result.gold_residual_sum+=diff->gold;
  result.food_residual_sum+=diff->food;
  result.troop_residual_sum+=diff->troops;
  if(diff->gold)result.gold_mismatches++;
  if(diff->food)result.food_mismatches++;
  if(diff->troops)result.troop_mismatches++;
  if(diff->town||diff->masons||diff->artisans||diff->food_workers)
    result.workforce_mismatches++;
  diff->differs=(uint8_t)!!(diff->gold||diff->food||diff->troops||
   diff->town||diff->masons||diff->artisans||diff->food_workers);
  result.differing_countries+=diff->differs;
  result.zero_residual_countries+=(uint16_t)!diff->differs;
 }
 *out=result;status=GK215_OK;
done:
 free(projected);free(clean);free(dataset);
 return status;
}
