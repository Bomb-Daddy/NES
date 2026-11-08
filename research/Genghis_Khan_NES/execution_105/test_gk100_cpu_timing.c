#include "gk100_cpu_cycles.h"
#include "gk97_nmi_trace.h"
#include "gk92_ppustatus_hypothesis.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK100 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 /* Table exhaustiveness and side-effect-free deterministic CPU timing. */
 const uint8_t branch[]={0xb0,0x90,0xd0,0xf0,0x10,0x30};
 const uint8_t indexed[]={0xbc,0xbd,0xb9,0xb1,0xd1,0xf1,0x71};
 for(size_t j=0;j<sizeof branch;j++){
  CHECK(gk100_cpu_cycles(branch[j],0,0)==2u);
  CHECK(gk100_cpu_cycles(branch[j],0,1)==2u);
  CHECK(gk100_cpu_cycles(branch[j],1,0)==3u);
  CHECK(gk100_cpu_cycles(branch[j],1,1)==4u);
 }
 for(size_t j=0;j<sizeof indexed;j++){
  uint8_t b=(j<3)?4u:5u;
  CHECK(gk100_cpu_cycles(indexed[j],0,0)==b);
  CHECK(gk100_cpu_cycles(indexed[j],0,1)==(uint8_t)(b+1u));
 }
 CHECK(gk100_cpu_cycles(0x91,0,1)==6u); /* STA indirect Y: NO page-cross penalty */
 CHECK(gk100_cpu_cycles(0x9d,0,1)==5u); /* STA absolute X: NO page-cross penalty */
 CHECK(gk100_cpu_cycles(0x7e,0,1)==7u); /* ROR abs X always seven cycles */
 CHECK(gk100_cpu_cycles(0x20,0,0)==6u); /* JSR */
 CHECK(gk100_cpu_cycles(0x40,0,0)==6u); /* RTI */
 CHECK(gk100_cpu_cycles(0x60,0,0)==6u); /* RTS */
 CHECK(gk100_cpu_cycles(0x00,0,0)==0u); /* BRK remains gated */
 CHECK(gk100_cpu_cycles(0xff,0,0)==0u); /* unknown source opcode remains gated */
 for(unsigned p=0;p<65536u;p+=251u){
  CHECK(gk100_page_crossed((uint16_t)p,(uint16_t)(p+1u))==((p&255u)==255u));
  CHECK(gk100_page_crossed((uint16_t)p,(uint16_t)p)==0u);
 }
 /* Authenticated original ROM source trace, WITHOUT scheduling a fake NMI.
  * Public trace has only opcode/cycle statistics and PPUCTRL write intent.
  * No private RAM or copyrighted ROM bytes are serialized. */
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(!rom||!*rom){puts("GK100 CPU cycle table PASS; original ROM optional, private source trace not run");return 0;}
 uint8_t status_reads[256];memset(status_reads,0x80,sizeof status_reads);
 Gk83Trace pre={0},nmi={0},replay={0};
 Gk83Stop gate=gk92_trace_reset_ppustatus_hypothesis(rom,status_reads,256,3000,&pre);
 CHECK(gate!=GK83_INVALID&&gate!=GK83_SOURCE_REFUSED);
 CHECK(pre.completed_cpu_cycles>pre.instructions*2u);
 CHECK(pre.ppuctrl_write_attempts>0u);
 CHECK(pre.ppuctrl_nmi_enable_attempts>0u);
 CHECK(pre.last_ppuctrl_source_pc!=0u);
 CHECK(pre.hypothetical_nmi_injections==0u);
 Gk83Stop ns=gk97_trace_reset_one_hypothetical_nmi(rom,status_reads,256,30000,&nmi);
 CHECK(ns==GK83_HARDWARE_GATE&&nmi.hypothetical_nmi_injections==1u);
 CHECK(nmi.hypothetical_nmi_vector==0xc0d9u);
 CHECK(nmi.ppuctrl_write_attempts>=pre.ppuctrl_write_attempts);
 CHECK(nmi.ppuctrl_nmi_requested_at_injection==1u);
 CHECK(nmi.nmi_requested_after_completed_cycles>0u);
 CHECK(nmi.completed_cpu_cycles>=nmi.nmi_requested_after_completed_cycles+7u);
 CHECK(nmi.completed_cpu_cycles>nmi.instructions*2u);
 CHECK(gk97_trace_reset_one_hypothetical_nmi(rom,status_reads,256,30000,&replay)==ns);
 CHECK(memcmp(&replay,&nmi,sizeof replay)==0);
 printf("GK100 VERIFIED_STATIC: source PPUCTRL write attempts=%u enable attempts=%u last=$%02X at $%04X; hypothetical NMI injection at completed-cycles=%llu (NO PPU CLOCK SIMULATION); ended gate=%u instruction=%u\n",
 nmi.ppuctrl_write_attempts,nmi.ppuctrl_nmi_enable_attempts,nmi.last_ppuctrl_value,nmi.last_ppuctrl_source_pc,
 (unsigned long long)nmi.nmi_requested_after_completed_cycles,ns,nmi.instructions);
 return 0;
}
