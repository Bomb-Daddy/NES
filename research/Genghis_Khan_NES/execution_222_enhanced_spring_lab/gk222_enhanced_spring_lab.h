#ifndef GK222_ENHANCED_SPRING_LAB_H
#define GK222_ENHANCED_SPRING_LAB_H
#include "gk214_spring_batch.h"
/* EXEC0222: explicitly non-stock ENHANCED finance/order practice.
 * It never promotes the original PRE_SETUP templates or claims original NES
 * ruler generation, difficulty, RNG, birth, AI, or merchant scheduling.
 * A real mutable three-order drill separate from Classic original gameplay. */
typedef enum {
 GK222_OK=0, GK222_BAD_SOURCE, GK222_BAD_PLAYER, GK222_NOT_STARTED,
 GK222_NO_ORDERS, GK222_WRONG_COUNTRY, GK222_NO_WORKER, GK222_BAD_ACTION
} Gk222Status;
typedef enum {GK222_PASS=0,GK222_ASSIGN_TOWN_TO_FOOD=1} Gk222Action;
typedef struct {
 uint8_t active,scenario,player_index,selected_country,remaining,ended;
 uint16_t reference_year;
 Gk1LoadedScenario working; /* distinct from immutable original-source world */
 uint32_t taxes,payroll,food_used;
 uint8_t actions_committed;
} Gk222Spring;
Gk222Status gk222_open(const Gk1LoadedScenario *source,
                       uint8_t player_index,Gk222Spring *out);
Gk222Status gk222_apply(Gk222Spring *in_out,
                        uint8_t selected_country,Gk222Action action);
#endif
