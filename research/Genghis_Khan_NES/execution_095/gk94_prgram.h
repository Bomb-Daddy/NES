#ifndef GK94_PRGRAM_H
#define GK94_PRGRAM_H
#include <stdint.h>
#include <stddef.h>
/* Authentic NES CPU $6000-$7FFF SRAM, distinct from the 2KiB work RAM.
 * SaveRam physical 0x0DCD is CPU 0x6DCD, NEVER CPU WorkRam 0x0DCD.
 * No power-on or battery-backed values are assumed. Both maps default UNKNOWN.
 * This is a memory-addressing component, not cartridge initialization/RNG. */
#define GK94_PRGRAM_BYTES 8192u
typedef struct {
 uint8_t data[GK94_PRGRAM_BYTES];
 uint8_t known[GK94_PRGRAM_BYTES];
} Gk94PrgRam;
typedef enum { GK94_RAM_OK=0, GK94_RAM_UNKNOWN=1,
 GK94_RAM_INVALID=2 } Gk94RamResult;
void gk94_prgram_reset_unknown(Gk94PrgRam *ram);
Gk94RamResult gk94_prgram_read(const Gk94PrgRam *ram,uint16_t cpu_address,
                              uint8_t *value);
Gk94RamResult gk94_prgram_write(Gk94PrgRam *ram,uint16_t cpu_address,
                               uint8_t value);
/* Explicit external-SRAM source input: only trusted after caller verifies
 * source/hash/bank. Never seed from CPU WorkRam or infer from a screenshot. */
Gk94RamResult gk94_prgram_seed_physical(Gk94PrgRam *ram,
                              size_t physical_offset,
                              const uint8_t *bytes,size_t count);
#endif
