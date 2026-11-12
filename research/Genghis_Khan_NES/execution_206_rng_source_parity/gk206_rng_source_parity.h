#ifndef GK206_RNG_SOURCE_PARITY_H
#define GK206_RNG_SOURCE_PARITY_H
#include <stdint.h>
/* Bounded, deterministic original-stock RNG arithmetic QA. NOT a replay of
 * original reset, mapper/PPU timing or inferred first-Spring seed history.
 * No original cartridge bytes or screenshots are stored in this report. */
typedef enum {
 GK206_PASS=0, GK206_BAD_ARGUMENT=1, GK206_SOURCE_REFUSED=2,
 GK206_COEFFICIENT_MISMATCH=3
} Gk206Status;
typedef struct {
 uint32_t initial_seeds;
 uint32_t draw_pairs;
 uint32_t spring_rate_pairs;
 uint32_t mismatches;
 uint8_t source_authenticated;
 uint8_t original_first_spring_generated; /* Always false. */
} Gk206Audit;
/* The provided bytes are arbitrary test coefficients, NOT authenticated
 * cartridge data. If they are wrong, the test must refuse rather than
 * unconditionally report agreement with the native arithmetic model. */
Gk206Status gk206_compare_coefficients(const uint8_t coefficients[6],
 Gk206Audit *out);
/* Authenticates the exact private 262160B USA NES image and reads the
 * six coefficients in source ROM fixed PRG bank 15, CPU $C2EC. No licensed
 * bytes are emitted or retained after use. Bad source leaves out unchanged. */
Gk206Status gk206_compare_original_stock(const char *private_rom,
 Gk206Audit *out);
#endif
