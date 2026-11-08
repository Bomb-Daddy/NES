#include "gk84_reset_hypothesis.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(v) do {if(!(v)){fprintf(stderr,"GK84 FAILURE %s:%d: %s\n",__FILE__,__LINE__,#v);return 1;}}while(0)
int main(void){
 Gk83Trace out,before,exact;
 memset(&out,0xa5,sizeof out);before=out;
 CHECK(gk84_trace_reset_single_ram_hypothesis(NULL,0xf5,0,20000u,&out)==GK83_INVALID);
 CHECK(!memcmp(&out,&before,sizeof out));
 CHECK(gk84_trace_reset_single_ram_hypothesis("/missing/rom.nes",0x800,0,20000u,&out)==GK83_INVALID);
 CHECK(!memcmp(&out,&before,sizeof out));
 CHECK(gk84_trace_reset_single_ram_hypothesis("/missing/rom.nes",0xf5,0,0,&out)==GK83_INVALID);
 CHECK(!memcmp(&out,&before,sizeof out));
 CHECK(gk84_trace_reset_single_ram_hypothesis("/missing/rom.nes",0xf5,0,20000u,&out)==GK83_SOURCE_REFUSED);
 CHECK(!memcmp(&out,&before,sizeof out));
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom && *rom){
  CHECK(gk83_trace_original_reset(rom,20000u,&exact)==GK83_UNKNOWN_RAM_GATE);
  CHECK(exact.instructions==70 && exact.first_unknown_ram_read==0x00f5u);
  unsigned unknown=0,next_f6=0,next_b3=0,next_bd=0,max_steps=0;
  for(unsigned i=0;i<256u;i++){
   Gk83Trace b={0},repeat={0};
   Gk83Stop status=gk84_trace_reset_single_ram_hypothesis(rom,0x00f5u,(uint8_t)i,20000u,&b);
   CHECK(status!=GK83_INVALID && status!=GK83_SOURCE_REFUSED);
   CHECK(b.instructions>=exact.instructions && b.first_unknown_ram_read!=0x00f5u);
   CHECK(b.control_known==1 && b.prg_known==1);
   CHECK(b.mapper_register_commits==4 && b.control==0x0e && b.prg==14);
   if(status==GK83_UNKNOWN_RAM_GATE)unknown++;
   if(b.first_unknown_ram_read==0x00f6u)next_f6++;
   if(b.first_unknown_ram_read==0x00b3u)next_b3++;
   if(b.first_unknown_ram_read==0x00bdu)next_bd++;
   if(i==0u)CHECK(b.stop_pc==0x8065u&&b.first_unknown_ram_read==0xf6u);
   if(i==2u)CHECK(b.stop_pc==0x8080u&&b.first_unknown_ram_read==0xbdu);
   if(i==1u)CHECK(b.first_unknown_ram_read==0xb3u);
   if(b.instructions>max_steps)max_steps=b.instructions;
   CHECK(gk84_trace_reset_single_ram_hypothesis(rom,0x00f5u,(uint8_t)i,20000u,&repeat)==status);
   CHECK(memcmp(&b,&repeat,sizeof b)==0);
  }
  Gk83Trace pure={0};
  CHECK(gk83_trace_original_reset(rom,20000u,&pure)==GK83_UNKNOWN_RAM_GATE);
  CHECK(memcmp(&pure,&exact,sizeof pure)==0); /* no hypothetical state leakage */
  CHECK(unknown==256u&&next_f6==1u&&next_b3==254u&&next_bd==1u);
  printf("GK84 original ROM: 256/256 power-on-byte counterfactuals; unknown=%u nextF6=%u nextB3=%u nextBD=%u maxsteps=%u\n",unknown,next_f6,next_b3,next_bd,max_steps);
 }
 puts("PASS: exact stock source bounded and all counterfactual paths clearly hypothetical");
 return 0;
}
