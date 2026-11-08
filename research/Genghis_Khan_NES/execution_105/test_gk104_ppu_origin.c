#include "gk103_ntsc_ppu.h"
#include "gk104_ppu_origin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MUST(x) do{if(!(x)){fprintf(stderr,"GK104 FAIL at %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk103Ppu s={0},legacy={0};uint8_t value=0;
 MUST(!gk104_ppu_reset_origin(NULL,0));
 MUST(!gk104_ppu_reset_origin(&s,GK103_FRAME_DOTS));
 MUST(!gk104_ppu_reset_origin(&s,GK103_VBLANK_START));
 MUST(!gk104_ppu_reset_origin(&s,GK103_VBLANK_END-1u));
 MUST(gk104_ppu_reset_origin(&s,GK103_VBLANK_END));
 MUST(s.last_dot==GK103_VBLANK_END);
 MUST(gk104_ppu_reset_origin(&s,89341u));
 MUST(gk103_ppu_advance(&s,1u));
 MUST(s.last_dot==89344u&&s.frames==1u);
 MUST(gk104_ppu_reset_origin(&s,82179u));
 MUST(!gk103_ppu_status(&s,1u,0u,&value));
 MUST(s.race_gate==1u);
 /* Same CPU-cycle count can yield different vblank events depending on
  * full reset-origin hypothesis: the previous 0..2 domain cannot express this. */
 MUST(gk104_ppu_reset_origin(&s,0));
 MUST(gk103_ppu_advance(&s,1000u));
 MUST(s.vblank_events==0u);
 MUST(gk104_ppu_reset_origin(&s,80000u));
 MUST(gk103_ppu_advance(&s,1000u));
 MUST(s.vblank_events==1u&&s.vblank==1u);
 /* Legacy phase 0/1/2 remains exactly equivalent to full origins 0/1/2. */
 for(uint8_t phase=0;phase<3u;phase++){
  MUST(gk103_ppu_reset(&legacy,phase));
  MUST(gk104_ppu_reset_origin(&s,phase));
  MUST(gk103_ppu_advance(&legacy,30000u));
  MUST(gk103_ppu_advance(&s,30000u));
  MUST(legacy.last_dot==s.last_dot&&legacy.vblank_events==s.vblank_events);
 }
 Gk83Trace out,saved;memset(&out,0x7b,sizeof out);saved=out;
 MUST(gk104_trace_original_ppu_origin(NULL,0,100,&out)==GK83_INVALID);
 MUST(gk104_trace_original_ppu_origin("/notfound",89342u,100,&out)==GK83_INVALID);
 MUST(gk104_trace_original_ppu_origin("/notfound",GK103_VBLANK_START,100,&out)==GK83_INVALID);
 MUST(gk104_trace_original_ppu_origin("/notfound",0,100,&out)==GK83_SOURCE_REFUSED);
 MUST(memcmp(&saved,&out,sizeof out)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  uint32_t origins[]={0u,2u,10240u,80000u,89341u};
  for(size_t i=0;i<sizeof(origins)/sizeof(origins[0]);i++){
   Gk83Trace a={0},b={0};
   Gk83Stop ra=gk104_trace_original_ppu_origin(rom,origins[i],30000u,&a);
   Gk83Stop rb=gk104_trace_original_ppu_origin(rom,origins[i],30000u,&b);
   MUST(ra==rb&&ra!=GK83_INVALID&&ra!=GK83_SOURCE_REFUSED);
   MUST(memcmp(&a,&b,sizeof a)==0);
   MUST(a.ppu_scheduled_frame_origin_dots==origins[i]);
   printf("GK104 source-only origin=%u status=%d pc=%04x instructions=%u vblank=%llu\n",
    origins[i],ra,a.stop_pc,a.instructions,(unsigned long long)a.ppu_scheduled_vblank);
  }
 }else puts("GK104 full-frame hypotheses portable tests PASS; private ROM not present");
 return 0;
}
