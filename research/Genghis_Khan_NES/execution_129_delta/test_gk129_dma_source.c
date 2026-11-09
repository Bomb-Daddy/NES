#include "gk129_dma_source.h"
#include "gk83_reset_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK129 FAIL %u: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 uint8_t ram[2048],known[2048];Gk128Oam o={0},before;
 uint16_t stall=777u;
 for(unsigned i=0;i<2048;i++){ram[i]=(uint8_t)((i*37u+51u)&255u);known[i]=1u;}
 before=o;
 CHECK(gk129_dma_from_cpu_ram(&o,ram,known,0x00,1,0,&stall)==GK129_DMA_UNKNOWN_OAM);
 CHECK(memcmp(&o,&before,sizeof o)==0&&stall==777u);
 CHECK(gk128_set_oam_address(&o,251u)==GK128_OK);
 before=o;
 for(unsigned pg=0x20;pg<256;pg++){
  CHECK(gk129_dma_from_cpu_ram(&o,ram,known,(uint8_t)pg,1,0,&stall)==GK129_DMA_UNMODELLED_PAGE);
  CHECK(memcmp(&o,&before,sizeof o)==0&&stall==777u);
 }
 CHECK(gk129_dma_from_cpu_ram(&o,ram,known,0u,0,0,&stall)==GK129_DMA_UNKNOWN_PHASE);
 CHECK(memcmp(&o,&before,sizeof o)==0&&stall==777u);
 CHECK(gk129_dma_from_cpu_ram(&o,ram,known,0u,1,2,&stall)==GK129_DMA_BAD);
 CHECK(memcmp(&o,&before,sizeof o)==0&&stall==777u);
 for(unsigned pg=0;pg<32;pg++){
  unsigned at=(pg*256u+17u)&2047u;
  known[at]=0u;
  CHECK(gk129_dma_from_cpu_ram(&o,ram,known,(uint8_t)pg,1,0,&stall)==GK129_DMA_UNKNOWN_SOURCE);
  CHECK(memcmp(&o,&before,sizeof o)==0&&stall==777u);
  known[at]=1u;
 }
 for(unsigned pg=0;pg<32;pg++){
  for(unsigned phase=0;phase<2;phase++){
   CHECK(gk129_dma_from_cpu_ram(&o,ram,known,(uint8_t)pg,1,(uint8_t)phase,&stall)==GK129_DMA_OK);
   CHECK(stall==513u+phase&&o.address==251u&&o.last_dma_page==pg);
   for(unsigned i=0;i<256;i++){
    unsigned src=((pg*256u+i)&2047u);
    unsigned dest=(251u+i)&255u;
    CHECK(o.known[dest]==1u&&o.data[dest]==ram[src]);
   }
  }
 }
 CHECK(o.dma_count==64u&&o.dma_stall_cycles==64u*513u+32u);
 /* Source-hypothesis API arguments must not mutate an existing receipt
  * when phase or provenance is missing. */
 Gk83Trace rejected,unchanged;
 memset(&rejected,0x5a,sizeof rejected);unchanged=rejected;
 CHECK(gk129_trace_original_ppu_with_dma_alignment("none",0u,0u,0u,100u,&rejected)==GK83_INVALID);
 CHECK(memcmp(&rejected,&unchanged,sizeof rejected)==0);
 CHECK(gk129_trace_original_ppu_with_dma_alignment("none",0u,1u,2u,100u,&rejected)==GK83_INVALID);
 CHECK(memcmp(&rejected,&unchanged,sizeof rejected)==0);
 const char *stock=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(stock&&*stock){
  Gk83Trace t={0},p={0};
  CHECK(gk83_trace_original_reset(stock,20000u,&t)==GK83_UNKNOWN_RAM_GATE);
  CHECK(t.stop_pc==0x8060u&&t.first_unknown_ram_read==0xf5u&&t.instructions==70u);
  Gk83Stop s=gk129_trace_original_ppu_with_dma_alignment(stock,0u,1u,0u,150000u,&p);
  CHECK(s!=GK83_SOURCE_REFUSED&&s!=GK83_INVALID);
  CHECK(p.instructions>100u);
  CHECK(p.source_dma_alignment_is_hypothesis==1u);
 }
 puts("GK129 PASS: mirrored 2KB CPU RAM source DMA, 64 successful atomic transfers, >200 hostile peripheral-page refusals, explicit unverified APU phase, original boot source remains gated");
 return 0;
}
