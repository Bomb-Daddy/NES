#ifndef GK134_STOCK_COMMAND_ROUTE_H
#define GK134_STOCK_COMMAND_ROUTE_H
#include <stdint.h>
/* Source manual command number -> controller action. No market or game-state
 * authorization is implied by this routing table. Classic source actions
 * stay locked until their original ROM rules are independently established. */
typedef enum {
 GK134_ROUTE_INVALID=0,
 GK134_ROUTE_LOCKED_STOCK_ACTION=1,
 GK134_ROUTE_ENHANCED_ASSIGN=2,
 GK134_ROUTE_VIEW=3,
 GK134_ROUTE_TRADE_REFERENCE=4,
 GK134_ROUTE_PASS=5
} Gk134CommandRoute;
const char *gk134_command_name(unsigned ordinal);
Gk134CommandRoute gk134_command_route(unsigned ordinal);
#endif
