#ifndef GK331_APP_OFFER_BRIDGE_H
#define GK331_APP_OFFER_BRIDGE_H
#include "gk331_stock_offer_rng.h"
#include "gk1_app_state.h"
/* Inherited *actual Gk1App* checkpoint-guarded candidate generation.
 * This does NOT mutate its game world, orders, merchant presence or phase. */
Gk331Status gk331_app_stock_trade_offers(const Gk1App *app,
 const char *private_stock_rom,const uint8_t explicit_seed[6],Gk331Offers *out);
#endif
