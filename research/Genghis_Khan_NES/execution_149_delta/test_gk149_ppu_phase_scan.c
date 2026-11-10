#include "gk149_ppu_phase_scan.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"GK149 failed line %d: %s\n",__LINE__,#x);return 1;}} while(0)
int main(void){
 Gk149PhaseScan out,unchanged;
 memset(&out,0x5a,sizeof out);unchanged=out;
 CHECK(gk149_scan_original_reset_phases(NULL,&out)==GK149_INVALID);
 CHECK(gk149_scan_original_reset_phases("",&out)==GK149_INVALID);
 CHECK(gk149_scan_original_reset_phases("/nonexistent/private-stock-rom.nes",&out)==GK149_SOURCE_REFUSED);
 CHECK(memcmp(&out,&unchanged,sizeof out)==0);
 Gk1App a;gk1_app_init(&a);
 CHECK(gk1_app_source_ppu_phase_scan(&a,"/nonexistent/private-stock-rom.nes",&out)==GK149_INVALID);
 CHECK(memcmp(&out,&unchanged,sizeof out)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  CHECK(gk149_scan_original_reset_phases(rom,&out)==GK149_BOUNDED);
  CHECK(out.origins_checked==35u&&out.ppu_race_stops==17u&&
        out.render_enable_stops==18u&&out.other_stops==0u);
  CHECK(out.first_race_pc==0xc5beu && out.first_render_origin==2u);
  CHECK(out.first_render_pc==0xc5e9u && out.first_render_mask==0x1eu);
  CHECK(out.first_render_cycles==57192u);
  CHECK(out.vblank_set_reads>=18u&&out.ppu_status_reads>200000u);
  CHECK(out.spring_created==0u);
  Gk149PhaseScan r=out;
  CHECK(gk1_app_source_ppu_phase_scan(&a,rom,&r)==GK149_INVALID);
  CHECK(!memcmp(&r,&out,sizeof r));
  a.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;a.setup_difficulty[0]=3;
  Gk1App original=a;
  CHECK(gk1_app_source_ppu_phase_scan(&a,rom,&r)==GK149_BOUNDED);
  CHECK(memcmp(&r,&out,sizeof r)==0&&memcmp(&a,&original,sizeof a)==0);
  puts("GK149 VERIFIED_STATIC original source: 35 phase hypotheses, 17 PPUSTATUS race gates, 18 PPUMASK rendering gates at $C5E9 value $1E; NO authentic Mesen run or first Spring");
 }else puts("GK149 public source/argument guards PASS; authenticated ROM positive tests NOT RUN");
 return 0;
}
