#ifndef GK209_ARCHIVED_RATE_AMBIGUITY_H
#define GK209_ARCHIVED_RATE_AMBIGUITY_H
#include "gk208_archived_rng.h"
#include "gk138_rng_reverse.h"
/* EXEC0209: ROM-and-full-Mesen-state-attested *candidate* past Rate draws.
 * This is retrospective arithmetic only: a matching value cannot prove
 * which RNG call (if any) was used for Spring Rate, or recover startup setup.
 * Never modifies an original checkpoint, mutable campaign, ROM or Mesen file.
 */
typedef enum {
 GK209_OK=0,GK209_ARGUMENT=1,GK209_SOURCE_REFUSED=2,
 GK209_ARCHIVE_REFUSED=3,GK209_NOT_A_DRAW_BOUNDARY=4
} Gk209Status;
typedef struct {
 Gk77Profile profile;
 uint16_t observed_spring_rate;
 uint16_t archived_pc;
 uint32_t window;
 uint8_t candidate_difficulties;
 uint8_t uniquely_located_difficulties;
 uint8_t chronology_proved; /* always zero: no runtime RNG call log */
 uint8_t first_spring_generated; /* always zero */
 Gk138RateScan by_difficulty[5];
} Gk209RateAmbiguity;
/* Bounded 1..65536 reverse draws, independent for each difficulty (1..5).
 * Returns NOT_A_DRAW_BOUNDARY if the original CPU seed is even: a partial
 * RNG computation/non-draw write is possible, so reverse-draw provenance
 * must not be fabricated. On every refusal, *out is byte-identical. */
Gk209Status gk209_archived_rate_ambiguity(const char *original_rom,
 const char *archived_mss,Gk77Profile profile,uint32_t window,
 Gk209RateAmbiguity *out);
#endif
