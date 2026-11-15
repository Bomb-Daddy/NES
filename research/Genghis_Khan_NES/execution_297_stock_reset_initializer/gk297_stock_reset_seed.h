#ifndef GK297_STOCK_RESET_SEED_H
#define GK297_STOCK_RESET_SEED_H
#include <stdint.h>
/* EXEC0297: original-USA fixed-bank reset initializer SOURCE semantics.
 * The witnessed CPU history has MANY subsequent writes before C289, so this
 * initial four-byte source prefix MUST NOT be mistaken for C289's seed. */
typedef enum {GK297_OK=0,GK297_ARGUMENT=1,GK297_ROM_IO=2,
 GK297_ROM_IDENTITY=3,GK297_SOURCE_MISMATCH=4,GK297_STAGE=5} Gk297Status;
typedef struct {
 uint8_t reset_initializer_seed[6]; /* supplied tail bytes are NOT source-proven */
 uint8_t original_source_written_mask; /* exactly 0x0F */
 uint8_t externally_observed_ram_mask; /* exactly 0x30 */
 uint8_t original_rom_sha256_verified;
 uint8_t source_static_only; /* true: no new NES runtime execution */
 uint8_t first_c289_seed_proven; /* ALWAYS false, intervening writes exist */
 uint8_t gameplay_spring_unlocked; /* ALWAYS false */
} Gk297ResetSeed;
Gk297Status gk297_reset_initializer_from_source(
 const char *private_original_rom,const uint8_t observed_ram_tail[2],
 Gk297ResetSeed *out);
#endif
