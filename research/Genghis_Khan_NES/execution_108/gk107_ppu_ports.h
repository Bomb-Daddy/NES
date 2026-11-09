#ifndef GK107_PPU_PORTS_H
#define GK107_PPU_PORTS_H
#include "gk106_ppu_bus.h"
#include <stdint.h>
/* CPU PPU register addresses repeat every eight bytes through $3FFF.
 * Only $2000/$2001/$2003/$2005/$2006 are write-only registers whose CPU
 * reads reproduce the PPU I/O latch. $2002, $2004, $2007 are NOT equivalent.
 * The original PPU latch decays analogically. This executable source tracer
 * accepts only a deliberately conservative <=3000 CPU-cycle interval since
 * the LAST full bus write; it refuses older values instead of freezing them.
 * This cannot establish precise hardware decay or Mesen state. */
#define GK107_PPU_BUS_MAX_FRESH_CYCLES 3000ULL
typedef enum { GK107_PORT_INVALID=0, GK107_PORT_UNKNOWN=1,
               GK107_PORT_STALE=2, GK107_PORT_OK=3 } Gk107PpuPortResult;
Gk107PpuPortResult gk107_ppu_read_write_only(const Gk106PpuBus *bus,
 uint16_t address,uint64_t cpu_cycle,uint64_t last_full_drive_cycle,
 uint8_t *out);
#endif
