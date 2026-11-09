#ifndef GK116_ENHANCED_ASSIGN_H
#define GK116_ENHANCED_ASSIGN_H
#include "gk79_stock_checkpoint.h"
#include <stdint.h>
/* EXEC0116. A real, mutable ENHANCED-only strategic LABOR action starting
 * from a SHA-authenticated original Spring checkpoint. It is NOT an original
 * NES Assign implementation, original setup/RNG reconstruction or Classic
 * rule claim. Original source RAM and Gk79Checkpoint remain immutable.
 * The distinct mutable world is never mistaken for source authority. */
#define GK116_MAX_ACTIONS 3u
typedef enum {
 GK116_OK=0, GK116_INPUT=1, GK116_NEEDS_ORIGINAL=2,
 GK116_CLASSIC_LOCKED=3, GK116_SOURCE_CHANGED=4, GK116_NO_WORKER=5,
 GK116_NO_ORDERS=6, GK116_IO=7, GK116_EXISTS=8, GK116_CORRUPT=9
} Gk116Result;
typedef struct {uint8_t from,to;uint16_t quantity;} Gk116LaborAction;
typedef struct {
 Gk1LoadedScenario mutable_world;
 Gk71Budget budget;
 uint8_t active,profile,player_index,source_sha[32],action_count;
 uint8_t pass_ended; /* EXEC0119: Enhanced-only Pass remaining, never stock source mutation */
 uint16_t year,rate;
 Gk116LaborAction actions[GK116_MAX_ACTIONS];
} Gk116EnhancedAssign;
/* The arithmetic primitive is explicitly SOURCE-NEUTRAL, for hostile QA.
 * Only gk116_transfer enforces the real source-provenance boundary. */
Gk116Result gk116_worker_math(Gk1LoadedScenario *world,Gk71Budget *budget,
 uint8_t country_index,uint8_t from,uint8_t to,uint16_t quantity);
Gk116Result gk116_open(const Gk79Checkpoint *stock,uint8_t enhanced,
 Gk116EnhancedAssign *out);
Gk116Result gk116_transfer(Gk116EnhancedAssign *session,
 const Gk79Checkpoint *original,uint8_t from,uint8_t to,uint16_t quantity);
Gk116Result gk116_pass_remaining(Gk116EnhancedAssign *session,
 const Gk79Checkpoint *original);
int gk116_attestation_intact(const Gk116EnhancedAssign *session,
 const Gk79Checkpoint *original);
/* 96-byte exclusive-create receipt (backward-compatible version-1 byte 14 Pass flag): original source 83-byte records never
 * serialized, must be privately reattached and reverified for cold load. */
Gk116Result gk116_save_new(const char *path,const Gk79Checkpoint *original,
 const Gk116EnhancedAssign *session);
Gk116Result gk116_cold_load(const char *path,const Gk79Checkpoint *original,
 uint8_t enhanced,Gk116EnhancedAssign *out);
#endif
