#include "gk133_boot_order.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"GK133 FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(void){
 Gk133BootOrder a;memset(&a,0x5a,sizeof a);Gk133BootOrder before=a;
 CHECK(gk133_verify_boot_f5_order(NULL,&a)==GK133_BOOT_ARGUMENT);
 CHECK(gk133_verify_boot_f5_order("",&a)==GK133_BOOT_ARGUMENT);
 CHECK(gk133_verify_boot_f5_order("/nonexistent/original_private.nes",&a)==GK133_BOOT_ROM_REFUSED);
 CHECK(!memcmp(&a,&before,sizeof a));
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(rom&&*rom){
  CHECK(gk133_verify_boot_f5_order(rom,&a)==GK133_BOOT_OK);
  CHECK(a.first_banked_pc==0x8000u&&a.banked_jsr_target==0x8060u);
  CHECK(a.first_f5_read_pc==0x8060u&&a.later_f5_clear_pc==0x8040u);
  CHECK(a.unknown_ram_instructions==70u&&a.mesen_allzeros_instructions==142u);
  CHECK(a.unseeded_f5_writes==0u&&a.mesen_allzeros_f5_writes==1u);
  CHECK(a.subsequent_ppu_gate_pc==0xc5beu&&a.subsequent_ppu_gate_register==0x2002u);
  CHECK(a.power_on_ram_required&&!a.stock_first_spring_ready);
  Gk133BootOrder repeat={0};CHECK(gk133_verify_boot_f5_order(rom,&repeat)==GK133_BOOT_OK);
  CHECK(memcmp(&a,&repeat,sizeof a)==0);
  puts("GK133 PASS authenticated source boot order: first F5 read precedes later F5 clear; Mesen AllZeros reaches unmodeled PPUSTATUS, no Spring unlock");
 }else puts("GK133 PASS invalid-source negative controls; private original runtime check NOT RUN");
 return 0;
}
