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
/* EXEC0276: a known original stock receipt is a historical continuation, not
 * evidence that an unrelated native wizard selected the same NES RNG path.
 * Fresh profile retains the EXEC0275 known-choice gate. Archived profiles
 * have NO source-proven chooser history: allow only untouched title setup.
 * This is a source-free, pure admission predicate, not an NES initializer. */
int gk162_original_setup_compatible(const Gk1App *app,Gk77Profile profile);
Gk162EntryStatus gk162_enter_original_checkpoint(Gk1App *app,
 const char *canonical_csv,const char *private_stock83,
 Gk77Profile profile,uint8_t player_index);
/* A strictly source-authenticated checkpoint already validated against the
 * 83-byte original-stock record SHA may use this identical atomic commit.
 * Re-validates profile, world, exact-source bytes and order invariants.
 * NOT a first-Spring generator or a bypass for PRE_SETUP source authority.
 */
Gk162EntryStatus gk162_commit_verified_checkpoint(Gk1App *app, const char *canonical_csv,
 const Gk79Checkpoint *candidate);
#endif
