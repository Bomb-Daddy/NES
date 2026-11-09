#include "gk115_market_gate.h"
/* This gate deliberately neither accepts a boolean claimed by the market
 * caller nor equates verified original country records with verified trade.
 * A future non-synthetic merchant event proof is required to advance the
 * final result to SOURCE_VERIFIED. Never fabricate the presence/offer RNG. */
Gk115MarketGate gk115_stock_market_gate(const Gk79Checkpoint *stock,
 const Gk1LoadedScenario *world,const Gk61Market *market,
 uint8_t w,uint8_t country){
 if(!stock||!world||!market||w>1||world->scenario_id!=w+1u||
    country==0||country>world->country_count||
    world->country_count!=(w==0?14u:27u))return GK115_MARKET_BAD_INPUT;
 if(world->phase==GK1_PRE_SETUP_TEMPLATE)return GK115_MARKET_WORLD_PRE_SETUP;
 if(world->phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE||
    !stock->valid||stock->source_verified!=1u||
    !gk85_raw_source_intact(stock)||!gk88_stock_projection_intact(stock))
   return GK115_MARKET_STOCK_SOURCE_MISSING;
 if(world->scenario_id!=stock->world.scenario_id||
    world->country_count!=stock->world.country_count||
    stock->orders.season!=GK71_SPRING||stock->orders.country_id!=country||
    stock->year!=(w==0?1175u:1206u)||
    !market->playable||market->rate!=stock->rate||
    market->profile!=GK61_CLASSIC)
   return GK115_MARKET_SOURCE_CONFLICT;
 /* Stock captures contain zero original merchant-presence or offer-event
  * observations. A caller saying a merchant is present proves nothing.
  * This is the mandatory missing evidence gate, not an RNG simulation. */
 return GK115_MARKET_MERCHANT_EVENT_MISSING;
}
