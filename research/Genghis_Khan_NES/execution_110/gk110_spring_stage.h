#ifndef GK110_SPRING_STAGE_H
#define GK110_SPRING_STAGE_H
#include "gk75_spring_economy.h"
#include "../execution_049/gk1_scenario_factory.h"
/* EXEC0110: all 14 / all 27 country projections of ONLY source-confirmed
 * Spring food, tax, and payroll. It is a non-committable intermediate,
 * never actual setup, first Spring, original RNG or a player-ready season.
 * The output is *not* a Gk1LoadedScenario and deliberately has no method
 * to become one. Authentic source template identity is required. */
typedef enum {
 GK110_OK=0, GK110_ARGUMENT=1, GK110_SOURCE_REFUSED=2,
 GK110_BASELINE_DRIFT=3, GK110_NUMERIC_REFUSED=4
} Gk110Result;
typedef struct {
 uint8_t scenario_id, country_count;
 uint8_t setup_resolved,first_spring_ready,orders_granted;
 Gk75SpringResult country[GK1_COUNTRIES_WORLD];
 uint32_t food_before,food_after,gold_before,gold_after;
 uint32_t troops_before,troops_after,tax_total,food_consumed,payroll_total;
} Gk110SpringStage;
/* Fail-closed and atomic; no changes to input world or output upon rejection.
 * Uses original CSV before any player/difficulty/RNG/other seasonal transforms.
 * Defers authoritatively unknown setup/seasonal steps and 3-order entry. */
Gk110Result gk110_prepare_source_finance(const char *verified_csv,
 const Gk1LoadedScenario *template_world,Gk110SpringStage *out);
#endif
