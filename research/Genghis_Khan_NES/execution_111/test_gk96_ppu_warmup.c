#include "gk96_ppu_warmup.h"
#include "gk91_reset_zero.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK96 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk96Warmup result,before;memset(&result,0x5c,sizeof result);before=result;
 CHECK(gk96_original_reset_warmup(NULL,&result)==GK96_INVALID);
 CHECK(gk96_original_reset_warmup("",&result)==GK96_INVALID);
 CHECK(gk96_original_reset_warmup("/no-private-original-rom.nes",&result)==GK96_SOURCE_REFUSED);
 CHECK(memcmp(&result,&before,sizeof result)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk83Trace baseline={0};
  CHECK(gk91_trace_reset_mesen_allzeros(rom,20000,&baseline)==GK83_HARDWARE_GATE);
  CHECK(baseline.instructions==142&&baseline.stop_pc==0xc5be);
  CHECK(gk96_original_reset_warmup(rom,&result)==GK96_NEEDS_NMI);
  CHECK(result.required_nmi==1&&result.scenarios_checked==2);
  CHECK(result.immediate_vblank.stop==GK83_BUDGET_GATE);
  CHECK(result.delayed_vblank.stop==GK83_BUDGET_GATE);
  CHECK(result.immediate_vblank.stop_pc==0xc460);
  CHECK(result.delayed_vblank.stop_pc==0xc460);
  CHECK(result.immediate_vblank.instructions==30000);
  CHECK(result.delayed_vblank.instructions==30000);
  CHECK(result.immediate_vblank.hypothetical_ppustatus_reads==3);
  CHECK(result.delayed_vblank.hypothetical_ppustatus_reads==6);
  Gk96Warmup replay={0};
  CHECK(gk96_original_reset_warmup(rom,&replay)==GK96_NEEDS_NMI);
  CHECK(memcmp(&replay,&result,sizeof result)==0);
  printf("GK96 SOURCE-DERIVED NMI WAIT: CPU=$%04X, immediate=%u delayed=%u PPU reads, %u steps each\n",
    result.immediate_vblank.stop_pc,
    result.immediate_vblank.hypothetical_ppustatus_reads,
    result.delayed_vblank.hypothetical_ppustatus_reads,
    result.delayed_vblank.instructions);
 }else puts("GK96 NO PRIVATE ROM: fail-closed negative controls PASS; source-real positive NOT RUN");
 return 0;
}
