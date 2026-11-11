#ifndef GK183_CHECKPOINT_ISOLATION_H
#define GK183_CHECKPOINT_ISOLATION_H
#include "gk1_app_state.h"
/* PORT-ONLY source/saved-profile isolation. No first Spring or NES behavior.
 * A private Classic save is invalid if ANY Enhanced derived era is active.
 * This gate is independently testable without private original-ROM bytes. */
int gk183_classic_storage_isolated(const Gk1App *app);
/* Call ONLY after a source-authenticated original checkpoint was validated.
 * The calling code must commit the new source in the same transaction.
 * Never touch canonical 14/27 pre-setup countries, original raw evidence,
 * explicit profile selection, or original controller order allowance. */
void gk183_reset_checkpoint_derived(Gk1App *app);
#endif
