#ifndef GK64_MANUAL_RULES_H
#define GK64_MANUAL_RULES_H
#include <stdint.h>
#include "../execution_049/gk1_scenario_factory.h"
/* KOEI printed English NES manual, pages 5-6, 7/17, 18-19, 22, 24-27, 29,31.
 * MANUAL_DERIVED, not Mesen parity nor player-ready country initialization.
 */
typedef enum {GK64_OK=0,GK64_INVALID=1,GK64_UNRESOLVED=2} Gk64Status;
typedef enum {GK64_PLAIN=0,GK64_FOREST=1,GK64_MOUNTAIN=2,
 GK64_DESERT=3,GK64_WATER=4,GK64_TOWN=5,GK64_CASTLE=6} Gk64Terrain;
typedef enum {GK64_NO_RESULT=0,GK64_ATTACKER_RETREAT_TIME=1,
 GK64_ATTACKER_WINS_COMMAND=2,GK64_DEFENDER_WINS_COMMAND=3,
 GK64_ATTACKER_WINS_SURRENDER=4,GK64_DEFENDER_WINS_SURRENDER=5} Gk64BattleResult;
typedef struct {
 uint16_t first_order_year; /* spring 1175 Mongol; spring 1206 World */
 uint16_t population; /* original loaded PRE_SETUP template, NOT final Spring */
 uint16_t feed_whole; /* half population, integer portion */
 uint8_t feed_half; /* .5 on odd population; actual NES rounding UNKNOWN */
 uint16_t soldier_gold_wages; /* 1 gold per soldier; before losses/AI/tax */
 uint16_t troops, current_gold, current_food;
 uint8_t base_orders, direct_vassal_orders;
 uint8_t days_for_remaining_orders;
} Gk64ManualPreview;
typedef struct {
 uint8_t max_days, used_days;
 Gk64BattleResult result;
} Gk64BattleClock;
Gk64Status gk64_preview(const Gk1LoadedScenario *world,uint8_t country_index,
                        uint8_t remaining_orders,Gk64ManualPreview *out);
Gk64Status gk64_battle_begin(uint8_t remaining_orders,Gk64BattleClock *out);
Gk64Status gk64_battle_day(Gk64BattleClock *clock,Gk64BattleResult resolution);
Gk64Status gk64_terrain_mobility(Gk64Terrain terrain,uint8_t *cost);
Gk64Status gk64_pass_mobility(uint8_t current,uint8_t cavalry,uint8_t *next);
Gk64Status gk64_daughter_marriage(uint8_t age,uint8_t *eligible);
Gk64Status gk64_divide_cavalry(uint8_t enhanced,uint8_t mobility,uint8_t *eligible);
uint16_t gk64_first_spring_year(Gk1ScenarioId id);
#endif
