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
 GK134_ROUTE_PASS=5,
 GK134_ROUTE_PORT_STORAGE=6, /* port checkpoint management, NOT NES Other parity */
 GK134_ROUTE_CLASSIC_TAX=7 /* manual-derived TAX; original ability costs UNKNOWN */
} Gk134CommandRoute;
const char *gk134_command_name(unsigned ordinal);
Gk134CommandRoute gk134_command_route(unsigned ordinal);
/* EXEC0253: truthful controller-menu availability for an independently
 * source-authenticated Spring.  This is only a UI classification; it never
 * authorizes a command, applies a rule, or advances an original NES season.
 * The caller must first pass its raw-record and mutable-world integrity gates.
 * An unpassed third action may still be undone from TAX/ASSIGN's Y command.
 */
const char *gk134_menu_capability(unsigned ordinal,uint8_t enhanced,
 uint8_t orders_remaining,uint8_t actions,uint8_t passed);

#endif
