#include "gk1_app_state.h"
#include "gk116_enhanced_assign.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static Gk1LoadedScenario fixture(unsigned scenario){
 Gk1LoadedScenario w={0};
 w.scenario_id=(Gk1ScenarioId)scenario;
 w.phase=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE;
 w.country_count=(uint8_t)(scenario==1u?14u:27u);
 for(unsigned i=0;i<w.country_count;i++){
  Gk1CountryTemplate *c=&w.countries[i];
  c->scenario_id=(uint8_t)scenario;c->country_id=(uint8_t)(i+1u);c->index=(uint8_t)i;
  c->troops=61;c->town=10;c->masons=10;c->artisans=10;c->food_workers=20;
  c->drafted_population=111;c->undrafted_reserve=9;c->combined_population=120;
 }
 return w;
}
static void math_case(unsigned scenario,unsigned player){
 Gk1LoadedScenario world=fixture(scenario);
 Gk71Budget budget={0};
 assert(gk71_open(&world,(uint8_t)player,GK71_SPRING,GK71_BASE,&budget)==GK71_OK);
 assert(budget.remaining==3&&budget.completed==0);
 const Gk1LoadedScenario before=world;
 Gk71Budget b=budget;
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,0,0,1)==GK116_INPUT);
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,0,5,1)==GK116_INPUT);
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,0,4,0)==GK116_INPUT);
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,0,4,62)==GK116_NO_WORKER);
 assert(gk116_worker_math(&world,&budget,(uint8_t)(player+1),0,4,1)==GK116_INPUT);
 assert(memcmp(&before,&world,sizeof world)==0&&memcmp(&budget,&b,sizeof b)==0);
 /* This is pure ENHANCED DESIGN arithmetic, not stock Mesen parity. */
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,0,4,1)==GK116_OK);
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,1,4,1)==GK116_OK);
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,2,4,1)==GK116_OK);
 assert(budget.remaining==0&&budget.completed==3&&budget.ended);
 assert(world.countries[player].troops==60&&world.countries[player].town==9&&
        world.countries[player].masons==9&&world.countries[player].food_workers==23);
 assert(world.countries[player].drafted_population==111&&
        world.countries[player].combined_population==120);
 for(unsigned i=0;i<world.country_count;i++)if(i!=player)
   assert(memcmp(&world.countries[i],&before.countries[i],sizeof before.countries[i])==0);
 Gk1LoadedScenario end=world;b=budget;
 assert(gk116_worker_math(&world,&budget,(uint8_t)player,0,4,1)==GK116_NO_ORDERS);
 assert(memcmp(&end,&world,sizeof world)==0&&memcmp(&budget,&b,sizeof b)==0);
}
int main(void){
 math_case(1,0);math_case(2,0);math_case(2,6);math_case(2,10);math_case(2,18);
 Gk79Checkpoint stock={0};Gk116EnhancedAssign session={0};
 Gk116EnhancedAssign prior=session;
 assert(gk116_open(&stock,0,&session)==GK116_CLASSIC_LOCKED);
 assert(gk116_open(&stock,1,&session)==GK116_NEEDS_ORIGINAL);
 assert(memcmp(&prior,&session,sizeof session)==0);
 assert(gk116_transfer(&session,&stock,0,4,1)==GK116_SOURCE_CHANGED);
 assert(memcmp(&prior,&session,sizeof session)==0);
 assert(gk116_save_new("should_not_create.gk16",&stock,&session)==GK116_SOURCE_CHANGED);
 assert(gk116_cold_load("missing.gk16",&stock,1,&session)==GK116_NEEDS_ORIGINAL);
 assert(memcmp(&prior,&session,sizeof session)==0);
 Gk1App app;gk1_app_init(&app);app.screen=GK1_SCREEN_STOCK_COMMANDS;
 app.rules_profile=1;app.stock_checkpoint=stock;
 assert(!gk1_app_input(&app,GK1_INPUT_CONFIRM)); /* no valid source => no game advance */
 assert(app.screen==GK1_SCREEN_STOCK_COMMANDS);
 puts("GK116 enhanced labor pure math and strict original-source refusal PASS; no original-ROM positive case claimed");
 return 0;
}
