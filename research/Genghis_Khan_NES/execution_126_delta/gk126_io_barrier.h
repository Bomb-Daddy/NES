#ifndef GK126_IO_BARRIER_H
#define GK126_IO_BARRIER_H
#include <stdint.h>
/* A conservative, ROM-source interpreter bus-write classifier, NOT a PPU,
 * APU or stock Mesen emulator. A non-NONE result means that following the
 * source instruction without that peripheral's effects would be misleading.
 * In particular OAMDMA stalls the CPU for 513/514 cycles, while enabling
 * PPU rendering changes frame timing and is not modeled by GK103. */
typedef enum {
 GK126_IO_KNOWN_OR_DEFERRED=0,
 GK126_IO_OAMDMA_UNMODELLED=1,
 GK126_IO_EXPANSION_UNMODELLED=2,
 GK126_IO_PPU_RENDER_UNMODELLED=3
} Gk126IoBarrier;
Gk126IoBarrier gk126_io_write_barrier(uint16_t address,uint8_t value,
                                     int ppu_timing_enabled);
#endif
