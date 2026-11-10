#include "gk151_even_source.h"
#include "gk82_stock_cartridge.h"
Gk151Status gk151_scan_even_source(const char *rom,Gk151Report *out){
 if(!rom||!*rom||!out)return GK151_INVALID;
 Gk82RomBootstrap boot;
 if(gk82_probe_stock_rom(rom,&boot)!=GK82_ROM_OK)return GK151_SOURCE;
 Gk151Report result={0};
 for(unsigned i=0;i<GK151_ORIGINS;i++){
  Gk83Trace old={0},fresh={0};
  Gk83Stop a=gk150_trace_original_ppu_render_window(rom,i,0u,120000u,&old);
  Gk83Stop b=gk151_trace_original_even_prerender(rom,i,0u,120000u,&fresh);
  if(a==GK83_SOURCE_REFUSED||b==GK83_SOURCE_REFUSED)return GK151_SOURCE;
  if(a==GK83_INVALID||b==GK83_INVALID)return GK151_INVALID;
  result.origins++;
  if(fresh.experimental_even_prerender_crossings){
   result.even_crossed++;
   if(!result.first_cross_stop_pc){
    result.first_cross_origin=(uint8_t)i;
    result.first_cross_stop_pc=fresh.stop_pc;
    result.first_cross_stop_cycles=fresh.completed_cpu_cycles;
   }
   if(fresh.completed_cpu_cycles>old.completed_cpu_cycles)
    result.source_advanced_after_legacy=1u;
  }else if(old.experimental_render_window_gate)result.legacy_render_gated++;
  else if(old.ppu_scheduled_race_gate)result.ppu_race_gated++;
  else result.other_gated++;
 }
 result.generated_spring=0u;
 *out=result;
 return GK151_BOUNDED;
}
