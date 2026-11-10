#include "gk149_ppu_phase_scan.h"
#include "gk83_reset_trace.h"
#include "gk129_dma_source.h"
#include "gk82_stock_cartridge.h"
#include <string.h>
#include <limits.h>

Gk149Status gk149_scan_original_reset_phases(const char *rom,Gk149PhaseScan *out){
 if(!rom||!*rom||!out)return GK149_INVALID;
 Gk82RomBootstrap boot;
 if(gk82_probe_stock_rom(rom,&boot)!=GK82_ROM_OK)return GK149_SOURCE_REFUSED;
 Gk149PhaseScan next={0};
 for(unsigned origin=0;origin<GK149_PHASE_COUNT;origin++){
  Gk83Trace trace={0};
  /* Explicitly hypothetical reset-origin and APU DMA alignment; no actual
   * original Mesen PPU timing, pixel rendering or controller sequence. */
  Gk83Stop stop=gk129_trace_original_ppu_with_dma_alignment(
   rom,origin,1u,0u,120000u,&trace);
  if(stop==GK83_SOURCE_REFUSED)return GK149_SOURCE_REFUSED;
  if(stop==GK83_INVALID)return GK149_INVALID;
  next.origins_checked++;
  if(UINT32_MAX-next.vblank_set_reads<trace.ppu_status_vblank_values||
     UINT32_MAX-next.ppu_status_reads<trace.ppu_scheduled_status_reads)
   return GK149_UNEXPECTED_GATE;
  next.vblank_set_reads+=trace.ppu_status_vblank_values;
  next.ppu_status_reads+=trace.ppu_scheduled_status_reads;
  if(stop==GK83_HARDWARE_GATE && trace.ppu_scheduled_race_gate &&
     trace.stop_pc==0xc5beu){
   next.ppu_race_stops++;
   if(!next.first_race_pc)next.first_race_pc=trace.stop_pc;
  }else if(stop==GK83_HARDWARE_GATE &&
     trace.first_unmodelled_io_write==0x2001u &&
     (trace.first_unmodelled_io_value&0x18u) &&
     !trace.ppu_scheduled_race_gate){
   next.render_enable_stops++;
   if(!next.first_render_pc){
    next.first_render_origin=(uint8_t)origin;
    next.first_render_pc=trace.stop_pc;
    next.first_render_mask=trace.first_unmodelled_io_value;
    next.first_render_cycles=trace.completed_cpu_cycles;
   }
  }else next.other_stops++;
 }
 /* A complete result includes even unfamiliar new source gates so source
  * divergence is visible, but the result is never promoted to gameplay. */
 next.spring_created=0u;
 *out=next;
 return next.other_stops?GK149_UNEXPECTED_GATE:GK149_BOUNDED;
}
