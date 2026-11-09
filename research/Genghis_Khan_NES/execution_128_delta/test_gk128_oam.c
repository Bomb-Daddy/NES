#include "gk128_oam.h"
#include "gk83_reset_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(p) do {if(!(p)){fprintf(stderr,"GK128 FAIL line %d: %s\n",__LINE__,#p);return 1;}}while(0)
int main(void){
 Gk128Oam o={0},before;uint8_t data[256],known[256];
 uint16_t cycles=900u;
 for(unsigned i=0;i<256;i++){data[i]=(uint8_t)(i^0xa5u);known[i]=1u;}
 before=o;CHECK(gk128_write_oam_data(&o,9)==GK128_UNKNOWN_ADDRESS);
 CHECK(gk128_oam_dma(&o,data,known,0x02u,1u,0u,&cycles)==GK128_UNKNOWN_ADDRESS);
 CHECK(memcmp(&o,&before,sizeof o)==0&&cycles==900);
 CHECK(gk128_set_oam_address(&o,251u)==GK128_OK);
 before=o;
 CHECK(gk128_oam_dma(&o,data,known,2u,0u,0u,&cycles)==GK128_UNKNOWN_ALIGNMENT);
 CHECK(memcmp(&o,&before,sizeof o)==0&&cycles==900);
 CHECK(gk128_oam_dma(&o,data,known,2u,1u,2u,&cycles)==GK128_BAD);
 CHECK(memcmp(&o,&before,sizeof o)==0&&cycles==900);
 known[17]=0u;
 CHECK(gk128_oam_dma(&o,data,known,2u,1u,0u,&cycles)==GK128_UNKNOWN_SOURCE);
 CHECK(memcmp(&o,&before,sizeof o)==0&&cycles==900);
 known[17]=1u;
 CHECK(gk128_oam_dma(&o,data,known,2u,1u,0u,&cycles)==GK128_OK);
 CHECK(cycles==513u&&o.dma_count==1u&&o.dma_stall_cycles==513u&&o.address==251u);
 for(unsigned i=0;i<256;i++){
  unsigned dest=(unsigned)(uint8_t)(251u+(uint8_t)i);
  CHECK(o.known[dest]==1u&&o.data[dest]==data[i]);
 }
 CHECK(gk128_oam_dma(&o,data,known,3u,1u,1u,&cycles)==GK128_OK);
 CHECK(cycles==514u&&o.dma_count==2u&&o.dma_stall_cycles==1027u);
 CHECK(o.last_dma_page==3u&&o.address==251u);
 CHECK(gk128_write_oam_data(&o,0x89u)==GK128_OK);
 CHECK(o.data[251]==0x89u&&o.address==252u&&o.direct_writes==1u);
 /* Probe exact private stock ROM only as OPTIONAL, without mistaking
  * original Mesen run from a source-only CPU trace. */
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk83Trace t={0};
  CHECK(gk83_trace_original_reset(rom,20000u,&t)==GK83_UNKNOWN_RAM_GATE);
  CHECK(t.instructions==70u&&t.stop_pc==0x8060u&&t.first_unknown_ram_read==0xf5u);
  CHECK(t.oam_direct_writes==0u&&t.oam_dma_transfers==0u);
 }
 puts("GK128 PASS: explicit 513/514-phase OAM DMA with 256-byte wraps, fail-closed unknown APU phase/source/OAM address, atomic rollback; no original gameplay claim");return 0;
}
