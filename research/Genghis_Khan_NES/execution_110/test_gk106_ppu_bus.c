#include "gk106_ppu_bus.h"
#include "gk83_reset_trace.h"
#include "gk103_ntsc_ppu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHECK(e) do{if(!(e)){fprintf(stderr,"EXEC0106 FAIL %d %s\n",__LINE__,#e);return 1;}}while(0)
int main(void){
 Gk106PpuBus bus={0};uint8_t out=0xa5u;
 gk106_ppu_bus_reset(&bus);
 CHECK(!gk106_ppu_bus_low5(&bus,&out)&&out==0xa5u);
 CHECK(!gk106_ppu_bus_status_read(&bus,0x80u,&out)&&out==0xa5u);
 CHECK(!gk106_ppu_bus_cpu_write(NULL,0x11u));
 CHECK(gk106_ppu_bus_cpu_write(&bus,0x35u));
 CHECK(gk106_ppu_bus_low5(&bus,&out)&&out==0x15u);
 CHECK(!gk106_ppu_bus_status_read(&bus,0x9fu,&out)&&out==0x15u&&bus.status_reads==0u);
 CHECK(gk106_ppu_bus_status_read(&bus,0xa0u,&out)&&out==0xb5u);
 CHECK(bus.value==0xb5u&&bus.status_reads==1u);
 CHECK(gk106_ppu_bus_status_read(&bus,0x00u,&out)&&out==0x15u);
 CHECK(bus.value==0x15u&&bus.cpu_writes==1u);
 CHECK(gk106_ppu_bus_cpu_write(&bus,0xecu));
 CHECK(gk106_ppu_bus_status_read(&bus,0x80u,&out)&&out==0x8cu);
 CHECK(bus.cpu_writes==2u&&bus.status_reads==3u);
 /* A write to either $2000 or its mirror $2008 must be routed here. The
  * function models the bus value, not a fictional PPU register array. */
 Gk106PpuBus before=bus;
 CHECK(!gk106_ppu_bus_low5(&bus,NULL));
 CHECK(!gk106_ppu_bus_status_read(&bus,0x60u,NULL));
 CHECK(memcmp(&bus,&before,sizeof bus)==0);
 Gk83Trace reject;memset(&reject,0x5d,sizeof reject);
 Gk83Trace unchanged=reject;
 CHECK(gk106_trace_original_ppu_latch(NULL,0u,10u,&reject)==GK83_INVALID);
 CHECK(gk106_trace_original_ppu_latch("/absent",GK103_FRAME_DOTS,10u,&reject)==GK83_INVALID);
 CHECK(gk106_trace_original_ppu_latch("/absent",GK103_VBLANK_START,10u,&reject)==GK83_INVALID);
 CHECK(gk106_trace_original_ppu_latch("/absent",0u,10u,&reject)==GK83_SOURCE_REFUSED);
 CHECK(gk106_trace_original_ppu_latch_sprite_hypothesis("/absent",0u,4u,10u,&reject)==GK83_INVALID);
 CHECK(memcmp(&reject,&unchanged,sizeof reject)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  for(unsigned i=0;i<3;i++){
   const uint32_t origins[]={0u,10240u,80000u};
   Gk83Trace first={0},second={0};
   Gk83Stop a=gk106_trace_original_ppu_latch(rom,origins[i],30000u,&first);
   Gk83Stop b=gk106_trace_original_ppu_latch(rom,origins[i],30000u,&second);
   CHECK(a!=GK83_INVALID&&a!=GK83_SOURCE_REFUSED&&a==b);
   CHECK(memcmp(&first,&second,sizeof first)==0);
   CHECK(first.ppu_scheduled_frame_origin_dots==origins[i]);
   CHECK(first.ppu_bus_latch_writes>=1u);
   CHECK(first.ppu_bus_latch_known==1u);
   printf("SOURCE_HYPOTHESIS PPU_BUS origin=%u stop=%d pc=%04x writes=%u reads=%u gate=%u\n",
    origins[i],(int)a,first.stop_pc,first.ppu_bus_latch_writes,
    first.ppu_bus_latch_reads,first.ppu_bus_latch_gate);
   printf("TRACE cycles=%llu instructions=%u ppu_frames=%llu first_hardware=%04x\n",
     (unsigned long long)first.completed_cpu_cycles,first.instructions,
     (unsigned long long)first.ppu_scheduled_frames,first.first_hardware_read);
   /* The real 6502 CPU writes establish low-five bits. Early sprite flags
    * remain ambiguous. Compare all four expressly hypothetical flag states. */
   Gk83Trace hypotheses[4]={{0}};
   for(unsigned choice=0;choice<4;choice++){
    Gk83Stop st=gk106_trace_original_ppu_latch_sprite_hypothesis(
       rom,origins[i],(uint8_t)choice,30000u,&hypotheses[choice]);
    CHECK(st!=GK83_INVALID&&st!=GK83_SOURCE_REFUSED);
    CHECK(hypotheses[choice].ppu_bus_latch_reads>0u);
    CHECK(hypotheses[choice].ppu_bus_latch_known==1u);
    if(choice){
     CHECK(hypotheses[choice].stop==hypotheses[0].stop);
     CHECK(hypotheses[choice].stop_pc==hypotheses[0].stop_pc);
     CHECK(hypotheses[choice].instructions==hypotheses[0].instructions);
     CHECK(hypotheses[choice].ppu_bus_latch_reads==hypotheses[0].ppu_bus_latch_reads);
    }
    printf("SPRITE_FLAGS_HYPOTHESIS origin=%u flags=%u pc=%04x instructions=%u reads=%u\n",
      origins[i],choice,hypotheses[choice].stop_pc,
      hypotheses[choice].instructions,hypotheses[choice].ppu_bus_latch_reads);
   }
  }
 }else puts("EXEC0106 standalone PPU bus rollback PASS; private ROM NOT RUN");
 return 0;
}
