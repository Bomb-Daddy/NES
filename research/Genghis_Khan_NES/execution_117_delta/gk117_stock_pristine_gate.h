#ifndef GK117_STOCK_PRISTINE_GATE_H
#define GK117_STOCK_PRISTINE_GATE_H
#include "gk79_stock_checkpoint.h"
/* EXEC0117 — source authority for original-stock RESTORED checkpoints.
 * This does NOT create an authentic first Spring, invent RNG, or unlock any
 * original NES command. It closes a production trust-boundary gap: a real
 * 83-byte stock checkpoint was previously attached to arbitrary mutated
 * pre-setup port fields, which are NOT evidence from original NES RAM. */
typedef enum {
 GK117_OK=0,
 GK117_ARGUMENT=1,
 GK117_CSV_SOURCE_REFUSED=2,
 GK117_TEMPLATE_DRIFT=3,
 GK117_ORIGINAL_RECORD_REFUSED=4
} Gk117Status;
/* Checked at point of ORIGINAL checkpoint attachment in the native shell.
 * Requires both SHA-identified 41-country CSV AND exact canonical 14/27
 * PRE_SETUP state before allowing the existing original SHA record attach.
 * On any failure, *out is byte-identical to its entry state. */
Gk117Status gk117_open_attested_original(const char *source_csv,
 const Gk1LoadedScenario *pre,const char *private_records,
 Gk77Profile profile,uint8_t player_index,Gk79Checkpoint *out);
#endif
