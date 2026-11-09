#include "gk119_order_bridge.h"
#include "gk1_app_state.h"
/* No synthetic phase, no source-independent merchant action. These APIs
 * work only after original stock SHA attestation succeeded. */
Gk119Result gk119_effective_remaining(const Gk79Checkpoint *stock,
 const Gk116EnhancedAssign *enhanced,uint8_t profile,uint8_t *remaining){
 if(!remaining||!stock||!enhanced||profile>1u||!stock->valid||
    !gk85_raw_source_intact(stock))return GK119_SOURCE;
 if(enhanced->active){
  /* Only the Enhanced source-bound pathway requires original stock SHA.
   * Preserve old synthetic isolated base Pass research tests; production
   * native source loader does not permit entering this screen from setup. */
  if(!stock->source_verified)return GK119_SOURCE;
  if(profile!=1u)return GK119_PROFILE;
  if(!gk116_attestation_intact(enhanced,stock))return GK119_SOURCE;
  *remaining=enhanced->budget.remaining;return GK119_OK;
 }
 if(stock->orders.playable!=1u||stock->orders.remaining>3u||
    stock->orders.ended!=(stock->orders.remaining==0u))return GK119_SOURCE;
 *remaining=stock->orders.remaining;return GK119_OK;
}
Gk119Result gk119_pass(Gk79Checkpoint *stock,
 Gk116EnhancedAssign *enhanced,uint8_t profile){
 uint8_t remaining=0u;
 Gk119Result r=gk119_effective_remaining(stock,enhanced,profile,&remaining);
 if(r!=GK119_OK)return r;
 if(remaining==0u)return GK119_NO_ORDERS;
 if(enhanced->active){
  Gk116Result b=gk116_pass_remaining(enhanced,stock);
  if(b==GK116_NO_ORDERS)return GK119_NO_ORDERS;
  return b==GK116_OK?GK119_OK:GK119_SOURCE;
 }
 Gk79Result b=gk79_pass(stock);
 if(b==GK79_EXHAUSTED)return GK119_NO_ORDERS;
 return b==GK79_OK?GK119_OK:GK119_SOURCE;
}

int gk119_receipt_screen(int screen,const Gk116EnhancedAssign *enhanced){
 if(screen==GK1_SCREEN_ENHANCED_ASSIGN)return 1;
 if(!enhanced||enhanced->active!=1u)return 0;
 return screen==GK1_SCREEN_STOCK_CONTINUE ||
        screen==GK1_SCREEN_STOCK_COMMANDS ||
        screen==GK1_SCREEN_STOCK_TRADE_REFERENCE;
}
