#ifndef GK145_FRESH_PROGRESS_H
#define GK145_FRESH_PROGRESS_H
#include <stddef.h>
#include <stdint.h>
/* Evidence-only original stock NES fresh-reset controller progression.
 * The caller must authenticate its private raw Mesen source first: this
 * function is NOT a ROM emulator and cannot grant a playable season.
 * A positive Rate, changed Gold, and 3 orders are independent milestones.
 */
typedef struct {
 uint32_t frame;
 uint64_t cpu_cycles;
 uint16_t year,country_count,gold,rate,orders;
} Gk145FreshObservation;
typedef struct {
 uint8_t samples;
 uint8_t first_world_setup; /* 1-based sample, 0 if never observed */
 uint8_t first_spring_year;
 uint8_t first_nonzero_rate;
 uint8_t first_gold_change;
 uint8_t first_three_orders;
 uint8_t first_full_spring;
 uint8_t temporal_separation_observed;
} Gk145FreshProgress;
typedef enum { GK145_OK=0, GK145_ARGUMENT=1, GK145_INVALID_CHRONOLOGY=2,
               GK145_AMBIGUOUS_PROGRESS=3 } Gk145Status;
/* Atomic: reject malformed stages or years/scenario/country-count changes.
 * No specific Gold or Rate result is assumed; observed values vary by RNG.
 * A value of three orders is only an observation, not a turn grant. */
Gk145Status gk145_trace_fresh_progress(const Gk145FreshObservation *rows,
 size_t count, uint16_t scenario_year, uint16_t expected_countries,
 Gk145FreshProgress *out);
#endif
