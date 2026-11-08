#include "gk95_stock_trade_reference.h"
Gk95QuoteResult gk95_stock_trade_reference(const Gk79Checkpoint *s,
                     Gk61Merchant merchant, uint8_t item, Gk61Quote *out){
 Gk61Quote proposed;
 if(!s||!out||merchant>=GK61_MERCHANT_COUNT||item>=GK61_ITEM_COUNT)
  return GK95_QUOTE_INVALID;
 /* Reject even internally consistent synthetic fixtures at the production
  * API: only one of the three SHA-pinned original-stock captures qualifies. */
 if(!s->valid||!s->source_verified||!gk85_raw_source_intact(s)||
    !gk88_stock_projection_intact(s))return GK95_QUOTE_UNVERIFIED;
 if(gk61_quote(s->rate,merchant,item,&proposed)!=GK61_OK)
  return GK95_QUOTE_INVALID;
 *out=proposed;
 return GK95_QUOTE_OK;
}
