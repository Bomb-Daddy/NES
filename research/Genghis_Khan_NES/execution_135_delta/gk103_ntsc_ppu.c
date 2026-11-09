#include "gk103_ntsc_ppu.h"
#include <limits.h>
#include <stddef.h>
#include <string.h>
static int at_dot(uint64_t cycle,uint32_t origin,uint64_t *dot){
 if(!dot||cycle>(UINT64_MAX-origin)/3u)return 0;
 *dot=cycle*3u+origin;return 1;
}
int gk104_ppu_reset_origin(Gk103Ppu *p,uint32_t origin){
 if(!p||origin>=GK103_FRAME_DOTS ||
    (origin>=GK103_VBLANK_START && origin<GK103_VBLANK_END))return 0;
 memset(p,0,sizeof *p);
 p->frame_origin_dots=origin;
 p->phase=(uint8_t)(origin%3u);
 p->last_dot=origin;
 return 1;
}
int gk103_ppu_reset(Gk103Ppu *p,uint8_t phase){
 if(phase>2u)return 0;
 return gk104_ppu_reset_origin(p,phase);
}
/* Exact unrendered NTSC event periods. Crossings are evaluated explicitly;
 * a single CPU instruction may cross the start of vblank, and must not skip
 * that edge even if it passes its entire interval (large test advances). */
int gk103_ppu_advance(Gk103Ppu *p,uint64_t cycle){
 uint64_t to;
 if(!p||p->rendering_unmodeled||!at_dot(cycle,p->frame_origin_dots,&to)||to<p->last_dot)return 0;
 uint64_t from=p->last_dot;
 if(to==from)return 1;
 uint64_t frame=from/GK103_FRAME_DOTS;
 while(frame<=to/GK103_FRAME_DOTS){
  if(frame>UINT64_MAX/GK103_FRAME_DOTS)return 0;
  uint64_t start=frame*GK103_FRAME_DOTS;
  uint64_t on=start+GK103_VBLANK_START;
  uint64_t off=start+GK103_VBLANK_END;
  if(from<on&&on<=to){
   p->vblank=1;p->vblank_events++;
   if(p->nmi_enable){p->nmi_pending=1;p->nmi_edges++;}
  }
  if(from<off&&off<=to){p->vblank=0;}
  if(frame==to/GK103_FRAME_DOTS)break;
  frame++;
 }
 p->last_dot=to;p->frames=to/GK103_FRAME_DOTS;
 return 1;
}
/* EXEC0135: a rejected PPUSTATUS read cannot advance the event clock,
 * clear vblank, queue NMI, or partially consume a CPU cycle. The race bit
 * is a diagnostic failure flag only. These timings remain HYPOTHESES,
 * not authentic Mesen power-on phase evidence. */
int gk103_ppu_status(Gk103Ppu *p,uint64_t cycle,uint8_t low,uint8_t *out){
 uint64_t dot;
 if(!p||!out||!at_dot(cycle,p->frame_origin_dots,&dot))return 0;
 Gk103Ppu candidate=*p;
 if(!gk103_ppu_advance(&candidate,cycle))return 0;
 uint64_t in=dot%GK103_FRAME_DOTS;
 if(in>=GK103_VBLANK_START-2u&&in<=GK103_VBLANK_START+2u){
  p->race_gate=1u;return 0;
 }
 uint8_t result=(uint8_t)((low&0x7fu)|(candidate.vblank?0x80u:0u));
 candidate.status_reads++;
 candidate.vblank=0;
 *p=candidate;
 *out=result;
 return 1;
}
int gk103_ppu_ctrl(Gk103Ppu *p,uint64_t cycle,uint8_t value){
 if(!p)return 0;
 Gk103Ppu candidate=*p;
 if(!gk103_ppu_advance(&candidate,cycle))return 0;
 if(cycle<GK103_CTRL_WARMUP_CPU){
  candidate.warmup_ignored_ctrl=1;*p=candidate;return 1;
 }
 uint8_t next=(uint8_t)((value>>7u)&1u);
 if(next&&!candidate.nmi_enable&&candidate.vblank){
  candidate.nmi_pending=1;candidate.nmi_edges++;
 }
 candidate.nmi_enable=next;
 *p=candidate;
 return 1;
}
int gk103_ppu_mask(Gk103Ppu *p,uint64_t cycle,uint8_t v){
 if(!p)return 0;
 Gk103Ppu candidate=*p;
 if(!gk103_ppu_advance(&candidate,cycle))return 0;
 if(cycle<GK103_CTRL_WARMUP_CPU){*p=candidate;return 1;}
 if(v&0x18u){
  /* Fail closed: preserve the last successful clock and event counts, while
   * keeping the existing terminal reason visible to source diagnostics. */
  p->rendering_unmodeled=1;return 0;
 }
 candidate.ppumask=v;
 *p=candidate;
 return 1;
}
int gk103_ppu_take_nmi(Gk103Ppu *p){
 if(!p||!p->nmi_pending)return 0;
 p->nmi_pending=0;return 1;
}
