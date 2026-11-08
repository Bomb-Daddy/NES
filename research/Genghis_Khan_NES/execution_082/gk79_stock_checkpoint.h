#ifndef GK79_STOCK_CHECKPOINT_H
#define GK79_STOCK_CHECKPOINT_H
#include "gk77_stock_spring.h"
#include "gk71_order_budget.h"
#include "../execution_049/gk1_scenario_factory.h"
/* Authenticated ORIGINAL NES checkpoint continuation, not ROM startup RNG.
 * A full, independently verified 14/27-country numeric checkpoint is staged
 * into the SAME native app, separate from unchanged PRE_SETUP source worlds.
 * Only manual-derived BASE Pass is playable; NO season advance, AI, Trade,
 * inheritance, or generated initial attributes is claimed.
 * No ROM or raw source checkpoint is embedded in public builds.
 */
typedef enum { GK79_OK=0,GK79_BAD=1,GK79_SOURCE_REJECTED=2,
               GK79_UNSUPPORTED_PLAYER=3,GK79_EXHAUSTED=4 } Gk79Result;
typedef struct {
 Gk1LoadedScenario world; /* 14/27 source-restored numeric country records */
 Gk71Budget orders;      /* real three-order BASE budget, manual-derived */
 uint16_t year,rate;
 uint8_t profile,player_index,view_index,valid,pending_pass;
} Gk79Checkpoint;
/* A decoded, validated source result can be passed here; caller must NEVER
 * substitute manually constructed profile values in production. Standalone
 * synthetic tests of this conversion are NOT original ROM observations. */
Gk79Result gk79_from_decoded(const Gk1LoadedScenario *pre,
                              const Gk77StockSpring *source,
                              uint8_t player_index,Gk79Checkpoint *out);
/* Production boundary: exact 14x83/27x83 bytes and known SHA required. */
Gk79Result gk79_open_original(const Gk1LoadedScenario *pre,
                              const char *private_path,Gk77Profile profile,
                              uint8_t player_index,Gk79Checkpoint *out);
/* Commit only manual-proven PASS: cancel must not touch any numeric country
 * field; no fake future season is generated after the remaining 3 are ended. */
Gk79Result gk79_pass(Gk79Checkpoint *session);
typedef enum {GK79_IO_OK=0,GK79_IO_INVALID=1,GK79_IO_EXISTS=2,
              GK79_IO_ERROR=3,GK79_IO_CORRUPT=4} Gk79IOStatus;
/* Save only SHA-bound capture identity + the PASS receipt, never licensed RAM.
 * Exclusive-create, 32-byte versioned file; import requires user reattach the
 * exact authenticated source checkpoint first, including on cold start. */
Gk79IOStatus gk79_save_receipt_new(const char *path,const Gk79Checkpoint *session);
Gk79IOStatus gk79_load_receipt(const char *path,Gk79Checkpoint *validated_session);
#endif
