#ifndef GK323_APP_STOCK_BRIDGE_H
#define GK323_APP_STOCK_BRIDGE_H
#include "gk1_app_state.h"
#include "gk323_original_spring.h"
/* Linked to actual inherited Gk1App. Accept authentic source observations
 * for the active Classic scenario ONLY and leave all game state unchanged.
 * This is a genuine live-world bridge but not a seasonal initializer. */
Gk323Status gk323_app_observe(const Gk1App *app,const char *rom_sha,
 Gk323Condition condition,Gk323Bank bank,unsigned year,unsigned season,
 unsigned source_orders,const uint8_t *records,size_t length,
 Gk323Spring *out);
#endif
