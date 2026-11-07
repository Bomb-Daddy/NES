#ifndef GK64_MANUAL_RULES_H
#define GK64_MANUAL_RULES_H
#include <stdint.h>
#include "../execution_049/gk1_scenario_factory.h"
/* KOEI printed English NES manual: MANUAL_DERIVED only, not Mesen or Windows parity. */
typedef enum {GK64_OK=0,GK64_INVALID=1,GK64_UNRESOLVED=2} Gk64Status;
typedef enum {GK64_PLAIN=0,GK64_FOREST=1,GK64_MOUNTAIN=2,
 GK64_DESERT=3,GK64_WATER=4,GK64_TOWN=5,GK64_CASTLE=6} Gk64Terrain;
typedef enum {GK64_NO_RESULT=0,GK64_ATTACKER_RETREAT_TIME=1,
 GK64_ATTACKER_WINS_COMMAND=2,GK64_DEFENDER_WINS_COMMAND=3,
 GK64_ATTACKER_WINS_SURRENDER=4,GK64_DEFENDER_WINS_SURRENDER=5} Gk64BattleResult;
typedef struct {
 uint16_t first_order_year,population,feed_whole;
 uint8_t feed_half;
 uint16_t soldier_gold_wages,troops,current_gold,current_food;
 uint8_t base_orders,direct_vassal_orders,days_for_remaining_orders;
} Gk64ManualPreview;
typedef struct {uint8_t winter_severe_cold_eligible,summer_storm_eligible;} Gk64SeasonGeography;
/* MANUAL_DERIVED *eligibility*, never RNG/disaster occurrence. One-based
 * country IDs, original source scenarios and intact PRE_SETUP provenance. */
Gk64Status gk64_season_geography(const Gk1LoadedScenario *world,
                                uint8_t index,Gk64SeasonGeography *out);
/* Army unit type: 0 cavalry, 1 infantry, 2 archers. Manual p29.
 * Original NES threshold, not raw terrain-index decoding or ROM proof. */
Gk64Status gk64_divide_manual(uint8_t unit_type,uint8_t mobility,uint8_t *eligible);
typedef struct {uint8_t max_days,used_days; Gk64BattleResult result;} Gk64BattleClock;
Gk64Status gk64_preview(const Gk1LoadedScenario *world,uint8_t index,
                        uint8_t remaining_orders,Gk64ManualPreview *out);
Gk64Status gk64_battle_begin(uint8_t remaining_orders,Gk64BattleClock *out);
Gk64Status gk64_battle_day(Gk64BattleClock *clock,Gk64BattleResult resolution);
Gk64Status gk64_terrain_mobility(Gk64Terrain terrain,uint8_t *cost);
Gk64Status gk64_pass_mobility(uint8_t current,uint8_t cavalry,uint8_t *next);
Gk64Status gk64_daughter_marriage(uint8_t age,uint8_t *eligible);
Gk64Status gk64_divide_cavalry(uint8_t enhanced,uint8_t mobility,uint8_t *eligible);
uint16_t gk64_first_spring_year(Gk1ScenarioId id);
#endif
