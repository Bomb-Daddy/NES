#include "gk103_ntsc_ppu.h"
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
static uint64_t checks;
#define CHECK(q) do { checks++; if(!(q)) { fprintf(stderr,"GK136 FAIL %s:%d: %s\n",__FILE__,__LINE__,#q);return 1;} }while(0)
/* A deliberately slow, independent per-dot-frame reference oracle for a
 * bounded range, retaining interrupted vblank state after a PPUSTATUS read. */
static int slow(Gk103Ppu *p,uint64_t cycle){
 if(!p||cycle>(UINT64_MAX-p->frame_origin_dots)/3u)return 0;
 const uint64_t target=cycle*3u+p->frame_origin_dots;
 if(target<p->last_dot)return 0;
 const uint64_t from=p->last_dot;
 uint64_t frame=from/GK103_FRAME_DOTS;
 while(frame<=target/GK103_FRAME_DOTS){
  uint64_t on=frame*GK103_FRAME_DOTS+GK103_VBLANK_START;
  uint64_t off=frame*GK103_FRAME_DOTS+GK103_VBLANK_END;
  if(from<on&&on<=target){p->vblank=1;p->vblank_events++;
    if(p->nmi_enable){p->nmi_pending=1;p->nmi_edges++;}}
  if(from<off&&off<=target)p->vblank=0;
  if(frame==target/GK103_FRAME_DOTS)break;
  frame++;
 }
 p->last_dot=target;p->frames=target/GK103_FRAME_DOTS;
 return 1;
}
static uint64_t next_rand(uint64_t *s){
 *s^=*s<<13;*s^=*s>>7;*s^=*s<<17;return *s;
}
int main(void){
 Gk103Ppu fast,reference,original;
 uint64_t rng=UINT64_C(0x43505a5ac001d00d);
 for(unsigned origin=0;origin< GK103_FRAME_DOTS;origin+=2297){
  if(origin>=GK103_VBLANK_START&&origin<GK103_VBLANK_END)continue;
  CHECK(gk104_ppu_reset_origin(&fast,origin));
  reference=fast;
  for(unsigned j=0;j<1000;j++){
   uint64_t bound=GK103_FRAME_DOTS*8u/3u;
   uint64_t delta=next_rand(&rng)%bound;
   uint64_t at=fast.last_dot/3u+delta/3u+1u;
   if(at>5000000u)break;
   fast.nmi_enable=reference.nmi_enable=(uint8_t)(next_rand(&rng)&1u);
   CHECK(gk103_ppu_advance(&fast,at));
   CHECK(slow(&reference,at));
   CHECK(memcmp(&fast,&reference,sizeof fast)==0);
   if((j%37u)==0u){
    /* A PPUSTATUS read clears VBlank inside the same frame; this must not
     * spontaneously reassert on the next advance, unless start is crossed. */
    fast.vblank=reference.vblank=0;
   }
  }
 }
 CHECK(gk103_ppu_reset(&fast,0));
 fast.nmi_enable=1;
 uint64_t very_large_cycle=UINT64_C(12000000000000);
 CHECK(gk103_ppu_advance(&fast,very_large_cycle));
 CHECK(fast.last_dot==very_large_cycle*3u);
 CHECK(fast.frames==fast.last_dot/GK103_FRAME_DOTS);
 uint64_t expected_starts=(fast.last_dot-GK103_VBLANK_START)/GK103_FRAME_DOTS+1u;
 CHECK(fast.vblank_events==expected_starts&&fast.nmi_edges==expected_starts);
 CHECK(fast.nmi_pending==1u);
 original=fast;
 CHECK(gk103_ppu_advance(&fast,very_large_cycle));
 CHECK(!memcmp(&fast,&original,sizeof fast));
 CHECK(!gk103_ppu_advance(&fast,very_large_cycle-1u));
 CHECK(!memcmp(&fast,&original,sizeof fast));
 /* Explicitly test near the 64-bit cycle-to-dot boundary in constant time. */
 uint64_t max_cycle=(UINT64_MAX-0u)/3u;
 CHECK(gk103_ppu_advance(&fast,max_cycle));
 CHECK(fast.last_dot==max_cycle*3u);
 original=fast;
 CHECK(!gk103_ppu_advance(&fast,max_cycle+1u));
 CHECK(!memcmp(&fast,&original,sizeof fast));
 CHECK(gk103_ppu_reset(&fast,0));
 CHECK(gk103_ppu_advance(&fast,27393u));
 fast.vblank_events=UINT64_MAX;
 original=fast;
 CHECK(!gk103_ppu_advance(&fast,27396u));
 CHECK(!memcmp(&fast,&original,sizeof fast));
 CHECK(gk103_ppu_reset(&fast,0));
 fast.nmi_enable=1u;fast.nmi_edges=UINT64_MAX;
 original=fast;
 CHECK(!gk103_ppu_advance(&fast,30000u));
 CHECK(!memcmp(&fast,&original,sizeof fast));
 /* NMI disabled: saturating count is not incremented. */
 fast.nmi_enable=0u;
 CHECK(gk103_ppu_advance(&fast,30000u));
 CHECK(fast.nmi_edges==UINT64_MAX&&fast.vblank_events==1u);
 /* A status-read-cleared flag must remain cleared before next start. */
 CHECK(gk103_ppu_reset(&fast,0));
 CHECK(gk103_ppu_advance(&fast,27400u));
 uint8_t out=0;
 CHECK(gk103_ppu_status(&fast,27401u,0,&out));
 CHECK(out==0x80u);
 CHECK(fast.vblank==0);
 CHECK(gk103_ppu_advance(&fast,27500u));
 CHECK(fast.vblank==0);
 CHECK(gk103_ppu_advance(&fast,60000u));
 CHECK(fast.vblank==0);
 CHECK(gk103_ppu_advance(&fast, (uint64_t)(2u*GK103_FRAME_DOTS)/3u+27400u));
 CHECK(fast.vblank==1u);
 printf("GK136 O(1) PPU event oracle, long horizon and refusal: %" PRIu64 " checks PASS\n",checks);
 return 0;
}
