#include "gk83_reset_trace.h"
#define CHECK(expr) do {if(!(expr)){fprintf(stderr,"GK83 FAILED at %s:%d: %s\n",__FILE__,__LINE__,#expr);return 1;}}while(0)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int main(void){
 Gk83Trace out;memset(&out,0xa5,sizeof out);
 Gk83Trace before=out;
 CHECK(gk83_trace_original_reset(NULL,100u,&out)==GK83_INVALID);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 CHECK(gk83_trace_original_reset("/unlikely_to_exist/stock.nes",100u,&out)==GK83_SOURCE_REFUSED);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 CHECK(gk83_trace_original_reset("/unlikely_to_exist/stock.nes",0u,&out)==GK83_INVALID);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 CHECK(gk83_trace_original_reset("/unlikely_to_exist/stock.nes",1000001u,&out)==GK83_INVALID);
 CHECK(memcmp(&before,&out,sizeof out)==0);
 const char *path=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(path&&*path){
  Gk83Stop gate=gk83_trace_original_reset(path,20000u,&out);
  printf("STOCK 6502 gate=%u steps=%u pc=$%04x opcode=$%02x mapper commits=%u ctrl=%u prg=%u hw=$%04x unknown_ram=$%04x jsr=%u lowbank=$%04x\n",
   (unsigned)gate,(unsigned)out.instructions,(unsigned)out.stop_pc,(unsigned)out.stop_opcode,
   (unsigned)out.mapper_register_commits,(unsigned)out.control,(unsigned)out.prg,
   (unsigned)out.first_hardware_read,(unsigned)out.first_unknown_ram_read,(unsigned)out.jsr_count,(unsigned)out.first_banked_pc);
  CHECK(gate==GK83_HARDWARE_GATE||gate==GK83_UNKNOWN_RAM_GATE||
         gate==GK83_OPCODE_GATE||gate==GK83_BANK_GATE||gate==GK83_BRK_GATE);
  CHECK(out.instructions>40);
  CHECK(out.mapper_serial_writes>10);
  CHECK(out.mapper_register_commits>=2);
  CHECK(out.control_known&&out.prg_known);
  CHECK(out.first_jsr==0xc71f);
  CHECK(gate==GK83_UNKNOWN_RAM_GATE && out.first_unknown_ram_read==0x00f5);
  CHECK(out.stop_pc==0x8060 && out.control==0x0e && out.prg==14);
  CHECK(out.mapper_register_commits==4 && out.instructions==70);
  Gk83Trace one;
  CHECK(gk83_trace_original_reset(path,20000u,&one)==gate);
  CHECK(memcmp(&one,&out,sizeof one)==0);
  Gk83Trace few={0};
  CHECK(gk83_trace_original_reset(path,4,&few)==GK83_BUDGET_GATE);
  CHECK(few.instructions==4&&few.mapper_serial_writes==0);
 }
 puts("GK83 bounded stock 6502 trace: PASS; no fabricated hardware read or first Spring");
 return 0;
}
