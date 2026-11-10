#include "gk153_country_write_watch.h"
#include "gk1_app_state.h"
#include "gk91_reset_zero.h"
#include "gk82_stock_cartridge.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK153 FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 CHECK(GK153_MONGOL_END==0x7257u);
 CHECK(GK153_WORLD_END==0x768eu);
 Gk153CountryWatch w={0},before=w;
 CHECK(gk153_note_executed_write(NULL,0x6dcdu,0x8000u)==GK153_INVALID);
 CHECK(gk153_note_executed_write(&w,0x6dccu,0x8000u)==GK153_OUTSIDE);
 CHECK(gk153_note_executed_write(&w,0x768eu,0x8000u)==GK153_OUTSIDE);
 CHECK(memcmp(&before,&w,sizeof w)==0);
 CHECK(gk153_note_executed_write(&w,0x6dcdu,0xc100u)==GK153_RECORDED);
 CHECK(w.first_country_index==1u&&w.first_cpu_address==0x6dcdu);
 CHECK(gk153_note_executed_write(&w,0x7256u,0xc200u)==GK153_RECORDED);
 CHECK(w.last_country_index==14u);
 CHECK(gk153_note_executed_write(&w,0x7257u,0xc300u)==GK153_RECORDED);
 CHECK(w.last_country_index==15u);
 CHECK(gk153_note_executed_write(&w,0x768du,0xc400u)==GK153_RECORDED);
 CHECK(w.last_country_index==27u&&w.world_region_writes==4u&&w.mongol_region_writes==2u);
 before=w;
 CHECK(gk153_note_executed_write(&w,0x7fffu,0x9000u)==GK153_OUTSIDE);
 CHECK(memcmp(&before,&w,sizeof w)==0);
 w.world_region_writes=UINT32_MAX;before=w;
 CHECK(gk153_note_executed_write(&w,0x6dcdu,0x8000u)==GK153_INVALID);
 CHECK(memcmp(&before,&w,sizeof w)==0);
 w.world_region_writes=4;w.mongol_region_writes=UINT32_MAX;before=w;
 CHECK(gk153_note_executed_write(&w,0x7256u,0x8000u)==GK153_INVALID);
 CHECK(memcmp(&before,&w,sizeof w)==0);
 Gk1App app;gk1_app_init(&app);
 Gk83Trace report;memset(&report,0xa3,sizeof report);Gk83Trace sent=report;
 CHECK(gk1_app_source_country_writer_trace(&app,"/invalid",&report)==GK153_TRACE_BAD_STAGE);
 CHECK(memcmp(&sent,&report,sizeof report)==0);
 app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 CHECK(gk1_app_source_country_writer_trace(&app,"/invalid",&report)==GK153_TRACE_BAD_SOURCE);
 CHECK(memcmp(&sent,&report,sizeof report)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk82RomBootstrap boot;
  CHECK(gk82_probe_stock_rom(rom,&boot)==GK82_ROM_OK);
  Gk1App pristine=app;
  CHECK(gk1_app_source_country_writer_trace(&app,rom,&report)==GK153_TRACE_BOUNDED);
  CHECK(memcmp(&pristine,&app,sizeof app)==0);
  CHECK(report.country_writes.mongol_region_writes<=report.country_writes.world_region_writes);
  CHECK(report.stop==GK83_HARDWARE_GATE||report.stop==GK83_UNKNOWN_RAM_GATE||
        report.stop==GK83_OPCODE_GATE||report.stop==GK83_BANK_GATE||
        report.stop==GK83_BUDGET_GATE||report.stop==GK83_BRK_GATE);
  printf("GK153 verified stock source CPU trace: %u PRG-RAM country-range stores (%u Mongol range); stop=$%04X gate=%u; NO NES first Spring\n",
    report.country_writes.world_region_writes,report.country_writes.mongol_region_writes,
    report.stop_pc,(unsigned)report.stop);
 }else puts("GK153 boundary/overflow/atomic refusal synthetic tests PASS; private ROM not provided");
 return 0;
}
