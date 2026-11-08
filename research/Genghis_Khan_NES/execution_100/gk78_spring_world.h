#ifndef GK78_SPRING_WORLD_H
#define GK78_SPRING_WORLD_H
#include "gk75_spring_economy.h"
#include "gk71_order_budget.h"
#include "gk1_app_state.h"
/* Actual C11 mutation of an existing, independently initialized strategic
 * world, not a formula for the original NES initial Spring. Only the bounded
 * source-derived Spring FOOD->TAX->WAGES segment is implemented here.
 * First-year initialization, growth/birth, hazard RNG, AI, etc. MUST run from
 * their own authenticated scheduler before a Spring turn can be player-ready.
 */
typedef enum {GK78_OK=0,GK78_BAD_INPUT=1,GK78_NOT_INITIALIZED=2,
              GK78_INCONSISTENT_COUNTRY=3,GK78_SOURCE_FORMULA_REJECTED=4} Gk78Result;
typedef struct {
 uint16_t countries,food_used,tax_gross,payroll,troops_lost,starved_countries;
 uint16_t source_food_before[27],source_gold_before[27];
 uint16_t new_food[27],new_gold[27],new_troops[27];
} Gk78SpringReceipt;
/* Performs the verified Spring finance segment for ALL 14/27 countries in
 * the SAME native game's mutable world. Fails atomically, including receipt.
 * Does not advance season or budget and never unlocks PRE_SETUP. Phase2 is
 * RESERVED and the app never currently creates it; a caller supplying phase2
 * is NOT evidence of original NES parity or original first Spring setup.
 */
Gk78Result gk78_apply_spring_finance(Gk1App *app,uint8_t world_index,
                                    Gk78SpringReceipt *receipt);
#endif
