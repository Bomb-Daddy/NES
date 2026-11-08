#include "gk92_ppustatus_hypothesis.h"
#include "gk83_reset_trace.h"
#include "gk91_reset_zero.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"GK93 FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
/* Copyright-clean deterministic test. Exact original ROM supplied privately
 * via environment at test runtime, never checked into this source archive.
 * 0x80 repeated is deliberately COUNTERFACTUAL: not a Mesen observation.
 * It exercises real 6502 bytes only after exact ROM SHA authentication. */
int main(void){
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(!rom||!*rom){puts("GK93 SKIP private original ROM not attached; source builds unaffected");return 0;}
 Gk83Trace normal={0};
 CHECK(gk83_trace_original_reset(rom,20000u,&normal)==GK83_UNKNOWN_RAM_GATE);
 CHECK(normal.instructions>0u && normal.hypothetical_ppustatus_reads==0u);
 /* Prior exact fresh Mesen AllZeros source gate MUST stay unchanged. */
 Gk83Trace zero={0};
 CHECK(gk91_trace_reset_mesen_allzeros(rom,20000u,&zero)==GK83_HARDWARE_GATE);
 CHECK(zero.instructions==142u && zero.stop_pc==0xc5beu);
 uint8_t high[256];memset(high,0x80,sizeof high);
 Gk83Trace deep={0};
 Gk83Stop s=gk92_trace_reset_ppustatus_hypothesis(rom,high,256,3000,&deep);
 CHECK(s==GK83_BUDGET_GATE);
 CHECK(deep.instructions==3000u && deep.hypothetical_ppustatus_reads>0u);
 CHECK(deep.mapper_register_commits>=9u && deep.jsr_count>=68u);
 /* Most importantly, no counterfactual execution leaks to normal trace. */
 Gk83Trace again={0};
 CHECK(gk91_trace_reset_mesen_allzeros(rom,20000u,&again)==GK83_HARDWARE_GATE);
 CHECK(memcmp(&zero,&again,sizeof zero)==0);
 Gk83Trace sentinel;memset(&sentinel,0xa5,sizeof sentinel);
 Gk83Trace untouched=sentinel;
 CHECK(gk92_trace_reset_ppustatus_hypothesis(rom,high,257,3000,&sentinel)==GK83_INVALID);
 CHECK(memcmp(&untouched,&sentinel,sizeof sentinel)==0);
 puts("GK93 PASS exact original source 6502 arithmetic/stack/indirect setup route; hypothetical PPU; no gameplay parity");
 return 0;
}
