#ifndef GK211_FRESH_SEQUENCE_H
#define GK211_FRESH_SEQUENCE_H
#include <stdint.h>
#define GK211_STAGES 14u
/* A fixed, archived, ORIGINAL STOCK fresh-reset history. Not a general NES
 * first-Spring generator. Raw RAM, ROM and screenshots remain PRIVATE. */
typedef enum {
 GK211_OK=0, GK211_ARGUMENT, GK211_SOURCE_REFUSED,
 GK211_EVIDENCE_REFUSED, GK211_BANK_REFUSED, GK211_CHRONOLOGY_REFUSED
} Gk211Status;
typedef struct {
 uint32_t frame;
 uint64_t cpu_cycles;
 uint16_t year,gold,food,troops,rate;
 uint8_t orders,countries;
 uint8_t changed_mask; /* year=1,gold=2,food=4,troops=8,rate=16,orders=32,countries=64 */
} Gk211Stage;
typedef struct {
 Gk211Stage stage[GK211_STAGES];
 uint8_t validated_stages;
 uint8_t observed_setup_step,observed_food_doubling_step;
 uint8_t observed_year_rate_step,observed_spring_economy_step;
 uint8_t observed_order_grant_step;
 uint8_t source_observation_only;
 uint8_t independent_initialization_implemented;
} Gk211Report;
/* Read exactly the 14 owner-authenticated 2026-11-07 archived stage JSON
 * receipts from private_dir. Digest is checked for EACH complete file.
 * No caller output is changed on ANY rejection. */
Gk211Status gk211_verify_fresh_reset_sequence(const char *private_dir,
 Gk211Report *out);
#endif
