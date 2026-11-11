#ifndef GK176_ENHANCED_CYCLE_H
#define GK176_ENHANCED_CYCLE_H
#include "gk161_enhanced_year2.h"
/* Post-year-two approved Enhanced LABOR-ONLY continuation. This does NOT
 * simulate original NES food, taxes, aging, events, AI, diplomacy or RNG.
 * Exact 20-season history is replayed against the immutable stock witness. */
#define GK176_MAX_SEASONS 20u
#define GK176_MAX_ACTIONS 3u
typedef enum {
 GK176_OK=0,GK176_INPUT,GK176_SOURCE,GK176_NO_ORDERS,
 GK176_NO_WORKER,GK176_END
} Gk176Result;
typedef struct {
 uint8_t action_count,pass_ended;
 Gk116LaborAction actions[GK176_MAX_ACTIONS];
} Gk176SeasonReceipt;
typedef struct {
 Gk1LoadedScenario mutable_world;
 Gk71Budget budget;
 uint16_t year;
 uint8_t active,season_index;
 Gk176SeasonReceipt receipts[GK176_MAX_SEASONS];
} Gk176EnhancedCycle;
Gk176Result gk176_open(const Gk161EnhancedYear2 *y,
 const Gk116EnhancedAssign *sp,const Gk158EnhancedSummer *su,
 const Gk159EnhancedFall *fa,const Gk160EnhancedWinter *wi,
 const Gk79Checkpoint *original,Gk176EnhancedCycle *out);
int gk176_attestation_intact(const Gk176EnhancedCycle *e,
 const Gk161EnhancedYear2 *y,const Gk116EnhancedAssign *sp,
 const Gk158EnhancedSummer *su,const Gk159EnhancedFall *fa,
 const Gk160EnhancedWinter *wi,const Gk79Checkpoint *original);
Gk176Result gk176_transfer(Gk176EnhancedCycle *e,
 const Gk161EnhancedYear2 *y,const Gk116EnhancedAssign *sp,
 const Gk158EnhancedSummer *su,const Gk159EnhancedFall *fa,
 const Gk160EnhancedWinter *wi,const Gk79Checkpoint *original,
 uint8_t from,uint8_t to,uint16_t amount);
Gk176Result gk176_pass(Gk176EnhancedCycle *e,
 const Gk161EnhancedYear2 *y,const Gk116EnhancedAssign *sp,
 const Gk158EnhancedSummer *su,const Gk159EnhancedFall *fa,
 const Gk160EnhancedWinter *wi,const Gk79Checkpoint *original);
Gk176Result gk176_advance(Gk176EnhancedCycle *e,
 const Gk161EnhancedYear2 *y,const Gk116EnhancedAssign *sp,
 const Gk158EnhancedSummer *su,const Gk159EnhancedFall *fa,
 const Gk160EnhancedWinter *wi,const Gk79Checkpoint *original);
#endif
