#include "gk101_mmc1_cycles.h"
#include "gk83_reset_trace.h"
#include "gk92_ppustatus_hypothesis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK101 FAIL at %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk101Mmc1Filter f={0};
 CHECK(gk101_mmc1_write(NULL,12,0)==GK101_NONMONOTONIC);
 CHECK(gk101_mmc1_write(&f,50,0)==GK101_ACCEPT);
 CHECK(gk101_mmc1_write(&f,51,1)==GK101_IGNORED); /* RMW final bit */
 CHECK(gk101_mmc1_write(&f,52,1)==GK101_IGNORED); /* all continuing consecutive */
 CHECK(gk101_mmc1_write(&f,52,0)==GK101_NONMONOTONIC); /* duplicate timestamp */
 CHECK(gk101_mmc1_write(&f,53,0x80)==GK101_RESET); /* bit7 reset always honored */
 CHECK(gk101_mmc1_write(&f,54,0)==GK101_IGNORED); /* next-cycle bit0 still ignored */
 CHECK(gk101_mmc1_write(&f,56,1)==GK101_ACCEPT);
 CHECK(f.accepted==3u&&f.ignored==3u&&f.resets==1u&&f.previous_cycle==56u);
 /* A normal two-write RMW sends its OLD value on penultimate CPU cycle.
    Second different value is suppressed; don't fabricate a mapper bit. */
 Gk101Mmc1Filter rmw={0};
 CHECK(gk101_mmc1_write(&rmw,1000,0x01)==GK101_ACCEPT);
 CHECK(gk101_mmc1_write(&rmw,1001,0x00)==GK101_IGNORED);
 CHECK(rmw.accepted==1&&rmw.ignored==1);
 /* Exception: second write with bit7=1 causes reset even if consecutive. */
 CHECK(gk101_mmc1_write(&rmw,1002,0x80)==GK101_RESET);
 CHECK(rmw.resets==1u);
 Gk101Mmc1Filter detached={0};
 CHECK(gk101_mmc1_write(&detached,UINT64_MAX-1,1)==GK101_ACCEPT);
 CHECK(gk101_mmc1_write(&detached,UINT64_MAX,0)==GK101_IGNORED);
 CHECK(gk101_mmc1_write(&detached,0,0)==GK101_NONMONOTONIC);
 /* On invalid or unknown exact original private ROM, output stays identical. */
 Gk83Trace refuse;memset(&refuse,0xA5,sizeof refuse);
 Gk83Trace sentinel=refuse;
 CHECK(gk83_trace_original_reset("/not/a/stock/rom",2000,&refuse)==GK83_SOURCE_REFUSED);
 CHECK(memcmp(&refuse,&sentinel,sizeof refuse)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  uint8_t status[256];memset(status,0x80,sizeof status);
  Gk83Trace a={0},b={0};
  Gk83Stop sa=gk92_trace_reset_ppustatus_hypothesis(rom,status,256,30000,&a);
  Gk83Stop sb=gk92_trace_reset_ppustatus_hypothesis(rom,status,256,30000,&b);
  CHECK(sa==sb && sa!=GK83_SOURCE_REFUSED && sa!=GK83_INVALID);
  CHECK(memcmp(&a,&b,sizeof a)==0);
  CHECK(a.mapper_serial_writes>=a.mapper_consecutive_ignored);
  CHECK(a.mapper_bit7_resets<=a.mapper_serial_writes);
  CHECK(a.mapper_rmw_double_writes<=a.mapper_serial_writes/2);
  CHECK(a.completed_cpu_cycles>=a.instructions*2u);
  printf("GK101 VERIFIED_STATIC original source: serial attempts %u, suppressed %u, reset %u, RMW pairs %u, stop %u at $%04X; no PPU/NMI parity inferred\n",
       a.mapper_serial_writes,a.mapper_consecutive_ignored,a.mapper_bit7_resets,
       a.mapper_rmw_double_writes,sa,a.stop_pc);
 }else puts("GK101 MMC1 cycle filter PASS; stock original private ROM was not supplied");
 return 0;
}
