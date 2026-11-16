#ifndef GK327_FIXED6502_RNG_H
#define GK327_FIXED6502_RNG_H
#include <stddef.h>
#include <stdint.h>
/* Source-executing, bounded NMOS 6502 interpreter for the original fixed-bank
 * RNG subroutine, not a guessed random-number formula. No ROM bytes included.
 * It does NOT implement original setup, turn admission, AI or a complete NES.
 * The caller provides original ROM bytes transiently; game state is not edited. */
#define GK327_ROM_BYTES 262160u
#define GK327_PRG_BANK15_START (16u+15u*16384u)
#define GK327_RNG_PC 0xC289u
#define GK327_RNG_MAX_STEPS 100000u

typedef enum {
 GK327_OK=0, GK327_BAD_ARG=1, GK327_BAD_ROM=2,
 GK327_BAD_PC=3, GK327_UNSUPPORTED=4, GK327_BAD_WRITE=5,
 GK327_STEP_LIMIT=6, GK327_BAD_SOURCE=7
} Gk327Status;
typedef struct {
 uint8_t seed_before[6],seed_after[6];
 uint8_t result_66,result_67;
 uint16_t final_pc;
 uint32_t instructions;
 uint8_t authentic_rom;
 uint8_t first_spring_generated; /* always 0 */
 uint8_t player_orders_granted;  /* always 0 */
} Gk327RngReceipt;
/* Public test-only route. Never label its output as authentic NES. The caller
 * supplies a self-authored synthetic ROM and a fixed-bank PC. All execution
 * is inside bank15, all state writes are restricted to zero page. */
Gk327Status gk327_execute_fixture(const uint8_t *image,size_t n,
 uint16_t entry_pc,const uint8_t seed[6],uint32_t step_limit,
 Gk327RngReceipt *out);
/* Production source route: exact private original-USA image SHA-256, original
 * $C289 entry PC, and exact terminal RTS; otherwise reject atomically. */
Gk327Status gk327_execute_original_rng(const uint8_t *image,size_t n,
 const uint8_t seed[6],Gk327RngReceipt *out);
#endif
