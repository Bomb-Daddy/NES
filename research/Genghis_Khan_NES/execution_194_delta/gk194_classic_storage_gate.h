#ifndef GK194_CLASSIC_STORAGE_GATE_H
#define GK194_CLASSIC_STORAGE_GATE_H
#include "gk1_app_state.h"
/* Port-only controller OTHER slot access. If Classic ASSIGN has started,
 * require an exact replay of the immutable original-source checkpoint;
 * a forged or Enhanced phase can never open private persistence. */
int gk194_classic_storage_allowed(const Gk1App *app);
#endif
