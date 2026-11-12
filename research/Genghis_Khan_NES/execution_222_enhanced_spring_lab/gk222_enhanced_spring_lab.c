#include "gk222_enhanced_spring_lab.h"
#include <string.h>

Gk222Status gk222_open(const Gk1LoadedScenario *source,
                       uint8_t player_index,Gk222Spring *out){
 if(!source||!out||source->phase!=GK1_PRE_SETUP_TEMPLATE||
    source->scenario_id<GK1_MONGOL_CONQUEST||
    source->scenario_id>GK1_WORLD_CONQUEST ||
    source->country_count!=(source->scenario_id==GK1_MONGOL_CONQUEST?
      GK1_COUNTRIES_MONGOL:GK1_COUNTRIES_WORLD))return GK222_BAD_SOURCE;
 if(player_index>=source->country_count)return GK222_BAD_PLAYER;
 Gk214Batch batch;
 if(gk214_project_spring_batch(source,&batch)!=GK214_OK)
   return GK222_BAD_SOURCE;
 Gk222Spring tmp={0};
 tmp.active=1u;
 tmp.scenario=(uint8_t)source->scenario_id;
 tmp.reference_year=(uint16_t)(source->scenario_id==GK1_MONGOL_CONQUEST?1175u:1206u);
 tmp.player_index=tmp.selected_country=player_index;
 tmp.remaining=3u;
 tmp.working=batch.after; /* only bounded finance, not full original Spring */
 tmp.taxes=batch.gross_taxes;
 tmp.payroll=batch.payroll;
 tmp.food_used=batch.food_consumed;
 *out=tmp;return GK222_OK;
}
Gk222Status gk222_apply(Gk222Spring *in_out,
                        uint8_t selected_country,Gk222Action action){
 if(!in_out||!in_out->active||in_out->scenario<1u||
    in_out->scenario>2u||in_out->working.country_count!=(in_out->scenario==1u?14u:27u)||
    in_out->working.scenario_id!=(Gk1ScenarioId)in_out->scenario||
    in_out->working.phase!=GK1_PRE_SETUP_TEMPLATE||in_out->player_index>=in_out->working.country_count)
    return GK222_NOT_STARTED;
 if(selected_country>=in_out->working.country_count || selected_country!=in_out->player_index)
    return GK222_WRONG_COUNTRY;
 if(action!=GK222_PASS&&action!=GK222_ASSIGN_TOWN_TO_FOOD)return GK222_BAD_ACTION;
 if(in_out->remaining==0u||in_out->ended)return GK222_NO_ORDERS;
 if(action==GK222_ASSIGN_TOWN_TO_FOOD){
  Gk1CountryTemplate *country=&in_out->working.countries[selected_country];
  if(country->town==0u||country->food_workers>=999u)return GK222_NO_WORKER;
  country->town--;
  country->food_workers++;
  /* Reallocation is explicitly a port drill, NOT a proven NES Assign cost. */
 }
 in_out->remaining--;
 in_out->actions_committed++;
 in_out->ended=(uint8_t)(in_out->remaining==0u);
 return GK222_OK;
}
