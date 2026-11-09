#include "gk133_boot_order.h"
#include "gk82_stock_cartridge.h"
#include "gk131_boot_gate.h"
#include <stdio.h>
#include <string.h>

#define GK133_PRG_BANK14 (16ul+14ul*16384ul)
/* Only CPU opcodes and addresses, never copied PRG/CHR data. The cartridge
 * hash, board and reset vector are authenticated in GK82 first. */
Gk133BootStatus gk133_verify_boot_f5_order(const char *rom,
 Gk133BootOrder *out){
 if(!rom||!*rom||!out)return GK133_BOOT_ARGUMENT;
 Gk82RomBootstrap boot={0};
 if(gk82_probe_stock_rom(rom,&boot)!=GK82_ROM_OK)
  return GK133_BOOT_ROM_REFUSED;
 FILE *f=fopen(rom,"rb");if(!f)return GK133_BOOT_ROM_REFUSED;
 unsigned char entry[3],clear[6],consumer[3];
 int bad=0;
 if(fseek(f,(long)GK133_PRG_BANK14,SEEK_SET)!=0||
    fread(entry,1,sizeof entry,f)!=sizeof entry||
    fseek(f,(long)(GK133_PRG_BANK14+0x40u),SEEK_SET)!=0||
    fread(clear,1,sizeof clear,f)!=sizeof clear||
    fseek(f,(long)(GK133_PRG_BANK14+0x60u),SEEK_SET)!=0||
    fread(consumer,1,sizeof consumer,f)!=sizeof consumer||
    ferror(f))bad=1;
 if(fclose(f)!=0)bad=1;
 if(bad)return GK133_BOOT_ROM_REFUSED;
 if(entry[0]!=0x20u||entry[1]!=0x60u||entry[2]!=0x80u||
    clear[0]!=0x8du||clear[1]!=0xf5u||clear[2]!=0u||
    clear[3]!=0x8du||clear[4]!=0xf6u||clear[5]!=0u||
    consumer[0]!=0xadu||consumer[1]!=0xf5u||consumer[2]!=0u)
  return GK133_BOOT_SOURCE_DIVERGED;
 Gk131BootAudit actual={0};
 if(gk131_compare_boot_memory_profiles(rom,&actual)!=GK131_AUDIT_OK)
  return GK133_BOOT_SOURCE_DIVERGED;
 const Gk83Trace *unknown=&actual.unknown_power_on;
 const Gk83Trace *zero=&actual.archived_mesen_allzeros;
 /* Source execution reaches $8000 and JSR $8060 before the later STA
  * $00F5 at $8040. With uninitialized power-on WorkRAM, this is a real
  * missing input; it cannot be repaired by pretending the later STA ran.
  * The specifically recorded Mesen RamPowerOnState=1 AllZeros profile
  * reaches a later $8040 write and then stops at unknown PPUSTATUS. */
 if(unknown->first_banked_pc!=0x8000u||
    unknown->stop_pc!=0x8060u||unknown->first_unknown_ram_read!=0xf5u||
    unknown->source_f5_writes!=0u||unknown->instructions!=70u||
    zero->first_banked_pc!=0x8000u||
    zero->source_f5_known_reads==0u||zero->source_f5_writes!=1u||
    zero->source_f5_last_writer_pc!=0x8040u||
    zero->stop_pc!=0xc5beu||zero->first_hardware_read!=0x2002u||
    zero->instructions!=142u)return GK133_BOOT_SOURCE_DIVERGED;
 Gk133BootOrder next={0};
 next.first_banked_pc=0x8000u;next.banked_jsr_target=0x8060u;
 next.first_f5_read_pc=0x8060u;next.later_f5_clear_pc=0x8040u;
 next.subsequent_ppu_gate_pc=0xc5beu;
 next.subsequent_ppu_gate_register=0x2002u;
 next.unknown_ram_instructions=unknown->instructions;
 next.mesen_allzeros_instructions=zero->instructions;
 next.unseeded_f5_writes=0u;next.mesen_allzeros_f5_writes=1u;
 next.power_on_ram_required=1u;next.stock_first_spring_ready=0u;
 *out=next;return GK133_BOOT_OK;
}
