#include "gk150_render_window.h"
#include "gk82_stock_cartridge.h"
#include <string.h>
Gk150Status gk150_scan_render_window(const char *rom,Gk150Report *out){
 if(!rom||!*rom||!out)return GK150_INVALID;
 Gk82RomBootstrap boot;
 if(gk82_probe_stock_rom(rom,&boot)!=GK82_ROM_OK)return GK150_SOURCE;
 Gk150Report next={0};
 for(unsigned origin=0u;origin<GK150_ORIGINS;origin++){
  Gk83Trace trace={0};
  Gk83Stop stop=gk150_trace_original_ppu_render_window(
      rom,origin,0u,120000u,&trace);
  if(stop==GK83_SOURCE_REFUSED)return GK150_SOURCE;
  if(stop==GK83_INVALID)return GK150_INVALID;
  next.origins_checked++;
  if(trace.experimental_render_source_pc){
   next.render_accepted++;
   if(!next.first_render_pc){
    next.first_render_pc=trace.experimental_render_source_pc;
    next.first_render_mask=trace.experimental_render_mask;
    next.first_render_cycle=trace.experimental_render_source_cycles;
    next.first_render_limit_dot=trace.experimental_render_window_limit;
    next.first_render_origin=(uint8_t)origin;
    next.first_after_render_stop_pc=trace.stop_pc;
    next.first_after_render_stop_cycles=trace.completed_cpu_cycles;
   }
  }else if(trace.ppu_scheduled_race_gate && trace.stop_pc==0xc5beu){
   next.legacy_race_gates++;
  }else next.other_gates++;
  if(stop==GK83_HARDWARE_GATE)next.hardware_gates++;
  else if(stop==GK83_UNKNOWN_RAM_GATE)next.unknown_ram_gates++;
  else if(stop==GK83_OPCODE_GATE)next.opcode_gates++;
  else if(stop==GK83_BUDGET_GATE)next.budget_gates++;
 }
 next.reached_spring=0u;
 *out=next;
 return GK150_BOUNDED;
}
