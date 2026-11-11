#ifndef GK203_RNG_SELECTOR_WRITERS_H
#define GK203_RNG_SELECTOR_WRITERS_H
#include <stddef.h>
#include <stdint.h>
/* EXEC0203: source-only selector provenance for the AUTHENTIC original USA NES.
 * Exact ROM SHA gate is inherited from GK202. This does not emulate a CPU,
 * prove reachable execution, sample RNG, or initialize any Spring country. */
typedef enum {GK203_OK=0,GK203_ARGUMENT=1,GK203_SOURCE_REFUSED=2} Gk203Status;
#define GK203_FIXED_SITES 2u
typedef struct {
 uint16_t pc; /* LDA immediate start in fixed $C000 bank */
 uint8_t selector; /* immediate value written to zero-page $0050 */
 uint8_t subcommand; /* immediate value written to $0052 */
 uint8_t dispatch_slot_is_rng; /* exactly 17 when true; no dynamic inference */
 uint16_t dispatched_target; /* decoded from authenticated GK202 table */
} Gk203Site;
typedef struct {
 uint8_t authenticated_stock;
 uint8_t analysis_only;
 uint8_t rng_slot; /* 17, from GK202 */
 uint8_t fixed_caller_count; /* matched LDA# / STA$50 / LDA# / STA$52 / JSR$EF76 */
 uint8_t fixed_rng_caller_count;
 uint16_t raw_immediate_rng_slot_writes; /* across all 16 PRG banks; may include DATA */
 uint16_t raw_immediate_other_slot_writes; /* raw non-17 immediate source patterns */
 Gk203Site fixed[GK203_FIXED_SITES];
} Gk203Proof;
/* Refuse malformed source without modifying *out, even partially. */
Gk203Status gk203_from_bytes(const uint8_t *rom,size_t length,Gk203Proof *out);
Gk203Status gk203_from_file(const char *private_path,Gk203Proof *out);
#endif
