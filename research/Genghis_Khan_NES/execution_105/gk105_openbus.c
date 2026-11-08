#include "gk105_openbus.h"
#include "gk103_ntsc_ppu.h"
#include <string.h>
static int control_diff(const Gk83Trace *a,const Gk83Trace *b){
 return a->stop!=b->stop||a->stop_pc!=b->stop_pc||
  a->instructions!=b->instructions||a->completed_cpu_cycles!=b->completed_cpu_cycles||
  a->mapper_serial_writes!=b->mapper_serial_writes||
  a->ppuctrl_write_attempts!=b->ppuctrl_write_attempts||
  a->ppu_scheduled_status_reads!=b->ppu_scheduled_status_reads;
}
static int registers_diff(const Gk83Trace *a,const Gk83Trace *b){
 return a->a!=b->a||a->x!=b->x||a->y!=b->y||a->sp!=b->sp||a->flags!=b->flags;
}
Gk83Stop gk105_scan_lower7(const char *rom,uint32_t origin,
 uint32_t max_steps,Gk105Summary *out){
 if(!rom||!*rom||!out||max_steps==0u||max_steps>1000000u||
    origin>=GK103_FRAME_DOTS||
    (origin>=GK103_VBLANK_START&&origin<GK103_VBLANK_END))return GK83_INVALID;
 Gk105Summary s={0};s.frame_origin_dots=origin;s.step_limit=max_steps;
 Gk83Trace trial={0};
 for(unsigned i=0;i<128;i++){
  Gk83Stop result=gk105_trace_original_low7(rom,origin,(uint8_t)i,max_steps,&trial);
  if(result==GK83_INVALID||result==GK83_SOURCE_REFUSED)return result;
  if(i==0u)s.baseline=trial;
  else{
   s.control_divergences+=(uint16_t)control_diff(&s.baseline,&trial);
   s.register_divergences+=(uint16_t)registers_diff(&s.baseline,&trial);
  }
  s.trials++;
 }
 *out=s;return s.baseline.stop;
}
