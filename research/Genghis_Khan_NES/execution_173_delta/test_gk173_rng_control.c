#include "gk173_rng_control.h"
#include "gk83_reset_trace.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GK173 FAIL %s line %d\n",#x,__LINE__);return 1;}}while(0)
int main(int argc,char **argv){
 REQUIRE(argc==1||argc==2);
 Gk173RngControl t={0},snapshot;
 REQUIRE(gk173_note_completed(&t,0x20,0x9000,0xef85,3,70)==GK173_NOTED);
 REQUIRE(t.total==1&&t.dispatch_jsr_ef85==1&&t.first_pc==0x9000);
 REQUIRE(gk173_note_completed(&t,0x4c,0xc1b8,0xc289,15,73)==GK173_NOTED);
 REQUIRE(t.total==2&&t.fixed_trampoline_jmp_c289==1&&t.last_prg_register==15);
 REQUIRE(gk173_note_completed(&t,0x20,0xc400,0xc289,0xff,90)==GK173_NOTED);
 REQUIRE(gk173_note_completed(&t,0x4c,0xc180,0xc289,2,91)==GK173_NOTED);
 REQUIRE(gk173_note_completed(&t,0x6c,0xc220,0xc289,2,96)==GK173_NOTED);
 REQUIRE(t.total==5&&t.direct_jsr_c289==1&&t.other_jmp_c289==1&&t.indirect_jmp_c289==1);
 snapshot=t;
 REQUIRE(gk173_note_completed(&t,0xa9,0x9000,0xef85,1,99)==GK173_IGNORED);
 REQUIRE(gk173_note_completed(&t,0x20,0x9000,0xef85,32,99)==GK173_INVALID);
 REQUIRE(gk173_note_completed(NULL,0x20,0x9000,0xef85,1,99)==GK173_INVALID);
 REQUIRE(memcmp(&t,&snapshot,sizeof t)==0);
 t.total=UINT_MAX;snapshot=t;
 REQUIRE(gk173_note_completed(&t,0x20,0x9000,0xef85,1,99)==GK173_INVALID);
 REQUIRE(memcmp(&t,&snapshot,sizeof t)==0);
 if(argc==2){
  /* Source-authenticated bounded interpreter: no synthetic opcode pattern
   * is allowed to count as a completed control transfer. Whether any
   * transfer is reached depends on the real peripheral/hardware gate. */
  Gk83Trace a={0},b={0};
  Gk83Stop x=gk83_trace_original_reset(argv[1],50000,&a);
  Gk83Stop y=gk83_trace_original_reset(argv[1],50000,&b);
  REQUIRE(x==y&&x!=GK83_SOURCE_REFUSED&&x!=GK83_INVALID);
  REQUIRE(memcmp(&a,&b,sizeof a)==0);
  const Gk173RngControl *c=&a.executed_rng_transfers;
  REQUIRE(c->total==c->direct_jsr_c289+c->dispatch_jsr_ef85+
   c->fixed_trampoline_jmp_c289+c->other_jmp_c289+c->indirect_jmp_c289);
  REQUIRE(c->total<=a.instructions);
  printf("PRIVATE AUTHENTIC ROM bounded source CPU: stop=%u pc=$%04X complete_instructions=%u dispatch_JSR=%u RNG_JSR=%u trampoline_JMP=%u indirect_JMP=%u; NOT NES/Mesen parity.\n",
   x,a.stop_pc,a.instructions,c->dispatch_jsr_ef85,c->direct_jsr_c289,
   c->fixed_trampoline_jmp_c289,c->indirect_jmp_c289);
 }
 puts("GK173 original-ROM executed control telemetry and rollback PASS");
 return 0;
}
