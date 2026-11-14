#ifndef GK280_ORIGINAL_RATE_BRIDGE_H
#define GK280_ORIGINAL_RATE_BRIDGE_H
#include "gk178_source_rng.h"
#include <stdint.h>
/* Original stock $C289 fixed-bank RNG and source-derived difficulty/Rate
 * arithmetic, combined in the real native app with an authenticated private
 * original ROM. Seed must come from an independently attested CPU RAM event;
 * these values NEVER establish setup draw chronology or enable a season. */
typedef enum {GK280_OK=0, GK280_ARGUMENT=1, GK280_STAGE=2,
              GK280_STOCK_ROM=3, GK280_PARITY=4} Gk280Status;
typedef struct {
 uint8_t source_seed[6], next_seed[6], difficulty, authenticated_rom;
 uint8_t candidate_only, original_setup_proven, orders_granted;
 uint16_t source_rate_candidate, raw_draw15;
} Gk280RateCandidate;
struct Gk1App; /* Expose app-facing helper through gk1_app_state.h. */
Gk280Status gk280_calculate_rate_source(const char *private_original_rom,
 const uint8_t original_seed[6],uint8_t difficulty,Gk280RateCandidate *out);
#endif
