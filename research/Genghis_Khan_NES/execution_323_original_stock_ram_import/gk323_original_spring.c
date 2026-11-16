#include "gk323_original_spring.h"
#include <string.h>
static uint16_t u16le(const uint8_t *p,size_t off){return (uint16_t)((unsigned)p[off]|((unsigned)p[off+1]<<8));}
Gk323Status gk323_decode(const char *rom_sha,Gk323World world,
 Gk323Condition condition,Gk323Bank bank,unsigned year,
 unsigned season,unsigned source_orders,
 const uint8_t *records,size_t length,Gk323Spring *output){
 if(!rom_sha||!records||!output)return GK323_ARGUMENT;
 /* strncmp bounds the read to 65 bytes, even for hostile unterminated text. */
 if(strncmp(rom_sha,GK323_ROM_SHA256,65)!=0)return GK323_SOURCE_MISMATCH;
 unsigned count=world==GK323_MONGOL?14u:world==GK323_WORLD?27u:0u;
 if(!count)return GK323_UNSUPPORTED;
 if((condition!=GK323_FRESH_RESET&&condition!=GK323_ARCHIVED_RESTORED)||
    (bank!=GK323_WORK_RAM&&bank!=GK323_SAVE_RAM))return GK323_UNSUPPORTED;
 if(condition==GK323_FRESH_RESET &&
    (world!=GK323_MONGOL||bank!=GK323_WORK_RAM))return GK323_SOURCE_MISMATCH;
 if(condition==GK323_ARCHIVED_RESTORED && bank!=GK323_SAVE_RAM)return GK323_SOURCE_MISMATCH;
 if(year!=(world==GK323_MONGOL?1175u:1206u)||season!=0u||source_orders!=3u)
  return GK323_SOURCE_MISMATCH;
 if(length!=count*GK323_RECORD_BYTES)return GK323_LENGTH;
 Gk323Spring candidate={0};
 candidate.world=(uint8_t)world;candidate.condition=(uint8_t)condition;
 candidate.source_bank=(uint8_t)bank;candidate.country_count=(uint8_t)count;
 candidate.year=(uint16_t)year;candidate.season=(uint8_t)season;
 candidate.source_orders=(uint8_t)source_orders;
 for(unsigned i=0;i<count;i++){
  const uint8_t *p=records+GK323_RECORD_BYTES*i;
  Gk323Country *c=&candidate.countries[i];
  c->arms=u16le(p,0x14);c->skill=u16le(p,0x16);
  c->troops=u16le(p,0x18);c->economy=u16le(p,0x1a);
  c->town=u16le(p,0x1c);c->defense=u16le(p,0x1e);
  c->masons=u16le(p,0x20);
  /* Correct original traded goods begin at +0x22, NOT +0x24. */
  for(unsigned j=0;j<10;j++)c->goods[j]=u16le(p,0x22+2*j);
  c->food=u16le(p,0x36);c->artisans=u16le(p,0x3a);
  c->food_workers=u16le(p,0x3e);c->gold=u16le(p,0x40);
  c->morale=u16le(p,0x42);c->tax=u16le(p,0x44);
  c->extra_tax_used=u16le(p,0x46);
  c->undrafted_residents=u16le(p,0x49);
  if(c->arms>999||c->skill>999||c->troops>999||c->economy>999||
     c->town>999||c->defense>999||c->masons>999||c->food>999||
     c->artisans>999||c->food_workers>999||c->gold>999||
     c->morale>999||c->tax>100||c->extra_tax_used>1||
     c->undrafted_residents>999)return GK323_FIELD;
  for(unsigned j=0;j<10;j++)if(c->goods[j]>999)return GK323_FIELD;
 }
 *output=candidate;
 return GK323_OK;
}
