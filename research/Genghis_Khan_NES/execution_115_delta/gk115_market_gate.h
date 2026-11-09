#ifndef GK115_MARKET_GATE_H
#define GK115_MARKET_GATE_H
#include "gk79_stock_checkpoint.h"
#include "gk61_market.h"
/* Structural context and SOURCE AUTHORITY are separate. An exact original
 * first-Spring RAM record proves the countries/Rate, not merchant presence,
 * offers, or a current executable merchant action. No authentic offer event
 * has been recovered yet. Therefore SOURCE_VERIFIED is reserved/unreachable
 * until the original NES merchant scheduler/event capture is implemented. */
typedef enum {
 GK115_MARKET_BAD_INPUT=0,
 GK115_MARKET_WORLD_PRE_SETUP=1,
 GK115_MARKET_STOCK_SOURCE_MISSING=2,
 GK115_MARKET_SOURCE_CONFLICT=3,
 GK115_MARKET_MERCHANT_EVENT_MISSING=4,
 GK115_MARKET_SOURCE_VERIFIED=5
} Gk115MarketGate;
Gk115MarketGate gk115_stock_market_gate(const Gk79Checkpoint *stock,
 const Gk1LoadedScenario *app_world,const Gk61Market *market,
 uint8_t app_world_index,uint8_t country_one_based);
#endif
