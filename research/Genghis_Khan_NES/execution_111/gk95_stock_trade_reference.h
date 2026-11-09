#ifndef GK95_STOCK_TRADE_REFERENCE_H
#define GK95_STOCK_TRADE_REFERENCE_H
#include "gk79_stock_checkpoint.h"
#include "gk61_market.h"
/* An authenticated-Spring, read-only price reference, NOT a Trade action.
 * Only full owner-original record binding can make the rate authoritative.
 * Presence, offered items, merchant RNG, actor abilities and season orders
 * remain unknown and are deliberately NOT synthesized. */
typedef enum {
 GK95_QUOTE_OK=0, GK95_QUOTE_INVALID=1, GK95_QUOTE_UNVERIFIED=2
} Gk95QuoteResult;
Gk95QuoteResult gk95_stock_trade_reference(const Gk79Checkpoint *source,
                     Gk61Merchant merchant, uint8_t item,
                     Gk61Quote *out);
#endif
