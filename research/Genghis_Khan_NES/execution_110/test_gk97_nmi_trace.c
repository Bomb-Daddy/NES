#include "gk97_nmi_trace.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"GK97 FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 uint8_t ppu[256];memset(ppu,0x80,sizeof ppu);
 Gk83Trace out,original;
 memset(&out,0x5e,sizeof out);original=out;
 CHECK(gk97_trace_reset_one_hypothetical_nmi(NULL,ppu,256,30000,&out)==GK83_INVALID);
 CHECK(gk97_trace_reset_one_hypothetical_nmi("",ppu,256,30000,&out)==GK83_INVALID);
 CHECK(gk97_trace_reset_one_hypothetical_nmi("/missing/rom.nes",ppu,256,30000,&out)==GK83_SOURCE_REFUSED);
 CHECK(gk97_trace_reset_one_hypothetical_nmi("/missing/rom.nes",ppu,5,30000,&out)==GK83_INVALID);
 CHECK(memcmp(&out,&original,sizeof out)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(!rom||!*rom){puts("GK97 private original ROM not supplied: fail-closed tests PASS");return 0;}
 Gk83Stop stop=gk97_trace_reset_one_hypothetical_nmi(rom,ppu,256,30000,&out);
 CHECK(out.hypothetical_nmi_injections==1);
 CHECK(out.hypothetical_nmi_vector>=0x8000);
 CHECK(stop!=GK83_SOURCE_REFUSED&&stop!=GK83_INVALID);
 CHECK(stop==GK83_HARDWARE_GATE);
 CHECK(out.hypothetical_nmi_vector==0xc0d9);
 CHECK(out.stop_pc==0xc6ae);
 CHECK(out.last_hardware_read==0x4016);
 Gk83Trace replay={0};
 CHECK(gk97_trace_reset_one_hypothetical_nmi(rom,ppu,256,30000,&replay)==stop);
 CHECK(memcmp(&out,&replay,sizeof out)==0);
 printf("GK97 HYPOTHETICAL ONE-SHOT NMI: vector=$%04X, stop=%u, pc=$%04X, hardware_addr=$%04X, instructions=%u, ppu_reads=%u (NOT NES PPU TIMING)\n",
 out.hypothetical_nmi_vector,stop,out.stop_pc,out.last_hardware_read,out.instructions,out.hypothetical_ppustatus_reads);
 return 0;
}
