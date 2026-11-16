#include "gk329_app_bridge.h"
Gk329Status gk329_app_archived_rng_next(const Gk1App *app,
 const uint8_t *private_original_rom,size_t rom_size,
 const uint8_t *private_fields,size_t fields_size,Gk329Receipt *out){
 if(!app||!private_original_rom||!private_fields||!out)return GK329_ARGUMENT;
 if(app->rules_profile!=0u||app->active_world>1u||
    !app->world_ready[app->active_world]||
    !app->stock_checkpoint.valid||!app->stock_checkpoint.source_verified||
    app->stock_checkpoint.profile!=(app->active_world?
      GK77_ARCHIVED_WORLD_SAVERAM:GK77_ARCHIVED_MONGOL_SAVERAM)||
    app->stock_checkpoint.world.country_count!=(app->active_world?27u:14u)||
    app->stock_checkpoint.world.scenario_id!=(Gk1ScenarioId)(app->active_world+1u)||
    !gk85_raw_source_intact(&app->stock_checkpoint)||
    !gk88_stock_projection_intact(&app->stock_checkpoint))
   return GK329_NOT_ARCHIVED_SOURCE;
 return gk329_replay_archived_next_draw(app->active_world,
   private_original_rom,rom_size,private_fields,fields_size,out);
}
