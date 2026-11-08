#ifndef GK82_STOCK_CARTRIDGE_H
#define GK82_STOCK_CARTRIDGE_H
#include <stddef.h>
#include <stdint.h>
/* EXEC0082: ORIGINAL STOCK iNES source-authentication & CPU reset bootstrap
 * trace. This NEVER executes cartridge instructions or initializes gameplay.
 * The owner supplies exact original ROM privately at runtime. No licensed
 * PRG bytes are embedded in this source, installer, or public evidence.
 */
typedef enum {
 GK82_ROM_OK=0,GK82_ROM_INVALID=1,GK82_ROM_IO=2,
 GK82_ROM_LENGTH=3,GK82_ROM_SHA=4,GK82_ROM_HEADER=5,GK82_ROM_RESET=6
} Gk82RomStatus;
typedef struct {
 uint16_t nmi_vector,reset_vector,irq_vector;
 uint8_t mapper,prg_banks,chr_banks;
 uint8_t boot_instruction_count; /* only exact statically proven instructions */
 /* Explicitly no character attributes, RNG seed, campaign/season data here. */
} Gk82RomBootstrap;
/* Construct private path from same absolute user-data root as EXEC0081.
 * On invalid input, never change output. */
int gk82_rom_path(const char *root,char *output,size_t cap);
/* SHA256 authentic original user-owned USA iNES (262160B), MMC1 mapper1,
 * 16 PRG banks/0 CHR ROM banks. Proves source reset vector = CPU $C000 and
 * original five-instruction bootstrap (SEI/CLD/LDX #FD/TXS/ LDA #FF).
 * This is static PRG source tracing, NOT emulator execution. Atomic failure. */
Gk82RomStatus gk82_probe_stock_rom(const char *filename,Gk82RomBootstrap *out);
#endif
