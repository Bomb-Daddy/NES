#include "gk144_archived_mapper.h"
#include <string.h>
#define MAX_STATE (16u*1024u*1024u)
#define NAMED_MAX 127u
#define PAGES 256u
#define PRG_PAGES 128u
#define PRG_SIZE 262144u
#define PRG_BANK 16384u
#define PAGE_BYTES 256u
static uint32_t read32(const uint8_t *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
/* An explicitly bounded decoder of MesenCE's CPU $8000-$FFFF per-256B
 * memory manager mapping. The exact array format is saved-state-specific. */
Gk144Status gk144_probe_archived_prg_map(const uint8_t *raw,size_t n,
 Gk144ArchivedMapper *out){
 if(!raw||!out||n<32u||n>MAX_STATE)return GK144_MAPPER_BAD_INPUT;
 Gk139MesenProof original;
 if(gk139_parse_uncompressed_state(raw,n,&original)!=GK139_STATE_OK)
  return GK144_MAPPER_STATE_REFUSED;
 const char *const names[]={"mapper.prgMemoryOffset","mapper.prgMemoryType",
  "mapper.prgMemoryAccess","mapper.prgMode","mapper.slotSelect","mapper.prgReg"};
 const uint8_t *field[6]={0};size_t sizes[6]={0};
 size_t p=0;
 while(p<n){
  size_t begin=p;
  while(p<n&&raw[p]){
   if(raw[p]<33u||raw[p]>126u||p-begin>=NAMED_MAX)
    return GK144_MAPPER_STATE_REFUSED;
   ++p;
  }
  if(p==n||p==begin||n-p<5u)return GK144_MAPPER_STATE_REFUSED;
  size_t len=p-begin;
  ++p;uint32_t count=read32(raw+p);p+=4;
  if((size_t)count>n-p)return GK144_MAPPER_STATE_REFUSED;
  for(unsigned k=0;k<6u;k++){
   if(len==strlen(names[k])&&memcmp(raw+begin,names[k],len)==0){
    if(field[k])return GK144_MAPPER_STATE_REFUSED;
    field[k]=raw+p;sizes[k]=count;
   }
  }
  p+=(size_t)count;
 }
 for(unsigned k=0;k<6u;k++){
  if(!field[k]||sizes[k]!=(k<3u?PAGES*4u:1u))
   return GK144_MAPPER_STATE_REFUSED;
 }
 if(field[3][0]>1u||field[4][0]>1u||field[5][0]>15u)
  return GK144_MAPPER_STATE_REFUSED;
 /* Other configurations may be valid NES states, but this proof only
  * establishes the exact 16 KiB switchable-low/fixed-high configuration. */
 if(field[3][0]!=1u||field[4][0]!=1u)
  return GK144_MAPPER_UNSUPPORTED_MODE;
 const uint8_t *offsets=field[0],*types=field[1],*access=field[2];
 uint32_t low_base=read32(offsets+0x80u*4u);
 uint32_t high_base=read32(offsets+0xc0u*4u);
 if(low_base%PRG_BANK||high_base%PRG_BANK||
    low_base>=PRG_SIZE||high_base>=PRG_SIZE)
  return GK144_MAPPER_CONTRADICTION;
 for(unsigned i=0;i<PRG_PAGES;i++){
  unsigned page=0x80u+i;
  uint32_t base=i<64u?low_base:high_base;
  uint32_t j=i%64u;
  uint32_t expected=base+j*PAGE_BYTES;
  if(expected>=PRG_SIZE||read32(offsets+4u*page)!=expected||
     read32(types+4u*page)!=0u||read32(access+4u*page)!=1u)
    return GK144_MAPPER_CONTRADICTION;
 }
 Gk144ArchivedMapper next={0};
 next.lower_prg_bank=(uint8_t)(low_base/PRG_BANK);
 next.upper_prg_bank=(uint8_t)(high_base/PRG_BANK);
 next.mapper_prg_reg=field[5][0];
 next.mapper_prg_mode=field[3][0];
 next.mapper_slot_select=field[4][0];
 next.saved_program_counter=original.program_counter;
 next.saved_cpu_cycles=original.cpu_cycles;
 next.observed_country_ram_bank=original.country_bank;
 next.confirmed_prg_pages=PRG_PAGES;
 next.upper_bank_is_fixed_last=(uint8_t)(next.upper_prg_bank==15u);
 next.mapping_observed_at_saved_instant_only=1u;
 /* This Mesen state claims the register selects the low 16 KiB bank and
  * fixes high to the final bank. A mismatch would be misleading evidence. */
 if(next.lower_prg_bank!=next.mapper_prg_reg||!next.upper_bank_is_fixed_last)
  return GK144_MAPPER_CONTRADICTION;
 *out=next;return GK144_MAPPER_OK;
}
