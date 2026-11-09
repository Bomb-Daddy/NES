#include "gk127_io_transaction.h"
#include "gk83_reset_trace.h"
#include "gk106_ppu_bus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK127 FAIL:%d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk127RmwCheck r={0};
 CHECK(!gk127_preflight_rmw(0x2001u,0u,0u,1,NULL));
 for(unsigned old=0;old<256u;old++)for(unsigned next=0;next<256u;next++){
  int ok=gk127_preflight_rmw(0x2001u,(uint8_t)old,(uint8_t)next,1,&r);
  int should=((old|next)&0x18u)==0u;
  CHECK(ok==should);
  if(should)CHECK(r.barrier==GK126_IO_KNOWN_OR_DEFERRED);
  else{
   CHECK(r.barrier==GK126_IO_PPU_RENDER_UNMODELLED);
   CHECK(r.rejected_write_index==((old&0x18u)?0u:1u));
   CHECK(r.rejected_value==((old&0x18u)?old:next));
  }
  CHECK(gk127_preflight_rmw(0x2001u,(uint8_t)old,(uint8_t)next,0,&r));
 }
 CHECK(!gk127_preflight_rmw(0x4014u,0x5au,0x5bu,1,&r));
 CHECK(r.barrier==GK126_IO_OAMDMA_UNMODELLED&&r.rejected_write_index==0u&&r.rejected_value==0x5au);
 CHECK(!gk127_preflight_rmw(0x5000u,0x23u,0x30u,1,&r));
 CHECK(r.barrier==GK126_IO_EXPANSION_UNMODELLED&&r.rejected_value==0x23u);
 CHECK(gk127_preflight_rmw(0x5000u,0x23u,0x30u,0,&r));
 CHECK(gk127_preflight_rmw(0x0800u,0xffu,0x00u,1,&r));
 const char *path=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(path&&*path){
  Gk83Trace trace={0},repeat={0};
  Gk83Stop a=gk106_trace_original_ppu_latch_sprite_hypothesis(path,10240u,0u,30000u,&trace);
  CHECK(a==GK83_HARDWARE_GATE);
  CHECK(trace.first_unmodelled_io_write==0x2001u);
  CHECK(trace.first_unmodelled_io_reason==GK126_IO_PPU_RENDER_UNMODELLED);
  CHECK(trace.stop_pc==0xc5e9u);
  CHECK(trace.unmodelled_instruction_cpu_rolled_back==1u);
  CHECK(trace.unmodelled_rmw_preflight_rejections==0u); /* STA, not RMW */
  CHECK(gk106_trace_original_ppu_latch_sprite_hypothesis(path,10240u,0u,30000u,&repeat)==a);
  CHECK(!memcmp(&repeat,&trace,sizeof trace));
 }
 puts("GK127 bus RMW preflight/CPU rollback: PASS; no original first Spring claim");
 return 0;
}
