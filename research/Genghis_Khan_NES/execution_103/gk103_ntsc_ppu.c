#include "gk103_ntsc_ppu.h"
#include <limits.h>
#include <stddef.h>
#include <string.h>
static int at_dot(uint64_t cycle,uint8_t phase,uint64_t *dot){
 if(!dot||cycle>(UINT64_MAX-2u)/3u)return 0;
 *dot=cycle*3u+phase;return 1;
}
int gk103_ppu_reset(Gk103Ppu *p,uint8_t phase){
 if(!p||phase>2u)return 0;
 memset(p,0,sizeof *p);p->phase=phase;p->last_dot=phase;return 1;
}
/* Exact unrendered NTSC event periods. Crossings are evaluated explicitly;
 * a single CPU instruction may cross the start of vblank, and must not skip
 * that edge even if it passes its entire interval (large test advances). */
int gk103_ppu_advance(Gk103Ppu *p,uint64_t cycle){
 uint64_t to;
 if(!p||p->rendering_unmodeled||!at_dot(cycle,p->phase,&to)||to<p->last_dot)return 0;
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
int gk103_ppu_status(Gk103Ppu *p,uint64_t cycle,uint8_t low,uint8_t *out){
 uint64_t dot;
 if(!p||!out||!at_dot(cycle,p->phase,&dot)||!gk103_ppu_advance(p,cycle))return 0;
 /* Race at/before/after vblank set: the NMI may be suppressed depending
  * on master-clock phase; fail closed rather than fabricate a read. */
 uint64_t in=dot%GK103_FRAME_DOTS;
 if(in>=GK103_VBLANK_START-2u&&in<=GK103_VBLANK_START+2u){p->race_gate=1;return 0;}
 *out=(uint8_t)((low&0x7fu)|(p->vblank?0x80u:0u));
 p->status_reads++;p->vblank=0;
 return 1;
}
int gk103_ppu_ctrl(Gk103Ppu *p,uint64_t cycle,uint8_t value){
 if(!p||!gk103_ppu_advance(p,cycle))return 0;
 if(cycle<GK103_CTRL_WARMUP_CPU){p->warmup_ignored_ctrl=1;return 1;}
 uint8_t next=(uint8_t)((value>>7u)&1u);
 if(next&& !p->nmi_enable &&p->vblank){p->nmi_pending=1;p->nmi_edges++;}
 p->nmi_enable=next;
 return 1;
}
int gk103_ppu_mask(Gk103Ppu *p,uint64_t cycle,uint8_t v){
 if(!p||!gk103_ppu_advance(p,cycle))return 0;
 if(cycle<GK103_CTRL_WARMUP_CPU)return 1;
 p->ppumask=v;
 if(v&0x18u){p->rendering_unmodeled=1;return 0;}
 return 1;
}
int gk103_ppu_take_nmi(Gk103Ppu *p){
 if(!p||!p->nmi_pending)return 0;
 p->nmi_pending=0;return 1;
}
