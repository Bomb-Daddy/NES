#ifndef GK160_ENHANCED_WINTER_H
#define GK160_ENHANCED_WINTER_H
#include "gk159_enhanced_fall.h"
/* Approved ENHANCED-ONLY source-bound fourth labor season. This DOES NOT
 * implement the NES winter storm/cold, food, taxation, AI, RNG, dynasty, or
 * independent original playable Spring. Original source is immutable. */
#define GK160_MAX_ACTIONS 3u
#define GK160_RECEIPT_BYTES 160u
typedef enum {
 GK160_OK=0,GK160_INPUT=1,GK160_SOURCE=2,GK160_NO_ORDERS=3,
 GK160_NO_WORKER=4,GK160_IO=5,GK160_EXISTS=6,GK160_CORRUPT=7
} Gk160Result;
typedef struct {
 Gk1LoadedScenario mutable_world;
 Gk71Budget budget;
 uint8_t active,action_count,pass_ended;
 Gk116LaborAction actions[GK160_MAX_ACTIONS];
} Gk160EnhancedWinter;
int gk160_attestation_intact(const Gk160EnhancedWinter *winter,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk159EnhancedFall *fall,const Gk79Checkpoint *original);
Gk160Result gk160_open(const Gk116EnhancedAssign *spring,
 const Gk158EnhancedSummer *summer,const Gk159EnhancedFall *fall,
 const Gk79Checkpoint *original,Gk160EnhancedWinter *out);
Gk160Result gk160_transfer(Gk160EnhancedWinter *winter,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk159EnhancedFall *fall,const Gk79Checkpoint *original,
 uint8_t from,uint8_t to,uint16_t quantity);
Gk160Result gk160_pass(Gk160EnhancedWinter *winter,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk159EnhancedFall *fall,const Gk79Checkpoint *original);
Gk160Result gk160_save_new(const char *path,const Gk79Checkpoint *original,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk159EnhancedFall *fall,const Gk160EnhancedWinter *winter);
Gk160Result gk160_cold_load(const char *path,const Gk79Checkpoint *original,
 uint8_t enhanced,Gk116EnhancedAssign *spring_out,
 Gk158EnhancedSummer *summer_out,Gk159EnhancedFall *fall_out,
 Gk160EnhancedWinter *winter_out);
#endif
