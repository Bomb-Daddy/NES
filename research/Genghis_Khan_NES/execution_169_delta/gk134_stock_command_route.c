#include "gk134_stock_command_route.h"
#include <stddef.h>
/* Exact 0-based visible command order. These names are the sole authority
 * for native command cursor labels; action mapping must never swap TAX
 * with ASSIGN or imply original TAX is already playable. */
static const char *const names[14]={
 "TAX", "ASSIGN", "GIVE", "TRAIN", "TRADE", "PEOPLE", "VIEW",
 "MOVE", "POLICY", "TREATY", "SPY", "WAR", "PASS", "OTHER"
};
const char *gk134_command_name(unsigned ordinal){
 return ordinal<14u?names[ordinal]:NULL;
}
Gk134CommandRoute gk134_command_route(unsigned ordinal){
 if(ordinal>=14u)return GK134_ROUTE_INVALID;
 switch(ordinal){
  case 1u:return GK134_ROUTE_ENHANCED_ASSIGN;
  case 4u:return GK134_ROUTE_TRADE_REFERENCE;
  case 6u:return GK134_ROUTE_VIEW;
  case 12u:return GK134_ROUTE_PASS;
  case 13u:return GK134_ROUTE_PORT_STORAGE;
  default:return GK134_ROUTE_LOCKED_STOCK_ACTION;
 }
}
