#ifndef GK106_PPU_BUS_H
#define GK106_PPU_BUS_H
#include <stdint.h>
/* A bounded, copyright-clean PPU register open-bus latch. Before the first
 * CPU write, the latch is unknown; never guess power-on bytes. PPUSTATUS
 * exposes bits 0..4 from this latch and bits 5..7 from the actual PPU status.
 * The caller must establish sprite flags separately (see the NTSC PPU gate).
 * This isolated model is NOT a full PPU or a Mesen gameplay observation. */
typedef struct {
 uint8_t value, known;
 uint32_t cpu_writes, status_reads;
} Gk106PpuBus;
void gk106_ppu_bus_reset(Gk106PpuBus *bus);
/* Every PPU register CPU WRITE, including mirrored and ignored registers,
 * drives the internal PPU I/O bus. Call only for $2000..$3FFF writes. */
int gk106_ppu_bus_cpu_write(Gk106PpuBus *bus, uint8_t value);
/* Read low five bits if known. No output mutation when unknown. */
int gk106_ppu_bus_low5(const Gk106PpuBus *bus, uint8_t *low5);
/* Called ONLY after the independently modeled PPUSTATUS high bits have been
 * established. The status read refreshes ONLY the high three bus bits.
 * Failed calls leave bus and output unchanged. */
int gk106_ppu_bus_status_read(Gk106PpuBus *bus,uint8_t high3,uint8_t *value);
#endif
