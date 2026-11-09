#include "gk92_ppustatus_hypothesis.h"
#include "gk91_reset_zero.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK92 FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk83Trace out,before,zero,high,repeat;memset(&out,0xa5,sizeof out);before=out;
 uint8_t v[]={0,0x80,0x40};
 CHECK(gk92_trace_reset_ppustatus_hypothesis(NULL,v,1,200,&out)==GK83_INVALID);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 CHECK(gk92_trace_reset_ppustatus_hypothesis("/no-rom.nes",NULL,1,100,&out)==GK83_INVALID);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 CHECK(gk92_trace_reset_ppustatus_hypothesis("/no-rom.nes",v,0,100,&out)==GK83_INVALID);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 CHECK(gk92_trace_reset_ppustatus_hypothesis("/no-rom.nes",v,257,100,&out)==GK83_INVALID);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 CHECK(gk92_trace_reset_ppustatus_hypothesis("/no-rom.nes",v,1,0,&out)==GK83_INVALID);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 CHECK(gk92_trace_reset_ppustatus_hypothesis("/no-rom.nes",v,1,100,&out)==GK83_SOURCE_REFUSED);
 CHECK(memcmp(&out,&before,sizeof out)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom && *rom){
  Gk83Trace authentic;
  CHECK(gk91_trace_reset_mesen_allzeros(rom,10000,&authentic)==GK83_HARDWARE_GATE);
  CHECK(authentic.stop_pc==0xc5be && authentic.hypothetical_ppustatus_reads==0);
  uint8_t lo[]={0};uint8_t hi[]={0x80};
  Gk83Stop low_gate=gk92_trace_reset_ppustatus_hypothesis(rom,lo,1,10000,&zero);
  Gk83Stop hi_gate=gk92_trace_reset_ppustatus_hypothesis(rom,hi,1,10000,&high);
  printf("GK92 hypothetical first PPUSTATUS 00: gate=%u instructions=%u PC=%04x consumed=%u\n",(unsigned)low_gate,zero.instructions,zero.stop_pc,zero.hypothetical_ppustatus_reads);
  printf("GK92 hypothetical first PPUSTATUS 80: gate=%u instructions=%u PC=%04x consumed=%u\n",(unsigned)hi_gate,high.instructions,high.stop_pc,high.hypothetical_ppustatus_reads);
  CHECK(low_gate==GK83_HARDWARE_GATE && hi_gate==GK83_HARDWARE_GATE);
  CHECK(zero.instructions==144 && high.instructions==271);
  CHECK(zero.stop_pc==0xc5be && high.stop_pc==0xc5be);
  CHECK(zero.hypothetical_ppustatus_reads==1 && high.hypothetical_ppustatus_reads==1);
  CHECK(zero.instructions>authentic.instructions && high.instructions>authentic.instructions);
  CHECK(zero.first_hardware_read==0x2002 && high.first_hardware_read==0x2002);
  /* MesenCE source resets PPU status flags to zero when the archived fresh
   * settings omit RandomizeMapperPowerOnState (default false). The first
   * PPU read precedes any possible NTSC first VBlank by >26k CPU cycles;
   * its low five open-bus bits remain UNKNOWN. Exhaustively explore all 32
   * possibilities: branch to the next hardware gate must be identical.
   * This proves an INVARIANT source-control-flow step only, not read values
   * observed in a running Mesen emulator. */
  for(unsigned low=0;low<32;low++){
    uint8_t open_bus=(uint8_t)low;
    Gk83Trace branch;
    CHECK(gk92_trace_reset_ppustatus_hypothesis(rom,&open_bus,1,10000,&branch)==GK83_HARDWARE_GATE);
    CHECK(branch.instructions==144 && branch.stop_pc==0xc5be);
    CHECK(branch.hypothetical_ppustatus_reads==1);
    CHECK(branch.first_hardware_read==0x2002);
    CHECK(branch.x==zero.x && branch.y==zero.y && branch.sp==zero.sp);
    CHECK(branch.control==zero.control && branch.prg==zero.prg);
  }
  puts("GK92 32/32 first low open-bus variants preserve first-zero-vblank branch to $C5BE (source-only inference)");
  CHECK(gk92_trace_reset_ppustatus_hypothesis(rom,lo,1,10000,&repeat)==low_gate);
  CHECK(memcmp(&zero,&repeat,sizeof repeat)==0);
  CHECK(gk92_trace_reset_ppustatus_hypothesis(rom,v,3,10000,&repeat)!=GK83_INVALID);
  CHECK(repeat.hypothetical_ppustatus_reads>=1 && repeat.hypothetical_ppustatus_reads<=3);
  printf("GK92 hypothetical 3-read path: gate=%u instructions=%u PC=%04x consumed=%u\n",(unsigned)repeat.stop,repeat.instructions,repeat.stop_pc,repeat.hypothetical_ppustatus_reads);
  CHECK(gk92_trace_reset_ppustatus_hypothesis(rom,lo,1,142,&repeat)==GK83_BUDGET_GATE);
  CHECK(repeat.instructions==142 && repeat.hypothetical_ppustatus_reads==0);
 }
 puts("GK92 explicit hypothetical PPUSTATUS-only trace exploration PASS (not Mesen parity)");
 return 0;
}
