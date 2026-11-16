#include "gk327_app_rng_bridge.h"
/* Fail closed on wrong profile, missing campaign, uncompleted setup,
 * historical source checkpoint, and any already-started season. */
Gk327Status gk327_app_source_rng_candidate(const Gk1App *app,
 const uint8_t *private_original_rom,size_t size,
 const uint8_t seed_candidate[6],Gk327RngReceipt *out){
 if(!app||!private_original_rom||!seed_candidate||!out)return GK327_BAD_ARG;
 if(app->rules_profile!=0u||app->active_world>1u)return GK327_BAD_SOURCE;
 const unsigned w=app->active_world;
 if(!app->world_ready[w]||app->worlds[w].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->setup_stage[w]!=GK53_SETUP_AWAIT_SOURCE||
    app->setup_ability_method[w]==0u||
    app->setup_candidate[w]>=app->worlds[w].country_count||
    app->worlds[w].country_count!=(w==0u?14u:27u))
  return GK327_BAD_SOURCE;
 return gk327_execute_original_rng(private_original_rom,size,seed_candidate,out);
}
