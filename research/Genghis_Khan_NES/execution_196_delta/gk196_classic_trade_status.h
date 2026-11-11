#ifndef GK196_CLASSIC_TRADE_STATUS_H
#define GK196_CLASSIC_TRADE_STATUS_H
#include "gk1_app_state.h"
/* Source-attested, read-only Classic order status while visiting the Trade
 * PRICE REFERENCE. Not a merchant encounter or executable NES Trade. */
typedef enum {GK196_OK=0,GK196_INVALID=1,GK196_SOURCE=2} Gk196TradeStatusResult;
typedef struct {
 uint8_t remaining,actions,passed,derived,source_profile;
 uint16_t year,rate;
} Gk196TradeStatus;
Gk196TradeStatusResult gk196_classic_trade_status(const Gk1App *app,
 Gk196TradeStatus *out);
#endif
