#ifndef GK144_ARCHIVED_MAPPER_H
#define GK144_ARCHIVED_MAPPER_H
#include <stddef.h>
#include <stdint.h>
#include "gk139_mesen_state.h"
/* Only the uncompressed named-field block of an independently authenticated
 * MesenCE .mss. Page mapping is the SAVED instant, not mapper power-on or
 * the earlier CPU/RNG/setup chronology. Never accepts arbitrary ROM bytes. */
typedef enum {
 GK144_MAPPER_OK=0,
 GK144_MAPPER_BAD_INPUT=1,
 GK144_MAPPER_STATE_REFUSED=2,
 GK144_MAPPER_UNSUPPORTED_MODE=3,
 GK144_MAPPER_CONTRADICTION=4
} Gk144Status;
typedef struct {
 uint8_t lower_prg_bank,upper_prg_bank,mapper_prg_reg;
 uint8_t mapper_prg_mode,mapper_slot_select;
 uint16_t saved_program_counter;
 uint64_t saved_cpu_cycles;
 Gk139CountryBank observed_country_ram_bank;
 uint16_t confirmed_prg_pages; /* 128 x 256-byte pages */
 uint8_t upper_bank_is_fixed_last;
 uint8_t mapping_observed_at_saved_instant_only;
} Gk144ArchivedMapper;
/* Every failure leaves `out` untouched. This is a source check and never
 * resolves which bank was mapped at original fresh RESET. Private originals
 * and savestates remain outside the public game package and GitHub. */
Gk144Status gk144_probe_archived_prg_map(const uint8_t *named_fields,
 size_t bytes,Gk144ArchivedMapper *out);
#endif
