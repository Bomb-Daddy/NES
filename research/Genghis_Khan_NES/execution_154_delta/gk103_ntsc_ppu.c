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
/* EXEC0136: constant-time, integer-exact NTSC event accounting. An older
 * per-frame loop could take billions of iterations for a caller-supplied
 * long timeline advance. This model remains conditional on the explicitly
 * selected CPU/PPU phase; it does not infer actual Mesen power-on timing. */
static int edge_count(uint64_t from,uint64_t to,uint64_t offset,
                      uint64_t *count,uint64_t *last){
 if(!count||!last||from>to)return 0;
 *count=0u;*last=0u;
 if(to<offset)return 1;
 /* Count edges at offset + k * FRAME_DOTS in the open/closed interval
  * (from,to]. Arithmetic never evaluates a potentially overflowing edge
  * beyond to and is safe even with to near UINT64_MAX. */
 uint64_t through_to=(to-offset)/GK103_FRAME_DOTS + 1u;
 uint64_t through_from=from<offset?0u:(from-offset)/GK103_FRAME_DOTS + 1u;
 *count=through_to-through_from;
 if(*count)*last=offset+((to-offset)/GK103_FRAME_DOTS)*GK103_FRAME_DOTS;
 return 1;
}
int gk103_ppu_advance(Gk103Ppu *p,uint64_t cycle){
 uint64_t to;
 if(!p||p->rendering_unmodeled||!at_dot(cycle,p->frame_origin_dots,&to)||to<p->last_dot)return 0;
 uint64_t from=p->last_dot;
 /* GK151 strictly optional EVEN rendered-frame hypothesis. A complete
  * renderer and odd-frame skip are NOT modeled. We may cross ONE pre-render
  * boundary only if the caller separately assumed even parity; before
  * the next VBlank/possible NMI timing we must stop again. Any rendered
  * PPUSTATUS read is still refused. Legacy GK150 gate stays unchanged. */
 Gk103Ppu next=*p;
 if(next.render_window_active && to>=next.render_window_limit_dot){
  uint64_t end=next.render_window_limit_dot;
  uint64_t frame=end/GK103_FRAME_DOTS;
  if(!next.even_prerender_hypothesis || next.even_prerender_crossings ||
     frame>=UINT64_MAX/GK103_FRAME_DOTS-1u){
   p->render_window_gate=1u;return 0;
  }
  uint64_t boundary=(frame+1u)*GK103_FRAME_DOTS;
  if(boundary>UINT64_MAX-GK103_VBLANK_START ||
     to>=boundary+GK103_VBLANK_START-4u){
   p->render_window_gate=1u;return 0;
  }
  next.render_window_limit_dot=boundary+GK103_VBLANK_START-4u;
  next.even_prerender_crossings=1u;
 }
 if(to==from){*p=next;return 1;}
 uint64_t starts=0,ends=0,last_start=0,last_end=0;
 if(!edge_count(from,to,GK103_VBLANK_START,&starts,&last_start)||
    !edge_count(from,to,GK103_VBLANK_END,&ends,&last_end)||
    UINT64_MAX-p->vblank_events<starts||
    (p->nmi_enable&&UINT64_MAX-p->nmi_edges<starts))return 0;
 /* Commit only after every check passes. A read inside an existing VBlank
  * that cleared PPUSTATUS must remain cleared until the NEXT start event;
  * blindly deriving the bit from to % frame would be incorrect. */
 next.vblank_events+=starts;
 if(next.nmi_enable&&starts){
  next.nmi_pending=1u;
  next.nmi_edges+=starts;
 }
 if(starts||ends){
  next.vblank=(uint8_t)(starts&&(!ends||last_start>last_end));
 }
 next.last_dot=to;
 next.frames=to/GK103_FRAME_DOTS;
 *p=next;
 return 1;
}
/* EXEC0135: a rejected PPUSTATUS read cannot advance the event clock,
 * clear vblank, queue NMI, or partially consume a CPU cycle. The race bit
 * is a diagnostic failure flag only. These timings remain HYPOTHESES,
 * not authentic Mesen power-on phase evidence. */
int gk103_ppu_status(Gk103Ppu *p,uint64_t cycle,uint8_t low,uint8_t *out){
 uint64_t dot;
 if(!p||!out||!at_dot(cycle,p->frame_origin_dots,&dot))return 0;
 /* The rendered sprite-zero-hit and sprite-overflow bits are unknown. */
 if(p->render_window_active){p->render_window_gate=1u;return 0;}
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
int gk154_ppu_status_race_branch(Gk103Ppu *p,uint64_t cycle,
 uint8_t low,uint8_t after_edge,uint8_t *out){
 uint64_t dot;
 if(!p||!out||after_edge>1u||p->render_window_active||
    !at_dot(cycle,p->frame_origin_dots,&dot))return 0;
 uint64_t in=dot%GK103_FRAME_DOTS;
 if(in<GK103_VBLANK_START-2u||in>GK103_VBLANK_START+2u)return 0;
 Gk103Ppu next=*p;
 if(!gk103_ppu_advance(&next,cycle))return 0;
 /* This is a deliberately selected branch, not evidence of the
  * physical read/NMI ordering at the dot-level edge. */
 next.vblank=0u;
 if(next.status_reads==UINT64_MAX)return 0;
 next.status_reads++;
 *out=(uint8_t)((low&0x7fu)|(after_edge?0x80u:0u));
 *p=next;
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
/* Hypothesis: before the next pre-render/odd-frame boundary, frame-clock
 * accounting is unchanged by toggling the background/sprite enable bits.
 * Sprites, pixels, pattern fetches, PPUSTATUS sprite flags and odd-frame
 * shortened frames are NOT modeled; the trace stops before these matter.
 * Repeated writes cannot extend the original refusal boundary. */
int gk150_ppu_mask_bounded_window(Gk103Ppu *p,uint64_t cycle,uint8_t v){
 if(!p)return 0;
 Gk103Ppu next=*p;
 if(!gk103_ppu_advance(&next,cycle))return 0;
 if(cycle<GK103_CTRL_WARMUP_CPU){*p=next;return 1;}
 if((v&0x18u)==0u){
  next.ppumask=v;
  next.render_window_active=0u;
  next.render_window_limit_dot=0u;
  *p=next;return 1;
 }
 if(!next.render_window_active){
  uint64_t frame=next.last_dot/GK103_FRAME_DOTS;
  if(frame>UINT64_MAX/GK103_FRAME_DOTS-1u)return 0;
  uint64_t next_boundary=(frame+1u)*GK103_FRAME_DOTS;
  /* Conservatively refuse at the last pre-render dot regardless of frame
   * parity; do not assert which parity the original Mesen reset used. */
  uint64_t first_unsafe=next_boundary-1u;
  if(next.last_dot>=first_unsafe)return 0;
  next.render_window_limit_dot=first_unsafe;
  next.render_window_active=1u;
 }
 next.ppumask=v;
 *p=next;
 return 1;
}
int gk151_ppu_assume_even_prerender(Gk103Ppu *p){
 if(!p||p->even_prerender_hypothesis||p->even_prerender_crossings||
    p->render_window_active||p->rendering_unmodeled)return 0;
 p->even_prerender_hypothesis=1u;
 return 1;
}
int gk103_ppu_take_nmi(Gk103Ppu *p){
 if(!p||!p->nmi_pending)return 0;
 p->nmi_pending=0;return 1;
}
