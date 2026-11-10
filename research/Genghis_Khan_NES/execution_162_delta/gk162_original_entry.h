#ifndef GK162_ORIGINAL_ENTRY_H
#define GK162_ORIGINAL_ENTRY_H
#include "gk1_app_state.h"
/* Original-stock SOURCE-CHECKPOINT continuation, not a new original NES setup.
 * The owner's exact 83-byte records must pass the existing independent SHA,
 * CSV identity, scenario and projection gates before touching any live app.
 * No ROM bytes are shipped, no original RNG/merchant/AI are invented.
 */
typedef enum {
 GK162_ENTRY_OK=0,
 GK162_ENTRY_ARGUMENT=1,
 GK162_ENTRY_TEMPLATE=2,
 GK162_ENTRY_SOURCE=3
} Gk162EntryStatus;
/* Atomic on failure, including source/bank mismatch. On success invalidates
 * ALL prior Enhanced derived stages, including year-2, without altering either
 * pristine PRE_SETUP template or the user's explicit Classic/Enhanced choice.
 * The native controller gets the real first-Spring checkpoint menu with only
 * pre-existing, source-bounded VIEW/PASS actions; it does not independently
 * generate the NES first Spring nor imply 3 source-implemented actions.
 */
Gk162EntryStatus gk162_enter_original_checkpoint(Gk1App *app,
 const char *canonical_csv,const char *private_stock83,
 Gk77Profile profile,uint8_t player_index);
#endif
