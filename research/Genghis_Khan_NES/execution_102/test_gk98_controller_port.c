#include "gk98_controller_port.h"
#include "gk98_controller_trace.h"
#include "gk98_cpu_rotate.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK98 FAIL line %u: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 for(unsigned a=0;a<256u;a++)for(unsigned c=0;c<2u;c++){
  Gk98Rotate r=gk98_cpu_ror((uint8_t)a,(uint8_t)c);
  CHECK(r.value==(uint8_t)((a>>1u)|(c<<7u)));
  CHECK(r.carry==(a&1u));
 }
 Gk98Pad p={0};
 gk98_pad_reset(&p,0xa5u); /* A, Select, Down, Right pressed */
 gk98_pad_write(&p,1u);
 CHECK(gk98_pad_read(&p,0xacu)==0xadu); /* Strobe high repeats A */
 CHECK(gk98_pad_read(&p,0xacu)==0xadu);
 CHECK(p.index==0u);
 gk98_pad_write(&p,0u);
 for(unsigned i=0;i<8;i++)CHECK(gk98_pad_read(&p,0xfeu)==(uint8_t)(0xfeu|((0xa5u>>i)&1u)));
 CHECK(gk98_pad_read(&p,0u)==1u);
 CHECK(gk98_pad_read(&p,0x80u)==0x81u);
 CHECK(p.index==8u&&p.reads==12u&&p.writes==2u);
 Gk98Pad p2={0};gk98_pad_reset(&p2,0x02u);
 gk98_pad_write(&p2,1u);gk98_pad_write(&p2,0u);
 CHECK(gk98_pad_read(&p2,0u)==0u);
 CHECK(gk98_pad_read(&p2,0u)==1u);
 CHECK(p2.reads==2u&&p2.index==2u);
 p.buttons=0x08u; /* START: latch persists until next strobe */
 CHECK(gk98_pad_read(&p,0u)==1u);
 gk98_pad_write(&p,1u);gk98_pad_write(&p,0u);
 for(unsigned i=0;i<8;i++)CHECK(gk98_pad_read(&p,0u)==((0x08u>>i)&1u));
 CHECK(p.index==8u&&p.latched==0x08u);
 gk98_pad_reset(&p,0u);gk98_pad_write(&p,0u);
 CHECK(gk98_pad_read(&p,0u)==0u);
 CHECK(gk98_pad_read(&p,0u)==0u);
 uint8_t ppu[256];memset(ppu,0x80,sizeof ppu);
 Gk83Trace out,initial;memset(&out,0x5au,sizeof out);initial=out;
 CHECK(gk98_trace_reset_controller_hypothesis(NULL,ppu,256,0,0,0,150000,&out)==GK83_INVALID);
 CHECK(gk98_trace_reset_controller_hypothesis("/missing/original.nes",ppu,256,0,0,0,150000,&out)==GK83_SOURCE_REFUSED);
 CHECK(gk98_trace_reset_controller_hypothesis("/missing/original.nes",ppu,5,0,0,0,150000,&out)==GK83_INVALID);
 CHECK(gk98_trace_reset_controller_hypothesis("/missing/original.nes",ppu,256,0,0,1,0,&out)==GK83_INVALID);
 CHECK(memcmp(&out,&initial,sizeof out)==0);
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(!rom||!*rom){puts("GK98 private source absent: serial-pad and fail-closed controls PASS");return 0;}
 Gk83Stop stop=gk98_trace_reset_controller_hypothesis(rom,ppu,256,0u,0u,0u,150000,&out);
 CHECK(stop!=GK83_SOURCE_REFUSED&&stop!=GK83_INVALID);
 CHECK(out.hypothetical_nmi_injections==1u);
 CHECK(out.hypothetical_nmi_vector==0xc0d9u);
 CHECK(out.hypothetical_controller_reads>0u);
 CHECK(out.hypothetical_controller2_reads==8u);
 CHECK(out.hypothetical_controller_reads==8u);
 CHECK(out.hypothetical_controller_writes==4u);
 CHECK(out.last_hardware_read==0x4017u);
 CHECK(stop==GK83_BUDGET_GATE); /* Repeated RAM flag wait: second NMI not simulated. */
 Gk83Trace replay={0};
 CHECK(gk98_trace_reset_controller_hypothesis(rom,ppu,256,0u,0u,0u,150000,&replay)==stop);
 CHECK(memcmp(&out,&replay,sizeof out)==0);
 Gk83Trace start={0};
 Gk83Stop start_stop=gk98_trace_reset_controller_hypothesis(rom,ppu,256,0x08u,0u,0u,150000,&start);
 CHECK(start_stop!=GK83_SOURCE_REFUSED&&start_stop!=GK83_INVALID);
 CHECK(start.hypothetical_controller_reads==8u);
 CHECK(start.hypothetical_controller2_reads==8u);
 CHECK(start.last_hardware_read==0x4017u);
 printf("GK98 COUNTERFACTUAL SOURCE: neutral stop=%u PC=%04X I=%u next=$%04X $4016 reads=%u P2=%u strobe writes=%u; START stop=%u PC=%04X I=%u reads=%u (NOT Mesen source timing or original setup parity)\n",
  stop,out.stop_pc,out.instructions,out.last_hardware_read,out.hypothetical_controller_reads,out.hypothetical_controller2_reads,out.hypothetical_controller_writes,
  start_stop,start.stop_pc,start.instructions,start.hypothetical_controller_reads);
 return 0;
}
