#include "gk151_even_source.h"
#include "gk103_ntsc_ppu.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"GK151 FAIL line %d: %s\n",__LINE__,#c);return 1;} }while(0)
int main(void){
 Gk103Ppu p={0},before={0};
 CHECK(!gk151_ppu_assume_even_prerender(NULL));
 CHECK(gk103_ppu_reset(&p,0u));
 CHECK(gk151_ppu_assume_even_prerender(&p));
 CHECK(!gk151_ppu_assume_even_prerender(&p));
 CHECK(gk103_ppu_advance(&p,30500u));
 CHECK(gk150_ppu_mask_bounded_window(&p,30501u,0x1eu));
 uint64_t boundary=p.render_window_limit_dot;
 CHECK(boundary>p.last_dot);
 uint64_t crossing=(boundary+4u)/3u;
 CHECK(gk103_ppu_advance(&p,crossing));
 CHECK(p.even_prerender_crossings==1u);
 CHECK(p.render_window_active==1u && p.ppumask==0x1eu);
 CHECK(p.render_window_limit_dot>boundary);
 CHECK(p.vblank_events==2u); /* no invented new VBlank */
 CHECK(gk103_ppu_advance(&p,crossing+2u));
 before=p;
 uint8_t dest=0xceu;
 CHECK(!gk103_ppu_status(&p,crossing+2u,0u,&dest));
 CHECK(dest==0xceu); /* rendered flags still unknown, no read permitted */
 p.render_window_gate=0u;
 CHECK(memcmp(&p,&before,sizeof p)==0);
 uint64_t new_deadline=p.render_window_limit_dot;
 CHECK(!gk103_ppu_advance(&p,(new_deadline+3u)/3u));
 CHECK(p.render_window_gate==1u);
 p.render_window_gate=0u;
 CHECK(memcmp(&p,&before,sizeof p)==0); /* deadline refusal atomic */
 /* Switching back to disabled rendering preserves legacy original gate. */
 CHECK(gk103_ppu_reset(&p,0u));
 CHECK(gk103_ppu_advance(&p,30500u));
 CHECK(gk150_ppu_mask_bounded_window(&p,30501u,0x1eu));
 before=p;
 CHECK(!gk103_ppu_advance(&p,crossing));
 CHECK(p.render_window_gate==1u);
 p.render_window_gate=0u;
 CHECK(memcmp(&p,&before,sizeof p)==0);
 CHECK(gk103_ppu_reset(&p,0u));
 CHECK(gk151_ppu_assume_even_prerender(&p));
 CHECK(gk103_ppu_advance(&p,30500u));
 CHECK(gk150_ppu_mask_bounded_window(&p,30501u,0x1eu));
 CHECK(gk103_ppu_advance(&p,crossing));
 CHECK(gk150_ppu_mask_bounded_window(&p,crossing+1u,0u));
 CHECK(!p.render_window_active&&p.ppumask==0u);
 Gk151Report out; memset(&out,0xa5,sizeof out);
 Gk151Report guard=out;
 CHECK(gk151_scan_even_source("/no/source",&out)==GK151_SOURCE);
 CHECK(gk151_scan_even_source(NULL,&out)==GK151_INVALID);
 CHECK(memcmp(&out,&guard,sizeof out)==0);
 Gk1App app;gk1_app_init(&app);
 CHECK(gk1_app_source_even_prerender(&app,"/no/source",&out)==GK151_INVALID);
 CHECK(memcmp(&out,&guard,sizeof out)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom && *rom){
   Gk83Trace old={0},fresh={0};
   CHECK(gk150_trace_original_ppu_render_window(rom,0u,0u,120000u,&old)!=GK83_SOURCE_REFUSED);
   CHECK(gk151_trace_original_even_prerender(rom,0u,0u,120000u,&fresh)!=GK83_SOURCE_REFUSED);
   CHECK(gk151_scan_even_source(rom,&out)==GK151_BOUNDED);
   CHECK(out.origins==GK151_ORIGINS && out.generated_spring==0u);
   CHECK(out.even_crossed>0u);
   app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
   Gk1App app_before=app;
   Gk151Report another=out;
   CHECK(gk1_app_source_even_prerender(&app,rom,&another)==GK151_BOUNDED);
   CHECK(memcmp(&app,&app_before,sizeof app)==0);
   CHECK(memcmp(&another,&out,sizeof out)==0);
   printf("GK151 original-stock source hypothesis origins=%u even_crossing=%u advanced=%u first stop PC=$%04X cycles=%llu (no Mesen; no game Spring)\n",
     out.origins,out.even_crossed,(unsigned)out.source_advanced_after_legacy,
     out.first_cross_stop_pc,(unsigned long long)out.first_cross_stop_cycles);
 }else puts("GK151 public PPU guard pass; original STOCK not run");
 return 0;
}
