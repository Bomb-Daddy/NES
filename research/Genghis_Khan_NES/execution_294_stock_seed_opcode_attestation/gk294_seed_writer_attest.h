#ifndef GK294_SEED_WRITER_ATTEST_H
#define GK294_SEED_WRITER_ATTEST_H
#include <stdint.h>
#include <stddef.h>
/* EXEC0294 — Copyright-clean, authenticated 6502 STORE-opcode cross-check.
 * Raw source PC/bank claims are UNTRUSTED: a matching store in ROM does NOT
 * establish that Mesen actually executed it, nor prove the value written.
 * No original ROM bytes or gameplay data are distributed with this module.
 */
#define GK294_MAX_WRITES 64u
typedef enum {
 GK294_OK=0, GK294_ARGUMENT=1, GK294_ROM_IO=2, GK294_ROM_IDENTITY=3,
 GK294_SOURCE_MISMATCH=4, GK294_BANK_MISMATCH=5, GK294_CHRONOLOGY=6,
 GK294_INCOMPLETE=7, GK294_STAGE=8
} Gk294Status;
typedef struct {
 uint64_t cpu_cycle;          /* chronological CPU-cycle claim, not proof */
 uint16_t source_pc;          /* alleged executing STORE opcode address */
 uint8_t mapped_prg_bank;     /* 0..15 in 16 KiB MMC1 bank mapping */
 uint8_t destination_zp;     /* must be $84..$89 */
} Gk294Write;
typedef struct {
 uint8_t opcode;             /* authenticated ROM opcode, no PRG embedded */
 uint8_t target;             /* $84..$89 */
 uint8_t mapped_prg_bank;
 uint8_t count;
 uint8_t unique_target_mask; /* 0x3F means all six seed bytes observed */
 uint8_t source_candidates_only; /* ALWAYS 1: no live CPU proof */
 uint8_t original_rom_sha256_verified; /* source bytes verified once */
} Gk294SourceProof;
/* Reject all indirect, indexed, stack, or unclear write modes instead of
 * mistaking a nearby byte sequence for observed source. Direct zero-page
 * STA/STX/STY and absolute stores to zero-page addresses only.
 * Failure leaves *out unchanged; successful result cannot unlock gameplay.
 */
Gk294Status gk294_attest_seed_writers(const char *private_rom,
 const Gk294Write *claimed, size_t count, Gk294SourceProof *out);
#endif
