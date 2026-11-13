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
  case 0u:return GK134_ROUTE_CLASSIC_TAX;
  case 1u:return GK134_ROUTE_ENHANCED_ASSIGN;
  case 4u:return GK134_ROUTE_TRADE_REFERENCE;
  case 6u:return GK134_ROUTE_VIEW;
  case 12u:return GK134_ROUTE_PASS;
  case 13u:return GK134_ROUTE_PORT_STORAGE;
  default:return GK134_ROUTE_LOCKED_STOCK_ACTION;
 }
}

const char *gk134_menu_capability(unsigned ordinal,uint8_t enhanced,
 uint8_t orders_remaining,uint8_t actions,uint8_t passed){
 if(ordinal>=14u||enhanced>1u||orders_remaining>3u||actions>3u||passed>1u)
  return "REFUSED";
 switch(ordinal){
 case 0u: /* TAX: previously mislabeled LOCKED even when playable. */
  if(enhanced)return "CLASSIC ONLY";
  if(passed)return "ENDED";
  if(orders_remaining)return "SET RATE";
  return actions?"UNDO ONLY":"ENDED";
 case 1u: /* CLASSIC ASSIGN: retain access to PORT-ONLY Y undo. */
  if(enhanced)return "ENHANCED";
  if(passed)return "ENDED";
  if(orders_remaining)return "MANUAL";
  return actions?"UNDO ONLY":"ENDED";
 case 4u:return "PRICE VIEW"; /* Not actual merchant availability. */
 case 6u:return "READ ONLY";
 case 12u:
  if(enhanced)return "PROFILE PASS";
  return passed||!orders_remaining?"ENDED":"PASS";
 case 13u:return "PORT SAVE";
 default:return "LOCKED";
 }
}
