#ifndef GK296_STOCK_SEED_ORIGIN_H
#define GK296_STOCK_SEED_ORIGIN_H
#include "gk294_seed_writer_attest.h"
#include <stdint.h>
#include <stddef.h>
/* The genuine fresh original ROM's pre-first-C289 witnessed source writes
 * touch $84..$87, but NOT $88/$89. Their initial RAM origin is UNKNOWN.
 * This source-only verifier checks a claimed four-writer subsequence and
 * one C289, without asserting an emulator ran it or a Spring turn exists. */
typedef enum {
 GK296_OK=0,GK296_ARGUMENT=1,GK296_STAGE=2,GK296_WRITER_SOURCE=3,
 GK296_MASK=4,GK296_CHRONOLOGY=5,GK296_SEED=6,GK296_C289=7
} Gk296Status;
typedef struct {
 Gk294Write source;
 uint8_t before;
 uint8_t committed; /* alleged value, must match before of next same byte */
} Gk296Witness;
typedef struct {
 uint8_t source_writer_mask;      /* on success, exactly 0x0f */
 uint8_t unknown_poweron_mask;    /* on success, exactly 0x30 */
 uint8_t original_rom_sha256_verified;
 uint8_t original_source_static_only; /* ALWAYS 1; witness NOT proven executed */
 uint8_t first_c289_math_parity;
 uint8_t fresh_spring_implemented; /* ALWAYS 0 */
 uint16_t first_draw15;
} Gk296Report;
Gk296Status gk296_check_four_writer_first_draw(
 const char *private_rom,const Gk296Witness *writes,size_t count,
 const uint8_t explicit_initial_seed[6],const uint8_t claimed_first_pre[6],
 const uint8_t claimed_first_post[6],uint16_t claimed_draw15,
 Gk296Report *out);
#endif
