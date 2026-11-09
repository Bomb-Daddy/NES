#include "gk107_ppu_ports.h"
#include "gk83_reset_trace.h"
#include "gk103_ntsc_ppu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHECK(e) do{if(!(e)){fprintf(stderr,"EXEC0107 FAIL %d: %s\n",__LINE__,#e);return 1;}}while(0)
int main(void){
 Gk106PpuBus bus;gk106_ppu_bus_reset(&bus);
 uint8_t read=0x4fu;
 const uint16_t read_ports[]={0x2000,0x2001,0x2003,0x2005,0x2006,
                              0x2008,0x3ff8,0x3ff9,0x3ffb,0x3ffd,0x3ffe};
 const uint16_t other_ports[]={0x2002,0x2004,0x2007,0x3ffa,0x3ffc,0x3fff,0x1fff,0x4000};
 for(unsigned i=0;i<sizeof read_ports/sizeof read_ports[0];i++){
  CHECK(gk107_ppu_read_write_only(&bus,read_ports[i],20,0,&read)==GK107_PORT_UNKNOWN);
  CHECK(read==0x4fu);
 }
 CHECK(gk106_ppu_bus_cpu_write(&bus,0xa6u));
 for(unsigned i=0;i<sizeof read_ports/sizeof read_ports[0];i++){
  CHECK(gk107_ppu_read_write_only(&bus,read_ports[i],4000,1000,&read)==GK107_PORT_OK);
  CHECK(read==0xa6u);
  read=0x4fu;
  CHECK(gk107_ppu_read_write_only(&bus,read_ports[i],4001,1000,&read)==GK107_PORT_STALE);
  CHECK(read==0x4fu);
 }
 for(unsigned i=0;i<sizeof other_ports/sizeof other_ports[0];i++){
  CHECK(gk107_ppu_read_write_only(&bus,other_ports[i],1000,1000,&read)==GK107_PORT_INVALID);
  CHECK(read==0x4fu);
 }
 CHECK(gk107_ppu_read_write_only(&bus,0x2000,5,10,&read)==GK107_PORT_STALE);
 CHECK(read==0x4fu);
 CHECK(gk107_ppu_read_write_only(&bus,0x2000,0,0,NULL)==GK107_PORT_INVALID);
 CHECK(gk107_ppu_read_write_only(NULL,0x2000,0,0,&read)==GK107_PORT_INVALID);
 CHECK(gk106_ppu_bus_status_read(&bus,0xe0u,&read)&&read==0xe6u);
 CHECK(gk107_ppu_read_write_only(&bus,0x2003,1001,1000,&read)==GK107_PORT_OK&&read==0xe6u);
 /* Original exact stock SHA is checked by the existing tracer. Missing source
  * must reject atomically; these tests never assert gameplay from a trace. */
 Gk83Trace prior,next;memset(&prior,0x69,sizeof prior);next=prior;
 CHECK(gk106_trace_original_ppu_latch(NULL,0,1000,&next)==GK83_INVALID);
 CHECK(memcmp(&prior,&next,sizeof next)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  const uint32_t origins[]={0u,10240u,80000u};
  for(unsigned i=0;i<3;i++){
   Gk83Trace a={0},b={0};
   Gk83Stop x=gk106_trace_original_ppu_latch_sprite_hypothesis(rom,origins[i],0u,30000u,&a);
   Gk83Stop y=gk106_trace_original_ppu_latch_sprite_hypothesis(rom,origins[i],0u,30000u,&b);
   CHECK(x==y&&x!=GK83_INVALID&&x!=GK83_SOURCE_REFUSED);
   CHECK(memcmp(&a,&b,sizeof a)==0);
   printf("SOURCE_HYPOTHESIS origin=%u stop=%d pc=%04x write_only_reads=%u stale_gates=%u\n",
          origins[i],x,a.stop_pc,a.ppu_write_only_bus_reads,a.ppu_write_only_age_gates);
  }
 }else puts("NO PRIVATE ROM: isolated port semantics PASS only");
 return 0;
}
