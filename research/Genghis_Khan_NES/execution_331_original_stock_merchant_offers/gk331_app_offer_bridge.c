#include "gk331_app_offer_bridge.h"
Gk331Status gk331_app_stock_trade_offers(const Gk1App *app,
 const char *rom,const uint8_t seed[6],Gk331Offers *out){
 if(!app||!rom||!seed||!out)return GK331_BAD;
 const Gk79Checkpoint *cp=&app->stock_checkpoint;
 if(!cp->source_verified||!cp->valid||
    app->active_world>=2u||app->rules_profile>1u||
    cp->world.scenario_id!=app->active_world+1u||
    cp->orders.playable!=1u||cp->orders.season!=GK71_SPRING||
    !gk85_raw_source_intact(cp)||!gk88_stock_projection_intact(cp))
  return GK331_STAGE;
 /* Original offer MASK generator is safe as an immutable source candidate.
  * Merchant presence / RNG invocation in the active historical game cannot
  * be inferred from a raw country checkpoint or supplied seed alone. */
 return gk331_offer_masks(rom,seed,out);
}
