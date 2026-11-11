#include "gk195_classic_view.h"
#include <string.h>
Gk195ViewResult gk195_classic_country_view(const Gk1App *app,
 uint8_t country_index,Gk195CountryView *out){
 if(!app||!out)return GK195_INVALID;
 const Gk79Checkpoint *cp=&app->stock_checkpoint;
 if(app->rules_profile!=0u||cp->valid!=1u||cp->source_verified!=1u||
    !gk85_raw_source_intact(cp)||!gk88_stock_projection_intact(cp))
  return GK195_SOURCE;
 if(country_index>=cp->world.country_count||
    cp->player_index>=cp->world.country_count)return GK195_INVALID;
 if(app->classic_assign.active &&
    !gk187_intact(&app->classic_assign,cp))return GK195_SOURCE;
 Gk195CountryView v={0};
 v.country_index=country_index;
 v.is_player=country_index==cp->player_index;
 v.original=cp->world.countries[country_index];
 v.current=v.original;
 v.remaining=cp->orders.remaining;
 if(app->classic_assign.active){
  v.actions=app->classic_assign.action_count;
  v.remaining=app->classic_assign.budget.remaining;
  v.passed=app->classic_assign.passed;
  if(v.is_player){
   v.current=app->classic_assign.mutable_world.countries[country_index];
   v.derived=1u;
  }
 }
 /* No pointers escape: this is a verified copy for the UI only. */
 *out=v;
 return GK195_OK;
}
