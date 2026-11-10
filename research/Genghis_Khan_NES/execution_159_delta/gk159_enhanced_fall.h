#ifndef GK159_ENHANCED_FALL_H
#define GK159_ENHANCED_FALL_H
#include "gk158_enhanced_summer.h"
/* Enhanced research-only Fall: NO KOEI seasonal harvest or source-derived
 * AI/RNG. Source-attested Spring -> Summer -> Fall, three labor orders per
 * turn. Classic remains locked until independently reproduced NES startup. */
#define GK159_MAX_ACTIONS 3u
#define GK159_RECEIPT_BYTES 144u
typedef enum {
 GK159_OK=0,GK159_INPUT=1,GK159_SOURCE=2,GK159_NO_ORDERS=3,
 GK159_NO_WORKER=4,GK159_IO=5,GK159_EXISTS=6,GK159_CORRUPT=7
} Gk159Result;
typedef struct {
 Gk1LoadedScenario mutable_world;
 Gk71Budget budget;
 uint8_t active,action_count,pass_ended;
 Gk116LaborAction actions[GK159_MAX_ACTIONS];
} Gk159EnhancedFall;
int gk159_attestation_intact(const Gk159EnhancedFall *fall,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk79Checkpoint *original);
Gk159Result gk159_open(const Gk116EnhancedAssign *spring,
 const Gk158EnhancedSummer *summer,const Gk79Checkpoint *original,
 Gk159EnhancedFall *out);
Gk159Result gk159_transfer(Gk159EnhancedFall *fall,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk79Checkpoint *original,uint8_t from,uint8_t to,uint16_t quantity);
Gk159Result gk159_pass(Gk159EnhancedFall *fall,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk79Checkpoint *original);
Gk159Result gk159_save_new(const char *path,const Gk79Checkpoint *original,
 const Gk116EnhancedAssign *spring,const Gk158EnhancedSummer *summer,
 const Gk159EnhancedFall *fall);
Gk159Result gk159_cold_load(const char *path,const Gk79Checkpoint *original,
 uint8_t enhanced,Gk116EnhancedAssign *spring_out,
 Gk158EnhancedSummer *summer_out,Gk159EnhancedFall *fall_out);
#endif
