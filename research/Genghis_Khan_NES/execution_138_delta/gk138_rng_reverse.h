#ifndef GK138_RNG_REVERSE_H
#define GK138_RNG_REVERSE_H
#include <stdint.h>
/* Inverse of the EXACT source-derived GK137 48-bit NES PRNG step. This is a
 * research trace tool, NOT an original NES power-on seed or a live Spring.
 * The original step forces bit 0 of its input high. Consequently two initial
 * seeds (odd and preceding even) ALWAYS yield the same later stream.
 */
typedef enum { GK138_OK=0, GK138_BAD_INPUT=1 } Gk138Result;
typedef struct {
 uint64_t candidate_even;
 uint64_t candidate_odd;
 uint32_t draws_back;
} Gk138Reverse;
typedef struct {
 uint32_t matches;
 uint32_t nearest_offset; /* 0=immediately preceding last observed seed */
 uint32_t inspected;
} Gk138RateScan;
/* For draws_back=0 both candidates equal last_state, including even values.
 * For draws_back>=1 last_state must be odd. Bounded to 65536 draws for QA.
 * No changes to out on rejection.
 */
Gk138Result gk138_reverse(const uint64_t last_state, uint32_t draws_back,
                         Gk138Reverse *out);
/* Evaluate possible original Spring Rate draws in a post-setup state.
 * This is CONDITIONAL on externally authenticated post-setup seed and the
 * original call chronology. A numeric match NEVER proves causality.
 * oldest_search <=65536, 0 is invalid. Refuses impossible rate or difficulty.
 */
Gk138Result gk138_scan_spring_rate(uint64_t post_state, uint8_t difficulty,
                                   uint16_t observed_rate,
                                   uint32_t oldest_search,Gk138RateScan *out);
#endif
