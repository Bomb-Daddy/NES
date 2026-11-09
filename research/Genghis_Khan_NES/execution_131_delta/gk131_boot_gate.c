#include "gk131_boot_gate.h"
#include "gk91_reset_zero.h"
#include <string.h>
/* This executable source gate compares two *different* initial-RAM premises,
 * rather than seeding one hypothetical $F5 byte into the universal boot.
 * MesenCE archived RamPowerOnState=1 is an explicit AllZeros configuration;
 * it says nothing about arbitrary NES power-on RAM or independent Spring RNG.
 */
Gk131AuditResult gk131_compare_boot_memory_profiles(const char *private_rom,
 Gk131BootAudit *out){
 if(!private_rom||!*private_rom||!out)return GK131_AUDIT_BAD_ARGUMENT;
 Gk131BootAudit candidate={0};
 Gk83Stop unknown=gk83_trace_original_reset(private_rom,20000u,
                                                   &candidate.unknown_power_on);
 if(unknown==GK83_SOURCE_REFUSED||unknown==GK83_INVALID)return GK131_AUDIT_ROM_REFUSED;
 Gk83Stop zero=gk91_trace_reset_mesen_allzeros(private_rom,20000u,
                                                   &candidate.archived_mesen_allzeros);
 if(zero==GK83_SOURCE_REFUSED||zero==GK83_INVALID)return GK131_AUDIT_ROM_REFUSED;
 const Gk83Trace *u=&candidate.unknown_power_on,*z=&candidate.archived_mesen_allzeros;
 if(unknown!=GK83_UNKNOWN_RAM_GATE||u->instructions!=70u||
    u->stop_pc!=0x8060u||u->stop_opcode!=0xadu||
    u->first_unknown_ram_read!=0x00f5u||
    u->first_unknown_ram_source_pc!=0x8060u||
    u->first_unknown_ram_source_opcode!=0xadu||
    u->first_unknown_ram_mmc1_bank!=14u||
    u->source_f5_writes!=0u||u->source_f5_known_reads!=0u||
    zero!=GK83_HARDWARE_GATE||z->instructions!=142u||
    z->stop_pc!=0xc5beu||z->first_hardware_read!=0x2002u||
    z->first_unknown_ram_read!=0u||z->source_f5_known_reads==0u)
   return GK131_AUDIT_SOURCE_DIVERGED;
 candidate.stock_verified=1u;
 candidate.allzeros_is_explicit_archived_config=1u;
 candidate.first_spring_playable=0u;
 *out=candidate;
 return GK131_AUDIT_OK;
}
