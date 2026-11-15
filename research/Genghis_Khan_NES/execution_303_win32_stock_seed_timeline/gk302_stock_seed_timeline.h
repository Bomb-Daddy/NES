#ifndef GK302_STOCK_SEED_TIMELINE_H
#define GK302_STOCK_SEED_TIMELINE_H
#include <stddef.h>
#include <stdint.h>
/* EXEC0302: a strict, chronological CPU-claim replay across original 6502
 * NMI +2, directly source-attested seed stores, and original C289 calls.
 * Input is untrusted claimed observations, never proof that NES executed.
 * No first Spring or player-order unlock; no ROM or state embedded. */
#define GK302_MAX_EVENTS 4096u
typedef enum { GK302_NMI=1,GK302_STORE=2,GK302_C289=3 } Gk302Kind;
typedef struct {
 uint64_t cpu_cycle;
 Gk302Kind kind;
 uint16_t source_pc; /* STORE: original CPU instruction address */
 uint16_t draw15; /* C289: independently witnessed return */
 uint8_t bank; /* STORE: mapped 16 KiB PRG bank */
 uint8_t target; /* STORE: $84-$89 */
 uint8_t gate; /* NMI: observed $008A; nonzero suppresses both clocks */
 uint8_t before,after; /* STORE: single byte before/committed */
 uint8_t rng_before[6],rng_after[6]; /* C289: whole seed witness */
} Gk302SeedEvent;
typedef enum {
 GK302_OK=0,GK302_ARGUMENT=1,GK302_ORDER=2,GK302_VALUE=3,
 GK302_ROM=4,GK302_OPCODE=5,GK302_RNG=6,GK302_STAGE=7
} Gk302Status;
typedef struct {
 uint8_t initial[6],final[6];
 uint16_t event_count,seed_stores,nmi_enabled,nmi_gated,c289_calls;
 uint8_t source_store_targets_mask;
 uint8_t original_rom_sha256_verified;
 uint8_t source_claims_only; /* ALWAYS 1, not Mesen execution proof */
 uint8_t original_first_spring_generated; /* ALWAYS 0 */
 uint8_t original_orders_granted; /* ALWAYS 0 */
} Gk302SeedResult;
/* Checks every STORE source opcode against the SHA-pinned private ROM in
 * 64-write chunks, preserving chronology across the entire trace. The ROM
 * is an external input; this library does not distribute original assets.
 * Refusal is atomic for *out and does not modify event/input buffers. */
Gk302Status gk302_replay_stock_seed_timeline(const char *private_rom,
 const uint8_t initial[6],const Gk302SeedEvent *events,size_t count,
 Gk302SeedResult *out);
#endif
