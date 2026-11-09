#include "gk132_f5_setup.h"
#include "gk82_stock_cartridge.h"
#include <stdio.h>
#include <string.h>
#define GK132_NES_BYTES 262160u
#define GK132_LAST_MONGOL_PRG_BANK14 (16u+14u*16384u)
/* A control-flow fingerprint, NOT a bundled portion of the copyrighted ROM.
 * All byte tests are instruction opcode, operand, target or branch-offset
 * checks against the user's separately authenticated 262160B original ROM.
 * $8050: LDA $00B8; BNE $805F; LDA $00BA; BNE $805F;
 *        LDA #$02; STA $00F5; RTS
 * $8060: LDA $00F5
 * These are CONDITIONAL source possibilities, not authenticated runtime
 * setup history or source-known CPU RAM values. */
Gk132Result gk132_verify_conditional_f5_writer(const char *private_rom,
 Gk132SourceFacts *out){
 if(!private_rom||!*private_rom||!out)return GK132_BAD;
 Gk82RomBootstrap boot={0};
 if(gk82_probe_stock_rom(private_rom,&boot)!=GK82_ROM_OK)return GK132_SOURCE_REFUSED;
 FILE *f=fopen(private_rom,"rb");if(!f)return GK132_SOURCE_REFUSED;
 uint8_t bytes[20];
 if(fseek(f,(long)(GK132_LAST_MONGOL_PRG_BANK14+0x50u),SEEK_SET)!=0){fclose(f);return GK132_SOURCE_REFUSED;}
 size_t n=fread(bytes,1,sizeof bytes,f);
 int bad=ferror(f)||fclose(f)!=0;
 if(bad||n!=sizeof bytes)return GK132_SOURCE_REFUSED;
 /* Decode complete conditional routine without embedding program bytes. */
 int match= bytes[0]==0xadu && bytes[1]==0xb8u && bytes[2]==0x00u &&
  bytes[3]==0xd0u && (uint8_t)(0x8055u+(int8_t)bytes[4]-0x805fu)==0u &&
  bytes[5]==0xadu && bytes[6]==0xbau && bytes[7]==0x00u &&
  bytes[8]==0xd0u && (uint8_t)(0x805au+(int8_t)bytes[9]-0x805fu)==0u &&
  bytes[10]==0xa9u && bytes[11]==2u &&
  bytes[12]==0x8du && bytes[13]==0xf5u && bytes[14]==0x00u &&
  bytes[15]==0x60u &&
  bytes[16]==0xadu && bytes[17]==0xf5u && bytes[18]==0x00u;
 if(!match)return GK132_SOURCE_CHANGED;
 Gk132SourceFacts fact;memset(&fact,0,sizeof fact);
 fact.entry_pc=0x8050u;fact.next_read_pc=0x8060u;
 fact.b8_addr=0xb8u;fact.ba_addr=0xbau;fact.f5_addr=0xf5u;
 fact.possible_writer_pc=0x805cu;fact.possible_writer_value=2u;
 fact.control_is_unproven=1u;fact.first_spring_playable=0u;
 memcpy(out,&fact,sizeof fact);return GK132_OK;
}
Gk132ConditionalResult gk132_conditional_if_called(uint8_t b8_known,
 uint8_t b8,uint8_t ba_known,uint8_t ba){
 if(b8_known!=1u)return GK132_F5_UNDETERMINED;
 if(b8!=0u)return GK132_F5_WOULD_NOT_WRITE;
 if(ba_known!=1u)return GK132_F5_UNDETERMINED;
 if(ba!=0u)return GK132_F5_WOULD_NOT_WRITE;
 return GK132_F5_WOULD_WRITE_TWO;
}
