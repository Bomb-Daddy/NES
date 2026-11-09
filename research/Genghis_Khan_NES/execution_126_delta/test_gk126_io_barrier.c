#include "gk126_io_barrier.h"
#include "gk83_reset_trace.h"
#include "gk91_reset_zero.h"
#include "gk103_ppu_schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"GK126 FAIL %d: %s\n",__LINE__,#c);return 1;} }while(0)
int main(void){
 for(unsigned a=0;a<=65535u;a++){
  for(unsigned p=0;p<2;p++){
   Gk126IoBarrier g=gk126_io_write_barrier((uint16_t)a,0u,(int)p);
   CHECK(g==(p&&a==0x4014u?GK126_IO_OAMDMA_UNMODELLED:
     p&&a>=0x4020u&&a<0x6000u?GK126_IO_EXPANSION_UNMODELLED:
     GK126_IO_KNOWN_OR_DEFERRED));
  }
 }
 for(unsigned v=0;v<256u;v++){
  for(unsigned addr=0x2000u;addr<=0x3fffu;addr++){
   Gk126IoBarrier g=gk126_io_write_barrier((uint16_t)addr,(uint8_t)v,1);
   CHECK(g==(((addr&7u)==1u&&(v&0x18u))?
    GK126_IO_PPU_RENDER_UNMODELLED:GK126_IO_KNOWN_OR_DEFERRED));
   CHECK(gk126_io_write_barrier((uint16_t)addr,(uint8_t)v,0)==
     GK126_IO_KNOWN_OR_DEFERRED);
  }
 }
 Gk83Trace b;memset(&b,0x69,sizeof b);Gk83Trace saved=b;
 CHECK(gk83_trace_original_reset(NULL,1000,&b)==GK83_INVALID);
 CHECK(gk103_trace_original_ppu_schedule("/not/there",0,1000,&b)==GK83_SOURCE_REFUSED);
 CHECK(!memcmp(&b,&saved,sizeof b));
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk83Trace a={0},r={0},z={0};
  CHECK(gk83_trace_original_reset(rom,30000,&r)==GK83_UNKNOWN_RAM_GATE);
  CHECK(r.first_unknown_ram_read==0x00f5u&&r.instructions==70u);
  CHECK(gk91_trace_reset_mesen_allzeros(rom,30000,&z)==GK83_HARDWARE_GATE);
  CHECK(z.first_hardware_read==0x2002u && z.instructions==142u);
  Gk83Stop stop=gk106_trace_original_ppu_latch_sprite_hypothesis(
     rom,10240u,0u,30000u,&a);
  CHECK(stop==GK83_HARDWARE_GATE);
  CHECK(a.first_unmodelled_io_write==0x2001u);
  CHECK(a.first_unmodelled_io_reason==GK126_IO_PPU_RENDER_UNMODELLED);
  CHECK((a.first_unmodelled_io_value&0x18u)!=0u);
  CHECK(a.stop_pc==0xc5e9u);
  Gk83Trace again={0};
  CHECK(gk106_trace_original_ppu_latch_sprite_hypothesis(
     rom,10240u,0u,30000u,&again)==stop);
  CHECK(!memcmp(&a,&again,sizeof a));
  printf("SOURCE_TRACE verified PPU render barrier PC=$%04x value=$%02x, %u completed instructions; no first Spring\n",
         a.stop_pc,a.first_unmodelled_io_value,a.instructions);
 }else puts("GK126 CLASSIFIER PASS; stock ROM source path not run");
 return 0;
}
