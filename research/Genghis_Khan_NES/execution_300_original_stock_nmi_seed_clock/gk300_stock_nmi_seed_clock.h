#ifndef GK300_STOCK_NMI_SEED_CLOCK_H
#define GK300_STOCK_NMI_SEED_CLOCK_H
#include <stddef.h>
#include <stdint.h>
/* Original USA NES fixed-bank NMI $C106-$C12C. Under a ZERO $008A gate,
 * two independent 16-bit words (high $84/low $85, high $86/low $87)
 * advance by +2; nonzero gate suppresses both updates. An observed CPU NMI
 * sequence is required: elapsed VIDEO frames alone are not proof of an NMI.
 * $88/$89 are NOT touched by this routine, but may be modified elsewhere.
 * The first C289 seed and original first Spring remain NOT established.
 * No private NES program data, screenshots, assets, save states or ROMs here. */
#define GK300_MAX_NMIS 8192u
typedef enum {
 GK300_OK=0, GK300_ARGUMENT=1, GK300_ROM_IO=2,
 GK300_ROM_IDENTITY=3, GK300_SOURCE_MISMATCH=4, GK300_STAGE=5
} Gk300Status;
typedef struct {
 uint8_t initial[6],after[6];
 uint16_t nmi_observations,enabled_nmis,gated_nmis;
 uint8_t original_rom_sha256_verified; /* 1 source-authenticated, else 0 */
 uint8_t conditional_history; /* never implies sample authenticity */
 uint8_t first_c289_seed_proven; /* ALWAYS 0; other writers exist */
 uint8_t gameplay_spring_unlocked; /* ALWAYS 0 */
} Gk300NmiResult;
/* Arithmetic-only source model, flagged unverified; independently testable. */
Gk300Status gk300_model_nmi_gate_history(const uint8_t initial[6],
 const uint8_t *observed_gate_8a,size_t count,Gk300NmiResult *out);
/* Validates the 262160-byte original exact-SHA ROM and fixed-bank NMI
 * instructions before computing the SAME conditional arithmetic. No input
 * source is read from public stock assets or assumed power-on seed bytes. */
Gk300Status gk300_authentic_nmi_gate_history(const char *private_original_rom,
 const uint8_t initial[6],const uint8_t *observed_gate_8a,
 size_t count,Gk300NmiResult *out);
#endif
