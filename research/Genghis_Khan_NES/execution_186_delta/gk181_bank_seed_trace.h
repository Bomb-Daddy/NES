#ifndef GK181_BANK_SEED_TRACE_H
#define GK181_BANK_SEED_TRACE_H
#include <stdint.h>
#include <stddef.h>
#include "gk180_seed_writer_scan.h"
/* CONDITIONAL fixed + MMC1 selected-bank static source audit. This enumerates
 * candidates under one CALLER-EXPLICIT bank hypothesis, not chronological
 * original NES execution or source-derived initial seed. Stops at possible
 * mapper writes, unresolved indirect flow and unsupported opcodes. */
#define GK181_MAX_SITES 256u
#define GK181_MAX_RNG_CALLS 512u
typedef enum {
 GK181_OK=0, GK181_BAD_INPUT=1, GK181_SOURCE=2, GK181_OVERFLOW=3
} Gk181Result;
typedef struct {
 uint16_t pc;
 uint8_t bank; /* 15=immutable high bank, otherwise assumed selected low bank */
 uint8_t seed_addr; /* ZP $84..$89, direct unindexed only */
 uint8_t kind;      /* GK180_STORE / GK180_RMW */
 uint8_t roots;     /* 1 RESET, 2 NMI, 4 IRQ */
} Gk181Site;
typedef struct {
 uint16_t pc;
 uint8_t bank; /* 15 fixed, else explicit low-bank HYPOTHESIS */
 uint8_t roots; /* root reachability only: RESET=1 NMI=2 IRQ=4 */
} Gk181RngCall;
typedef struct {
 uint8_t assumed_low_bank;
 uint16_t vectors[3];
 uint32_t decoded[3], low_bank_decoded[3], fixed_bank_decoded[3];
 uint32_t low_seed_candidates, high_seed_candidates, source_sites;
 uint32_t mapper_barriers, indirect_jumps, unresolved_indexed_writes;
 uint32_t unsupported_opcodes, external_edges, dispatcher_calls;
 Gk181Site sites[GK181_MAX_SITES];
 uint16_t rng_call_count; /* source-decoded JSR $C289, NOT executed */
 uint16_t fixed_rng_call_count,low_rng_call_count;
 Gk181RngCall rng_calls[GK181_MAX_RNG_CALLS];
 /* No field asserting seed chronology, deterministic initial Spring or turn. */
} Gk181Map;
/* Structural-only synthetic entry; not authenticated as owned original ROM. */
Gk181Result gk181_scan_image(const uint8_t *image,size_t n,
 uint8_t hypothesized_bank,Gk181Map *out);
/* Exact SHA256 original stock auth before any source-derived output. */
Gk181Result gk181_scan_authenticated(const char *private_rom,
 uint8_t hypothesized_bank,Gk181Map *out);
#endif
