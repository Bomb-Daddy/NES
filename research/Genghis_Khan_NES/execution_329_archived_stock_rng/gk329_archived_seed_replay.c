#include "gk329_archived_seed_replay.h"
#include "gk326_sha256.h"
#include "gk137_stock_rng.h"
#include <string.h>

/* SHA-256 of original private archived Mesen *uncompressed named fields*.
 * These public digests disclose no ROM, SRAM or user checkpoint bytes.
 * Source states: original MesenCE restored Spring1175 and Spring1206 World.
 * NOT the fresh WorkRAM boot sequence. */
static const uint8_t stock_field_digests[2][32]={
 {0x50,0x29,0xc4,0xed,0x3d,0x4c,0x1f,0x00,0x41,0xa2,0x51,0x5f,0xbf,0x7e,0x33,0xa4,
  0x68,0x7d,0xc5,0xc2,0x3c,0x87,0xd3,0xb5,0x9d,0xfc,0xa9,0x3d,0x28,0xdc,0x97,0x08},
 {0xf6,0x1c,0x5b,0xfb,0x91,0x05,0xe4,0xef,0xb1,0xaf,0xdf,0x00,0x07,0x36,0x38,0x09,
  0x47,0x76,0x31,0x62,0x26,0x1d,0x79,0x43,0x3c,0x0d,0xd2,0x54,0x71,0x09,0xb7,0x94}
};
static const size_t sizes[2]={40712u,40711u};
static const uint64_t cycles[2]={UINT64_C(107624250),UINT64_C(212275526)};
Gk329Status gk329_replay_archived_next_draw(
 uint8_t scenario_index,const uint8_t *original_rom,size_t rom_size,
 const uint8_t *archived_mesen_named_fields,size_t fields_size,
 Gk329Receipt *out){
 if(!out||!original_rom||!archived_mesen_named_fields||scenario_index>1u)
  return GK329_ARGUMENT;
 if(fields_size!=sizes[scenario_index])return GK329_NOT_ARCHIVED_SOURCE;
 uint8_t digest[32];gk326_sha256(archived_mesen_named_fields,fields_size,digest);
 if(memcmp(digest,stock_field_digests[scenario_index],32u)!=0)
  return GK329_NOT_ARCHIVED_SOURCE;
 Gk139MesenProof named={0};
 if(gk139_parse_uncompressed_state(archived_mesen_named_fields,fields_size,
      &named)!=GK139_STATE_OK)return GK329_BAD_NAMED_FIELDS;
 if(named.country_bank!=GK139_BANK_SAVE)return GK329_WRONG_RAM_BANK;
 if(named.program_counter!=0xefc9u||named.cpu_cycles!=cycles[scenario_index])
  return GK329_WRONG_CPU_BOUNDARY;
 Gk327RngReceipt executed={0};
 if(gk327_execute_original_rng(original_rom,rom_size,named.nes_seed,
     &executed)!=GK327_OK)return GK329_ROM_OR_CPU_REJECTED;
 Gk137StockRng math={0};uint16_t draw=0;uint8_t expected_next[6];
 if(gk137_from_nes_bytes(&math,named.nes_seed)!=GK137_OK||
    gk137_draw15(&math,&draw)!=GK137_OK)return GK329_SOURCE_PARITY_FAILURE;
 gk137_to_nes_bytes(&math,expected_next);
 if(memcmp(executed.seed_after,expected_next,6u)!=0||
    draw!=(uint16_t)((((uint16_t)executed.result_67<<8)|executed.result_66)&0x7fffu))
   return GK329_SOURCE_PARITY_FAILURE;
 Gk329Receipt candidate={0};
 candidate.archived=named;candidate.source_instruction_result=executed;
 candidate.source_draw15=draw;candidate.scenario_index=scenario_index;
 candidate.evidence_is_archived_restored=1u;
 *out=candidate;return GK329_OK;
}
