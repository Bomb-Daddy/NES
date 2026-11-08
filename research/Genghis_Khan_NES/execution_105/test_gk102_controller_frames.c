#include "gk102_controller_frames.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK102 FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 uint8_t ppustatus[256];memset(ppustatus,0x80,sizeof ppustatus);
 const uint8_t held[]={0x08,0x00,0x01,0,1,0,1,0};
 Gk83Trace got,original;memset(&got,0xA5,sizeof got);original=got;
 CHECK(gk102_trace_scripted_controller_frames(NULL,ppustatus,256,held,8,150000,&got)==GK83_INVALID);
 CHECK(gk102_trace_scripted_controller_frames("/missing",ppustatus,256,held,8,150000,&got)==GK83_SOURCE_REFUSED);
 CHECK(gk102_trace_scripted_controller_frames("/missing",ppustatus,256,held,0,150000,&got)==GK83_INVALID);
 CHECK(gk102_trace_scripted_controller_frames("/missing",ppustatus,256,held,33,150000,&got)==GK83_INVALID);
 CHECK(gk102_trace_scripted_controller_frames("/missing",ppustatus,5,held,8,150000,&got)==GK83_INVALID);
 CHECK(gk102_trace_scripted_controller_frames("/missing",NULL,256,held,8,150000,&got)==GK83_INVALID);
 CHECK(memcmp(&got,&original,sizeof got)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  /* Newly implemented CPU cycle entries are checked explicitly. */
  Gk83Trace a={0},b={0};
  Gk83Stop sa=gk102_trace_scripted_controller_frames(rom,ppustatus,256,held,8,150000,&a);
  Gk83Stop sb=gk102_trace_scripted_controller_frames(rom,ppustatus,256,held,8,150000,&b);
  CHECK(sa==sb && sa!=GK83_SOURCE_REFUSED && sa!=GK83_INVALID);
  CHECK(memcmp(&a,&b,sizeof a)==0);
  CHECK(a.scripted_controller_frames>0&&a.scripted_controller_frames<=8);
  CHECK(a.scripted_controller_frames==a.hypothetical_nmi_injections);
  CHECK(a.scripted_last_buttons==held[a.scripted_controller_frames-1]);
  CHECK(a.hypothetical_controller_reads>=8);
  CHECK(a.scripted_controller_frames==8u);
  /* A separate neutral sequence is a negative control, not a claim that the
   * particular scripted START/A inputs reproduce archived button timing. */
  uint8_t neutral[8]={0};Gk83Trace c={0};
  Gk83Stop sc=gk102_trace_scripted_controller_frames(rom,ppustatus,256,neutral,8,150000,&c);
  CHECK(sc!=GK83_INVALID && sc!=GK83_SOURCE_REFUSED);
  CHECK(c.scripted_controller_frames>0 && c.scripted_controller_frames<=8);
  CHECK(c.hypothetical_nmi_injections==c.scripted_controller_frames);
  printf("GK102 neutral control: events=%u stop=%u PC=$%04X instructions=%u\n",c.scripted_controller_frames,sc,c.stop_pc,c.instructions);
  printf("GK102 SOURCE-TRACED SCRIPTED NMI (NOT Mesen parity): frames=%u stop=%u PC=$%04X inst=%u last=%02X padreads=%u opcode=%02X PPUreads=%u\n",
  a.scripted_controller_frames,sa,a.stop_pc,a.instructions,a.scripted_last_buttons,a.hypothetical_controller_reads,a.stop_opcode,a.hypothetical_ppustatus_reads);
 }else puts("GK102 controlled negative tests PASS; original ROM not present");
 return 0;
}
