#ifndef GK220_LABOR_HISTORY_H
#define GK220_LABOR_HISTORY_H
#include "gk219_finance_compare.h"
#include <stdint.h>
/* PORT-only reversible PRE_SETUP workforce drafting. NOT an original NES
 * Assign, first Spring, RNG draw or a turn/order mutation. */
#define GK220_HISTORY_CAP 64u
typedef enum {
 GK220_OK=0,GK220_INVALID=1,GK220_SOURCE_DRIFT=2,
 GK220_NO_HISTORY=3,GK220_WRONG_COUNTRY=4,GK220_FULL=5
} Gk220Result;
typedef struct {
 uint8_t world,country,from,to;
 uint16_t from_before,to_before;
} Gk220Move;
typedef struct {
 uint8_t count;
 Gk220Move moves[GK220_HISTORY_CAP];
} Gk220History;
Gk220Result gk220_labor_transfer(const Gk1LoadedScenario *source,
 Gk1LoadedScenario *draft, Gk220History *history,
 uint8_t world,uint8_t country,uint8_t from,uint8_t to);
Gk220Result gk220_labor_undo(const Gk1LoadedScenario *source,
 Gk1LoadedScenario *draft,Gk220History *history,
 uint8_t world,uint8_t country);
#endif
