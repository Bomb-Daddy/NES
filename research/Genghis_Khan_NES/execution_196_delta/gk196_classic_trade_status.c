#include "gk196_classic_trade_status.h"
#include "gk195_classic_view.h"
Gk196TradeStatusResult gk196_classic_trade_status(const Gk1App *app,
 Gk196TradeStatus *out){
 if(!app||!out)return GK196_INVALID;
 if(app->rules_profile!=0u)return GK196_SOURCE;
 const Gk79Checkpoint *cp=&app->stock_checkpoint;
 Gk195CountryView view={0};
 if(cp->player_index>=cp->world.country_count)return GK196_SOURCE;
 if(gk195_classic_country_view(app,cp->player_index,&view)!=GK195_OK)
  return GK196_SOURCE;
 /* Derive ONLY from replay-verified Classic budget, never the immutable
  * original source budget once a Classic action has been taken. */
 Gk196TradeStatus next={0};
 next.remaining=view.remaining;
 next.actions=view.actions;
 next.passed=view.passed;
 next.derived=view.derived;
 next.source_profile=cp->profile;
 next.year=cp->year;
 next.rate=cp->rate;
 *out=next;
 return GK196_OK;
}
