#ifndef GK327_APP_RNG_BRIDGE_H
#define GK327_APP_RNG_BRIDGE_H
#include "gk1_app_state.h"
#include "gk327_fixed6502_rng.h"
/* Same inherited native Gk1App. Read-only stock ROM instruction execution.
 * Caller-supplied seed is a *candidate*, not an independently witnessed
 * original NES timer/choice seed; result must not unlock PRE_SETUP/orders. */
Gk327Status gk327_app_source_rng_candidate(const Gk1App *app,
 const uint8_t *private_original_rom,size_t size,
 const uint8_t seed_candidate[6],Gk327RngReceipt *out);
#endif
