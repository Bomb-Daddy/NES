#include "gk126_io_barrier.h"
Gk126IoBarrier gk126_io_write_barrier(uint16_t address,uint8_t value,
                                     int ppu_timing_enabled){
 /* In explicitly source-timed PPU mode, OAMDMA uses a read from an entire CPU page and introduces 513 or 514
  * CPU cycles of stalled execution. Ignoring the write contaminates all
  * subsequent PPU cycle comparisons. Historical hand-scripted NMI traces
  * intentionally have no simulated PPU clock and remain isolated hypotheticals. */
 if(ppu_timing_enabled && address==0x4014u)return GK126_IO_OAMDMA_UNMODELLED;
 /* $4020-$5FFF is cartridge expansion space. The stock mapper study has
  * not established any side effects there. Never silently alias to RAM. */
 if(ppu_timing_enabled && address>=0x4020u && address<0x6000u)return GK126_IO_EXPANSION_UNMODELLED;
 /* All $2000-$3FFF registers mirror every eight addresses. The preexisting
  * GK103 model intentionally refuses rendering, including its odd-frame
  * skip and sprite timing. Status-barrier semantics are independent of the
  * caller-supplied reset phase and never prescribe a fictional frame. */
 if(ppu_timing_enabled && address>=0x2000u && address<0x4000u &&
    (address&7u)==1u && (value&0x18u))
  return GK126_IO_PPU_RENDER_UNMODELLED;
 return GK126_IO_KNOWN_OR_DEFERRED;
}
