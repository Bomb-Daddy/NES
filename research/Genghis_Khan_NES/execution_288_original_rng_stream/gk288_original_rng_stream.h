#ifndef GK288_ORIGINAL_RNG_STREAM_H
#define GK288_ORIGINAL_RNG_STREAM_H
#include <stdint.h>
/* Source-executed C289 draw stream, not an NES first-Spring initialization.
 * Seed MUST originate in independent original-CPU RAM evidence to be relevant.
 * ROM is read locally, validated against original-stock SHA, and never copied.
 * A contiguous stream is conditional: external seed writers, NMI/AI/menus,
 * and unknown setup calls make it unsafe to promote any result to a season.
 */
#define GK288_MAX_DRAWS 64u
typedef enum {
 GK288_OK=0, GK288_ARGUMENT=1, GK288_STAGE=2,
 GK288_ROM=3, GK288_PARITY=4
} Gk288Status;
typedef struct {
 uint8_t before[6],after[6],difficulty;
 uint8_t authenticated_original,conditional_stream;
 uint8_t actual_startup_seed_proven,first_spring_generated,orders_granted;
 uint16_t count;
 uint16_t draw15[GK288_MAX_DRAWS];
 uint16_t rate_candidates[GK288_MAX_DRAWS];
} Gk288Stream;
/* Fail-atomic output. No country, app, controller, game clock or save writes. */
Gk288Status gk288_calculate_original_stream(const char *private_rom,
 const uint8_t explicit_seed[6],uint8_t selected_difficulty,
 unsigned draws,Gk288Stream *out);
#endif
