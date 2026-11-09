#ifndef GK119_ORDER_BRIDGE_H
#define GK119_ORDER_BRIDGE_H
#include "gk116_enhanced_assign.h"
/* 118 allowed separate original Pass + Enhanced three orders in the same
 * attached stock source, an unsound six-order display/ledger. 119 always
 * operates on exactly one effective budget and leaves source immutable. */
typedef enum {GK119_OK=0,GK119_SOURCE=1,GK119_PROFILE=2,
              GK119_NO_ORDERS=3} Gk119Result;
Gk119Result gk119_effective_remaining(const Gk79Checkpoint *stock,
 const Gk116EnhancedAssign *enhanced,uint8_t rules_profile,uint8_t *remaining);
/* Native X/Y routing: an active Enhanced source-attested session must use
 * GA16 on any original-stock screen, not stock-only GK80 serialization. */
int gk119_receipt_screen(int screen,const Gk116EnhancedAssign *enhanced);
Gk119Result gk119_pass(Gk79Checkpoint *stock,
 Gk116EnhancedAssign *enhanced,uint8_t rules_profile);
#endif
