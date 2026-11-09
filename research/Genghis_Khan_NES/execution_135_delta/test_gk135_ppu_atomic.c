#include "gk103_ntsc_ppu.h"
#include "gk103_ppu_schedule.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned assertions;
#define CHECK(expr) do{assertions++;if(!(expr)){fprintf(stderr,"GK135 FAIL line %d: %s\\n",__LINE__,#expr);return 1;}}while(0)
/* Compare all behaviorally material fields, excluding documented terminal
 * reason flags. No diagnostic failure can commit a PPU event. */
static int unchanged_except_gate(const Gk103Ppu *after,const Gk103Ppu *before){
 Gk103Ppu a=*after,b=*before;
 a.race_gate=b.race_gate=0;
 a.rendering_unmodeled=b.rendering_unmodeled=0;
 return memcmp(&a,&b,sizeof a)==0;
}
int main(void){
 Gk103Ppu p,original,repeat;
 uint8_t value;
 for(uint32_t offset=0;offset<GK103_FRAME_DOTS;offset+=997u){
  if(offset>=GK103_VBLANK_START&&offset<GK103_VBLANK_END)continue;
  CHECK(gk104_ppu_reset_origin(&p,offset));
  original=p;
  value=0xa5u;
  /* Every backward clock and extreme overflow must leave state untouched. */
  CHECK(!gk103_ppu_advance(&p,UINT64_MAX));
  CHECK(memcmp(&p,&original,sizeof p)==0);
  CHECK(!gk103_ppu_status(&p,UINT64_MAX,0xffu,&value));
  CHECK(value==0xa5u&&memcmp(&p,&original,sizeof p)==0);
 }
 for(int delta=-2;delta<=2;delta++){
  /* Source-independent phase crossing: force exact race at the first vblank
   * of a cycle-sized origin without presuming original Mesen master phase. */
  uint32_t origin=(uint32_t)(GK103_VBLANK_START+delta);
  if(origin>=GK103_VBLANK_START&&origin<GK103_VBLANK_END){
   /* Move to the same race by starting three dots earlier on prior CPU step. */
   origin=(uint32_t)(GK103_VBLANK_START+delta-6);
   CHECK(gk104_ppu_reset_origin(&p,origin));
   original=p;value=0x5au;
   CHECK(!gk103_ppu_status(&p,2u,0u,&value));
   CHECK(p.race_gate==1u&&value==0x5au);
   CHECK(unchanged_except_gate(&p,&original));
  }
 }
 CHECK(gk103_ppu_reset(&p,0u));
 CHECK(gk103_ppu_advance(&p,27393u)); /* last_dot=82179 */
 original=p;
 value=0x5a;
 CHECK(!gk103_ppu_status(&p,27394u,0x15u,&value));
 CHECK(value==0x5a&&p.race_gate&&unchanged_except_gate(&p,&original));
 CHECK(gk103_ppu_status(&p,27395u,0x15u,&value));
 CHECK(value==0x95u&&p.status_reads==1u&&!p.vblank);
 CHECK(gk103_ppu_reset(&p,0));
 CHECK(gk103_ppu_advance(&p,27393u));
 original=p;
 CHECK(!gk103_ppu_mask(&p,30001u,0x18u));
 CHECK(p.rendering_unmodeled==1&&unchanged_except_gate(&p,&original));
 CHECK(!gk103_ppu_advance(&p,30002u));
 CHECK(gk103_ppu_reset(&p,0));
 CHECK(gk103_ppu_advance(&p,30000u));
 CHECK(gk103_ppu_ctrl(&p,30001u,0x80u));
 original=p;
 CHECK(!gk103_ppu_ctrl(&p,30000u,0));
 CHECK(memcmp(&p,&original,sizeof p)==0);
 CHECK(!gk103_ppu_mask(&p,30000u,0));
 CHECK(memcmp(&p,&original,sizeof p)==0);
 repeat=p;
 CHECK(gk103_ppu_advance(&repeat,58000u));
 CHECK(gk103_ppu_advance(&p,58000u));
 CHECK(memcmp(&p,&repeat,sizeof p)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk83Trace a={0},b={0};
  Gk83Stop x=gk103_trace_original_ppu_schedule(rom,0,100000,&a);
  Gk83Stop y=gk103_trace_original_ppu_schedule(rom,0,100000,&b);
  CHECK(x!=GK83_INVALID&&x!=GK83_SOURCE_REFUSED&&x==y);
  CHECK(memcmp(&a,&b,sizeof a)==0);
  CHECK(a.instructions>=142u);
  fprintf(stdout,"GK135 private original-source PPU hypothesis: stop=%d pc=%04x instructions=%u; NOT native Mesen replay\\n",x,a.stop_pc,a.instructions);
 }else puts("GK135 private original-stock source test NOT RUN (ROM absent)");
 printf("GK135 PPU atomic refusal and replay PASS: %u checks\\n",assertions);
 return 0;
}
