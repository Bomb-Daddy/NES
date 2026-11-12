#ifndef GK213_STOCK_SPRING_BOUNDARY_H
#define GK213_STOCK_SPRING_BOUNDARY_H
#include "gk75_spring_economy.h"
#include "gk211_fresh_sequence.h"
/* EXEC0213: a source-arithmetic consistency check of the authenticated
 * SINGLE Mongol country-one fresh Spring1175 history. Never generates
 * character RNG, remaining countries, merchant events, or playable orders.
 * The source-observed post-setup economy/morale and observed new troops are
 * INPUTS, not presumed changes derived by this function. */
typedef enum { GK213_OK=0, GK213_ARGUMENT=1, GK213_UNGROUNDED=2,
               GK213_INCONSISTENT=3 } Gk213Status;
typedef struct {
 uint16_t stock_pre_food,stock_pre_gold,stock_pre_troops;
 uint16_t stock_pre_town,stock_pre_masons,stock_pre_artisans,stock_pre_food_workers;
 uint16_t observed_post_setup_economy,observed_post_setup_morale,observed_tax;
 uint16_t observed_post_season_troops;
} Gk213Inputs;
typedef struct {
 uint16_t food_after_start_bonus;
 uint16_t food_after_feeding;
 uint16_t gold_after_taxes_and_wages;
 uint16_t troops_before_growth,troops_after_observed_growth;
 uint16_t tax_gross,wages;
 uint8_t source_observation_only,generated_rng,independent_first_spring,orders_granted;
} Gk213Boundary;
/* Requires externally authenticated GK211 full fresh chronology and exact
 * country-one original stock template values. Only checks two source-bounded
 * finance boundaries, without claiming intermediate economy was observed.
 * Refuses impossible numbers atomically. */
Gk213Status gk213_check_observed_mongol_boundary(const Gk213Inputs *,
 const Gk211Report *authenticated_trace,Gk213Boundary *);
#endif
