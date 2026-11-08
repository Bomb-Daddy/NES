#include "gk91_reset_zero.h"
#include "gk84_reset_hypothesis.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"GK91 FAIL %s:%d %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk83Trace result,before;memset(&result,0xA5,sizeof result);before=result;
 CHECK(gk91_trace_reset_mesen_allzeros(NULL,100,&result)==GK83_INVALID);
 CHECK(memcmp(&result,&before,sizeof result)==0);
 CHECK(gk91_trace_reset_mesen_allzeros("/not-present.nes",100,&result)==GK83_SOURCE_REFUSED);
 CHECK(memcmp(&result,&before,sizeof result)==0);
 CHECK(gk91_trace_reset_mesen_allzeros("/not-present.nes",0,&result)==GK83_INVALID);
 CHECK(memcmp(&result,&before,sizeof result)==0);
 CHECK(gk91_trace_reset_mesen_allzeros("/not-present.nes",1000001,&result)==GK83_INVALID);
 CHECK(memcmp(&result,&before,sizeof result)==0);
 const char *path=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(path&&*path){
  Gk83Trace unseeded,one,again;
  CHECK(gk83_trace_original_reset(path,20000,&unseeded)==GK83_UNKNOWN_RAM_GATE);
  CHECK(unseeded.stop_pc==0x8060&&unseeded.first_unknown_ram_read==0x00f5);
  CHECK(gk84_trace_reset_single_ram_hypothesis(path,0x00f5,0,20000,&one)==GK83_UNKNOWN_RAM_GATE);
  CHECK(one.first_unknown_ram_read==0x00f6);
  Gk83Stop gate=gk91_trace_reset_mesen_allzeros(path,20000,&result);
  printf("GK91 exact archived Mesen AllZeros RAM semantics: gate=%u steps=%u stop_pc=$%04x hw=$%04x unknown=$%04x opcode=$%02x\n",(unsigned)gate,(unsigned)result.instructions,result.stop_pc,result.first_hardware_read,result.first_unknown_ram_read,result.stop_opcode);
  CHECK(gate==GK83_HARDWARE_GATE);
  CHECK(result.instructions==142 && result.stop_pc==0xc5be);
  CHECK(result.first_hardware_read==0x2002 && result.stop_opcode==0xad);
  CHECK(result.instructions>one.instructions);
  CHECK(result.first_unknown_ram_read==0);
  CHECK(gk91_trace_reset_mesen_allzeros(path,20000,&again)==gate);
  CHECK(memcmp(&result,&again,sizeof result)==0);
  Gk83Trace short_run={0};
  CHECK(gk91_trace_reset_mesen_allzeros(path,4,&short_run)==GK83_BUDGET_GATE);
  CHECK(short_run.instructions==4);
  /* Altered original ROM SHA must refuse with an unchanged output. */
  char badname[512];snprintf(badname,sizeof badname,"/tmp/gk91-altered-%ld.nes",(long)getpid());
  FILE *in=fopen(path,"rb");CHECK(in!=NULL);
  FILE *out=fopen(badname,"wb");CHECK(out!=NULL);
  unsigned char b[4096];size_t nr;size_t pos=0;
  while((nr=fread(b,1,sizeof b,in))>0){if(pos==0)b[100]^=1u;CHECK(fwrite(b,1,nr,out)==nr);pos+=nr;}
  CHECK(fclose(in)==0);CHECK(fclose(out)==0);
  before=result;
  CHECK(gk91_trace_reset_mesen_allzeros(badname,20000,&result)==GK83_SOURCE_REFUSED);
  CHECK(memcmp(&result,&before,sizeof result)==0);
  remove(badname);
 }
 puts("GK91 original source MesenCE AllZeros reset CPU trace: PASS; hardware/unsupported gates preserved");
 return 0;
}
