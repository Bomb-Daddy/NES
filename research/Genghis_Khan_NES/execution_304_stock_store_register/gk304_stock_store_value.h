#ifndef GK304_STOCK_STORE_VALUE_H
#define GK304_STOCK_STORE_VALUE_H
#include "gk302_stock_seed_timeline.h"
#include <stdint.h>
#include <stddef.h>
/* EXEC0304: register-level validation of an externally observed original NES
 * 6502 STORE history. Claimed CPU registers are NOT proof of emulator execution.
 * This strengthens, but never replaces, the authentic first-Spring ROM run.
 * No original ROM/record bytes are embedded in this source. */
#define GK304_MAX_STORES GK302_MAX_EVENTS
typedef struct {
 uint64_t cpu_cycle;
 uint16_t pc;
 uint8_t mapped_prg_bank, destination;
 uint8_t a,x,y;  /* CPU register state at the STORE instruction */
} Gk304Registers;
typedef enum {
 GK304_OK=0,GK304_ARGUMENT=1,GK304_STOCK_ROM=2,
 GK304_TIMELINE=3,GK304_REGISTER_COUNT=4,
 GK304_SOURCE_OPCODE=5,GK304_VALUE=6,GK304_STAGE=7
} Gk304Status;
typedef struct {
 Gk302SeedResult seed;
 uint16_t register_verified_writes;
 uint8_t sta_count_nonzero,stx_count_nonzero,sty_count_nonzero;
 uint8_t authentic_original_rom_verified;
 uint8_t claimed_register_consistency_only; /* ALWAYS 1 */
 uint8_t original_stock_emulator_executed;  /* ALWAYS 0 */
 uint8_t playable_spring_created;           /* ALWAYS 0 */
 uint8_t orders_granted;                    /* ALWAYS 0 */
} Gk304Report;
/* The complete timeline (including intervening NMIs and C289 draws) is
 * independently replayed under the exact original ROM. Exactly one sidecar
 * register witness per STORE must agree in clock, PC, PRG bank and target.
 * The ROM's actual 6502 STORE opcode determines which register was written.
 * All failures preserve *out and the caller's event/input arrays unchanged. */
Gk304Status gk304_verify_store_values(const char *private_original_rom,
 const uint8_t initial[6],const Gk302SeedEvent *events,size_t count,
 const Gk304Registers *registers,size_t register_count,Gk304Report *out);
#endif
