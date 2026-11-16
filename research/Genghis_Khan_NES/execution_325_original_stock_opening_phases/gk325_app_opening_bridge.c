#include "gk325_app_opening_bridge.h"
Gk325Status gk325_app_audit(const Gk1App *app,const Gk325Trace *trace,Gk325Audit *out){
 if(!app||!trace||!out)return GK325_BAD_INPUT;
 if(app->rules_profile!=0||app->active_world>1||!app->world_ready[app->active_world])
  return GK325_WRONG_SOURCE;
 unsigned w=app->active_world;
 const Gk1LoadedScenario *world=&app->worlds[w];
 if(world->phase!=GK1_PRE_SETUP_TEMPLATE||
    world->scenario_id!=(w?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||
    world->country_count!=(w?27:14)||
    trace->world!=(w?GK325_WORLD:GK325_MONGOL)||
    trace->country_count!=world->country_count)return GK325_WRONG_SOURCE;
 for(unsigned i=0;i<world->country_count;i++){
  const Gk1CountryTemplate *c=&world->countries[i];
  if(c->country_id!=i+1||c->index!=i||c->scenario_id!=w+1)return GK325_WRONG_SOURCE;
 }
 return gk325_audit_trace(trace,out);
}
