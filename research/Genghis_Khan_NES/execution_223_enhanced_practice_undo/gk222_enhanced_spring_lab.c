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
 tmp.initial_town=tmp.working.countries[player_index].town;
 tmp.initial_food_workers=tmp.working.countries[player_index].food_workers;
 tmp.taxes=batch.gross_taxes;
 tmp.payroll=batch.payroll;
 tmp.food_used=batch.food_consumed;
 *out=tmp;return GK222_OK;
}
/* Three receipts, one player and a two-pool conserved transformation.
 * Validation detects forged counters, edited source identity and resource tamper
 * before either commit or undo. No state writes on failure. */
Gk222Status gk222_verify_receipts(const Gk222Spring *state){
 if(!state||!state->active||state->scenario<1u||state->scenario>2u||
    state->working.country_count!=(state->scenario==1u?14u:27u)||
    state->working.scenario_id!=(Gk1ScenarioId)state->scenario||
    state->working.phase!=GK1_PRE_SETUP_TEMPLATE||
    state->player_index>=state->working.country_count||
    state->actions_committed>3u||state->remaining>3u||
    state->remaining+state->actions_committed!=3u||
    !!state->ended!=(state->remaining==0u))return GK222_CORRUPT_HISTORY;
 unsigned assignments=0;
 for(unsigned i=0;i<state->actions_committed;i++){
   if(state->order_receipts[i]>GK222_ASSIGN_TOWN_TO_FOOD)return GK222_CORRUPT_HISTORY;
   assignments+=state->order_receipts[i]==GK222_ASSIGN_TOWN_TO_FOOD;
 }
 for(unsigned i=state->actions_committed;i<3u;i++)
  if(state->order_receipts[i]!=0u)return GK222_CORRUPT_HISTORY;
 const Gk1CountryTemplate *country=&state->working.countries[state->player_index];
 if(state->initial_town<assignments||
    (unsigned)state->initial_food_workers+assignments>999u||
    country->town!=(unsigned)state->initial_town-assignments||
    country->food_workers!=(unsigned)state->initial_food_workers+assignments||
    country->country_id!=state->player_index+1u)return GK222_CORRUPT_HISTORY;
 return GK222_OK;
}
Gk222Status gk222_undo_last(Gk222Spring *state){
 Gk222Status r=gk222_verify_receipts(state);
 if(r!=GK222_OK)return r;
 if(!state->actions_committed)return GK222_NO_HISTORY;
 uint8_t action=state->order_receipts[state->actions_committed-1u];
 if(action==GK222_ASSIGN_TOWN_TO_FOOD){
   Gk1CountryTemplate *country=&state->working.countries[state->player_index];
   if(!country->food_workers||country->town>=999u)return GK222_CORRUPT_HISTORY;
   country->food_workers--;country->town++;
 }
 state->order_receipts[--state->actions_committed]=0u;
 state->remaining++;state->ended=0u;
 return GK222_OK;
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
 if(gk222_verify_receipts(in_out)!=GK222_OK)return GK222_CORRUPT_HISTORY;
 if(in_out->remaining==0u||in_out->ended)return GK222_NO_ORDERS;
 if(action==GK222_ASSIGN_TOWN_TO_FOOD){
  Gk1CountryTemplate *country=&in_out->working.countries[selected_country];
  if(country->town==0u||country->food_workers>=999u)return GK222_NO_WORKER;
  country->town--;
  country->food_workers++;
  /* Reallocation is explicitly a port drill, NOT a proven NES Assign cost. */
 }
 in_out->order_receipts[in_out->actions_committed]=(uint8_t)action;
 in_out->remaining--;
 in_out->actions_committed++;
 in_out->ended=(uint8_t)(in_out->remaining==0u);
 return GK222_OK;
}
