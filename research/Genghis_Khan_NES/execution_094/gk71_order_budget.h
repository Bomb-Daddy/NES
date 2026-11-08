#ifndef GK71_ORDER_BUDGET_H
#define GK71_ORDER_BUDGET_H
#include <stdint.h>
#include "../execution_049/gk1_scenario_factory.h"
/* MANUAL AUDIT 0003 GK1-MAN-004..007,061: four seasons; up to three
 * base-country orders, one direct-government order, prince AI autonomous;
 * Pass from base forfeits all remaining orders. These are MANUAL_DERIVED,
 * not first-Spring ROM initialization or verified stock gameplay.
 * No interaction in PRE_SETUP may produce a live strategic order.
 */
typedef enum {GK71_SPRING=0,GK71_SUMMER=1,GK71_FALL=2,GK71_WINTER=3} Gk71Season;
typedef enum {GK71_BASE=0,GK71_DIRECT=1,GK71_PRINCE=2} Gk71Governor;
typedef enum {GK71_OK=0,GK71_INVALID=1,GK71_LOCKED=2,
              GK71_EXHAUSTED=3,GK71_PRINCE_AI=4} Gk71Result;
/* Phase 2 is *reserved* for a future independently authenticated
 * original first-Spring state; the current factory NEVER generates it.
 * Synthetic phase-2 fixtures in tests are NOT original ROM evidence.
 */
#define GK71_RESERVED_VERIFIED_PLAYABLE_PHASE 2u
typedef struct {
 uint8_t scenario, country_id, season, governor;
 uint8_t allowance, remaining, completed, ended, playable;
} Gk71Budget;
/* Atomic on invalid inputs; creates read-only preview for PRE_SETUP;
 * only a future genuinely authenticated phase-2 world could be playable. */
Gk71Result gk71_open(const Gk1LoadedScenario *world,uint8_t index,
                     Gk71Season season,Gk71Governor govt,Gk71Budget *out);
/* Underlying command engine must complete source-verified order first.
 * 'completed_effect=0' never spends an order. No game-world mutation.
 */
Gk71Result gk71_commit_completed(Gk71Budget *budget,uint8_t completed_effect);
/* Pass exhausts the current country's remaining allowance (p21).
 * Rejects PRE_SETUP and prince AI, leaves budget byte-identical. */
Gk71Result gk71_pass(Gk71Budget *budget);
/* Next season: only the complete original strategic scheduler may call
 * this after source-authenticated transition. No implicit hazard/AI RNG. */
Gk71Result gk71_next_season(const Gk1LoadedScenario *world,
                           Gk71Budget *budget,Gk71Season next);
#endif
