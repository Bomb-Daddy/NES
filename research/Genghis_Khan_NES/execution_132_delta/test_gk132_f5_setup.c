#include "gk132_f5_setup.h"
#include "gk131_boot_gate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"GK132 FAILED %s:%d %s\n",__FILE__,__LINE__,#c);return 1;}}while(0)
int main(void){
 Gk132SourceFacts result;memset(&result,0xa5,sizeof result);
 Gk132SourceFacts original=result;
 CHECK(gk132_verify_conditional_f5_writer(NULL,&result)==GK132_BAD);
 CHECK(gk132_verify_conditional_f5_writer("",&result)==GK132_BAD);
 CHECK(gk132_verify_conditional_f5_writer("/unavailable/private/rom.nes",&result)==GK132_SOURCE_REFUSED);
 CHECK(memcmp(&result,&original,sizeof result)==0);
 for(unsigned b8=0;b8<256;b8++)for(unsigned ba=0;ba<256;ba++){
  CHECK(gk132_conditional_if_called(1,(uint8_t)b8,1,(uint8_t)ba)==
    (b8||ba?GK132_F5_WOULD_NOT_WRITE:GK132_F5_WOULD_WRITE_TWO));
  CHECK(gk132_conditional_if_called(0,(uint8_t)b8,1,(uint8_t)ba)==GK132_F5_UNDETERMINED);
  CHECK(gk132_conditional_if_called(1,0,0,(uint8_t)ba)==GK132_F5_UNDETERMINED);
 }
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  CHECK(gk132_verify_conditional_f5_writer(rom,&result)==GK132_OK);
  CHECK(result.entry_pc==0x8050u&&result.next_read_pc==0x8060u);
  CHECK(result.possible_writer_pc==0x805cu&&result.possible_writer_value==2u);
  CHECK(result.b8_addr==0xb8u&&result.ba_addr==0xbau&&result.f5_addr==0xf5u);
  CHECK(result.control_is_unproven==1u&&result.first_spring_playable==0u);
  Gk131BootAudit audit={0};CHECK(gk131_compare_boot_memory_profiles(rom,&audit)==GK131_AUDIT_OK);
  CHECK(audit.unknown_power_on.source_f5_writes==0u);
  CHECK(audit.unknown_power_on.first_unknown_ram_read==0xf5u);
  CHECK(audit.archived_mesen_allzeros.source_f5_known_reads>0u);
  Gk132SourceFacts repeat={0};CHECK(gk132_verify_conditional_f5_writer(rom,&repeat)==GK132_OK);
  CHECK(memcmp(&result,&repeat,sizeof result)==0);
  puts("GK132 PASS: authenticated stock bank14 conditional $00F5 writer; unknown-control is NOT first-Spring initialization");
 }else puts("GK132 PASS conditional exhaustive controls; optional authentic ROM source check NOT RUN");
 return 0;
}
