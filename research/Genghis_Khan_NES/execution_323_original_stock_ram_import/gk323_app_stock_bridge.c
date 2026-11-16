#include "gk323_app_stock_bridge.h"
Gk323Status gk323_app_observe(const Gk1App *app,const char *rom_sha,
 Gk323Condition condition,Gk323Bank bank,unsigned year,unsigned season,
 unsigned source_orders,const uint8_t *records,size_t length,
 Gk323Spring *out){
 if(!app||!out||!records||!rom_sha)return GK323_ARGUMENT;
 if(app->rules_profile!=0||app->active_world>1)return GK323_UNSUPPORTED;
 const unsigned w=app->active_world;
 const unsigned count=w?GK1_COUNTRIES_WORLD:GK1_COUNTRIES_MONGOL;
 const Gk1LoadedScenario *active=&app->worlds[w];
 if(!app->world_ready[w]||active->phase!=GK1_PRE_SETUP_TEMPLATE||
    active->scenario_id!=(w?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||
    active->country_count!=count)return GK323_UNSUPPORTED;
 for(unsigned i=0;i<count;i++){
  const Gk1CountryTemplate *c=&active->countries[i];
  if(c->scenario_id!=(uint8_t)(w+1)||c->country_id!=i+1||c->index!=i)
   return GK323_UNSUPPORTED;
 }
 return gk323_decode(rom_sha,w?GK323_WORLD:GK323_MONGOL,
                     condition,bank,year,season,source_orders,
                     records,length,out);
}
