#include "gk205_selector_runtime_watch.h"
#include "gk83_reset_trace.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#define CHECK(q) do { if(!(q)){fprintf(stderr,"GK205 FAIL %s:%d %s\n",__FILE__,__LINE__,#q);return 1;} ++count;}while(0)
int main(void){
 unsigned count=0;
 Gk205SelectorWatch w={0},before;
 before=w;
 CHECK(gk205_note_completed_selector_store(NULL,0xc111,0x50,17,1,7,22)==GK205_BAD_INPUT);
 CHECK(gk205_note_completed_selector_store(&w,0xc111,0x51,17,1,7,22)==GK205_OUTSIDE);
 CHECK(!memcmp(&before,&w,sizeof(w)));
 CHECK(gk205_note_completed_selector_store(&w,0x4000,0x50,17,1,7,22)==GK205_BAD_INPUT);
 CHECK(!memcmp(&before,&w,sizeof(w)));
 CHECK(gk205_note_completed_selector_store(&w,0xc111,0x50,17,2,7,22)==GK205_BAD_INPUT);
 CHECK(!memcmp(&before,&w,sizeof(w)));
 CHECK(gk205_note_completed_selector_store(&w,0xc111,0x50,17,1,16,22)==GK205_BAD_INPUT);
 CHECK(!memcmp(&before,&w,sizeof(w)));
 CHECK(gk205_note_completed_selector_store(&w,0xc111,0x2050,17,1,7,22)==GK205_OUTSIDE);
 CHECK(!memcmp(&before,&w,sizeof(w)));
 CHECK(gk205_note_completed_selector_store(&w,0xc111,0x0050,17,1,7,22)==GK205_RECORDED);
 CHECK(w.completed_selector_writes==1&&w.first[0].selector_value==17);
 CHECK(w.first[0].pc==0xc111&&w.first[0].address==0x50&&w.first[0].bus_cpu_cycle==22);
 CHECK(gk205_note_completed_selector_store(&w,0x8123,0x1850,23,0,0,26)==GK205_RECORDED);
 CHECK(w.completed_selector_writes==2&&w.last.selector_value==23&&w.last.bank_known==0);
 CHECK(gk205_note_brk_gate(NULL,0xc111,1,17,1,2)==GK205_BAD_INPUT);
 before=w;
 CHECK(gk205_note_brk_gate(&w,0xc111,2,17,1,2)==GK205_BAD_INPUT);
 CHECK(!memcmp(&before,&w,sizeof(w)));
 CHECK(gk205_note_brk_gate(&w,0xc111,0,17,0,2)==GK205_BRK_ATTEMPT);
 CHECK(w.brk_gates==1&&w.brk_pc==0xc111&&w.brk_selector_known==0);
 CHECK(w.brk_selector_value==0&&w.brk_bank_known==0&&w.brk_bank==0);
 CHECK(gk205_note_brk_gate(&w,0xc113,1,17,1,3)==GK205_BRK_ATTEMPT);
 CHECK(w.brk_gates==2&&w.brk_selector_known&&w.brk_selector_value==17&&w.brk_bank==3);
 for(unsigned i=2;i<22;i++)
  CHECK(gk205_note_completed_selector_store(&w,0xc555,0x850,(uint8_t)i,1,0,i)==GK205_RECORDED);
 CHECK(w.completed_selector_writes==22&&w.dropped_event_details==6);
 CHECK(w.first[15].selector_value==15&&w.last.selector_value==21);
 w.completed_selector_writes=UINT32_MAX;before=w;
 CHECK(gk205_note_completed_selector_store(&w,0xc555,0x50,1,1,0,1)==GK205_BAD_INPUT);
 CHECK(!memcmp(&before,&w,sizeof(w)));
 /* Genuine stock input is not packaged with the public test. */
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  Gk83Trace trace={0};
  Gk83Stop stop=gk83_trace_original_reset(rom,20000,&trace);
  CHECK(stop==GK83_UNKNOWN_RAM_GATE);
  CHECK(trace.stop_pc==0x8060&&trace.instructions==70);
  CHECK(trace.selector_runtime.completed_selector_writes==0);
  CHECK(trace.selector_runtime.brk_gates==0);
  CHECK(trace.first_unknown_ram_read==0xf5);
 }
 printf("GK205 bounded 6502 selector-write/BRK-gate telemetry: %u assertions PASS; original source gate is NOT an NES game initialization\n",count);
 return 0;
}
