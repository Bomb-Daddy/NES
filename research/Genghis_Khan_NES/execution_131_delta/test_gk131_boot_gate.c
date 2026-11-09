#include "gk131_boot_gate.h"
#include "gk83_reset_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do{if(!(e)){fprintf(stderr,"GK131 FAILED %s:%u %s\n",__FILE__,(unsigned)__LINE__,#e);return 1;}}while(0)
int main(void){
 Gk131BootAudit result;memset(&result,0xa5,sizeof result);
 Gk131BootAudit before=result;
 CHECK(gk131_compare_boot_memory_profiles(NULL,&result)==GK131_AUDIT_BAD_ARGUMENT);
 CHECK(memcmp(&before,&result,sizeof result)==0);
 CHECK(gk131_compare_boot_memory_profiles("",&result)==GK131_AUDIT_BAD_ARGUMENT);
 CHECK(memcmp(&before,&result,sizeof result)==0);
 CHECK(gk131_compare_boot_memory_profiles("/invalid/private_stock_rom.nes",&result)==GK131_AUDIT_ROM_REFUSED);
 CHECK(memcmp(&before,&result,sizeof result)==0);
 const char *stock=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(stock&&*stock){
  CHECK(gk131_compare_boot_memory_profiles(stock,&result)==GK131_AUDIT_OK);
  CHECK(result.stock_verified==1u&&result.allzeros_is_explicit_archived_config==1u);
  CHECK(result.first_spring_playable==0u);
  CHECK(result.unknown_power_on.stop==GK83_UNKNOWN_RAM_GATE);
  CHECK(result.unknown_power_on.first_unknown_ram_source_pc==0x8060u);
  CHECK(result.unknown_power_on.first_unknown_ram_source_opcode==0xadu);
  CHECK(result.unknown_power_on.first_unknown_ram_mmc1_bank==14u);
  CHECK(result.unknown_power_on.source_f5_writes==0u);
  CHECK(result.archived_mesen_allzeros.stop==GK83_HARDWARE_GATE);
  CHECK(result.archived_mesen_allzeros.first_hardware_read==0x2002u);
  CHECK(result.archived_mesen_allzeros.source_f5_known_reads>0u);
  Gk131BootAudit repeat={0};
  CHECK(gk131_compare_boot_memory_profiles(stock,&repeat)==GK131_AUDIT_OK);
  CHECK(memcmp(&result,&repeat,sizeof result)==0);
  puts("GK131 PASS private stock RAM gate: no $F5 write before bank14 LDA $00F5 at $8060; Mesen AllZeros proceeds to $2002, not Spring");
 }else puts("GK131 PASS negative controls; optional stock identity/source run NOT RUN");
 return 0;
}
