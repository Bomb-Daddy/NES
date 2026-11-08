#include "gk96_ppu_warmup.h"
#include <string.h>
/* Not real hardware timing. Deliberately contrast first- and fourth-read
 * vblank to check how SOURCE BYTES behave once PPUSTATUS ceases to gate.
 * The 6502 interpreter has no NMI; reaching an identical repeating source
 * loop is a diagnostic blocker, NOT a license to synthesize a game state. */
Gk96Result gk96_original_reset_warmup(const char *rom,Gk96Warmup *out){
 if(!rom||!*rom||!out)return GK96_INVALID;
 Gk96Warmup candidate={0};
 uint8_t early[256],late[256];
 memset(early,0x80,sizeof early);
 memset(late,0x80,sizeof late);
 late[0]=late[1]=late[2]=0;
 Gk83Stop a=gk92_trace_reset_ppustatus_hypothesis(rom,early,256,30000,
                                                     &candidate.immediate_vblank);
 if(a==GK83_SOURCE_REFUSED)return GK96_SOURCE_REFUSED;
 if(a==GK83_INVALID)return GK96_INVALID;
 Gk83Stop b=gk92_trace_reset_ppustatus_hypothesis(rom,late,256,30000,
                                                     &candidate.delayed_vblank);
 if(b==GK83_SOURCE_REFUSED)return GK96_SOURCE_REFUSED;
 if(b==GK83_INVALID)return GK96_INVALID;
 candidate.scenarios_checked=2;
 if(a==GK83_BUDGET_GATE&&b==GK83_BUDGET_GATE&&
    candidate.immediate_vblank.stop_pc==candidate.delayed_vblank.stop_pc&&
    candidate.immediate_vblank.stop_pc==0xc460u&&
    candidate.immediate_vblank.hypothetical_ppustatus_reads>=3&&
    candidate.delayed_vblank.hypothetical_ppustatus_reads>=6){
   candidate.required_nmi=1;
   *out=candidate;
   return GK96_NEEDS_NMI;
 }
 *out=candidate;return GK96_OTHER_GATE;
}
