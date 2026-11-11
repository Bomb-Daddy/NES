#ifndef GK187_CLASSIC_ASSIGN_H
#define GK187_CLASSIC_ASSIGN_H
#include "gk79_stock_checkpoint.h"
#include "gk116_enhanced_assign.h"
/* An original-source-attested, MANUAL-DERIVED Classic ASSIGN implementation.
 * No claim of NES binary parity, independent startup, character setup, AI,
 * seasonal economic updates, or unverified original order/RNG behavior.
 * Immutable original source is authoritative; mutable gameplay is separate.
 */
typedef enum {GK187_OK=0,GK187_INVALID=1,GK187_SOURCE=2,
 GK187_NO_WORKERS=3,GK187_EXHAUSTED=4} Gk187Result;
typedef struct {
 uint8_t from,to;
 uint16_t quantity;
} Gk187Action;
typedef struct {
 Gk1LoadedScenario mutable_world;
 Gk71Budget budget;
 Gk187Action actions[3];
 uint8_t action_count,active,passed,source_profile,player_index;
 uint8_t source_sha[32];
} Gk187ClassicAssign;
Gk187Result gk187_open(const Gk79Checkpoint *stock,Gk187ClassicAssign *out);
int gk187_intact(const Gk187ClassicAssign *s,const Gk79Checkpoint *stock);
Gk187Result gk187_transfer(Gk187ClassicAssign *s,const Gk79Checkpoint *stock,
 uint8_t from,uint8_t to,uint16_t quantity);
Gk187Result gk187_pass(Gk187ClassicAssign *s,const Gk79Checkpoint *stock);
#endif
