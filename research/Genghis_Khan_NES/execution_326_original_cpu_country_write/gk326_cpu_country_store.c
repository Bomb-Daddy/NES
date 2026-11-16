#include "gk326_cpu_country_store.h"
#include <string.h>
#include "gk326_sha256.h"

static uint16_t u16(uint8_t low,uint8_t high){return (uint16_t)((unsigned)low|((unsigned)high<<8));}
static uint16_t read_zp_pointer(const uint8_t zp[256],uint8_t base){
 return u16(zp[base],zp[(uint8_t)(base+1u)]);
}
Gk326Status gk326_apply_country_store(const uint8_t *rom,size_t size,
 const Gk326CpuSource *cpu,uint8_t ram[GK326_RAM_BYTES],Gk326StoreReceipt *receipt){
 if(!rom||!cpu||!ram||!receipt)return GK326_BAD_ARGUMENT;
 if(size!=GK326_ORIGINAL_ROM_SIZE||memcmp(rom,"NES\x1a",4)!=0||
    rom[4]!=16u||rom[5]!=0u||
    ((unsigned)(rom[6]>>4)|(unsigned)(rom[7]&0xf0u))!=1u||
    (rom[7]&0x0cu)==0x08u)return GK326_BAD_ROM;
 if((cpu->country_count!=14u&&cpu->country_count!=27u)||
    (cpu->condition!=GK326_FRESH_WORKRAM&&
     cpu->condition!=GK326_ARCHIVED_SAVERAM))return GK326_BAD_CONTEXT;
 if((cpu->pc>=0xc000u && cpu->bank!=15u)||
    (cpu->pc>=0x8000u&&cpu->pc<0xc000u&&cpu->bank>=15u)||
    cpu->pc<0x8000u)return GK326_WRONG_BANK;
 unsigned off=(unsigned)(cpu->pc&0x3fffu);
 unsigned bankstart=16u+(unsigned)cpu->bank*16384u;
 uint8_t op=rom[bankstart+off];
 if(off==0x3fffu)return GK326_BAD_CONTEXT; /* operand crosses bank boundary */
 uint8_t arg=rom[bankstart+off+1u];
 uint8_t value=cpu->a;
 uint16_t ea=0;
 switch(op){
 case 0x91: /* STA ($nn),Y: wraps ZP pointer high byte */
  ea=(uint16_t)(read_zp_pointer(cpu->zp,arg)+cpu->y);break;
 case 0x81: /* STA ($nn,X): wraps base and pointer high byte */
  ea=read_zp_pointer(cpu->zp,(uint8_t)(arg+cpu->x));break;
 case 0x8d:case 0x99:case 0x9d:case 0x8e:case 0x8c:
  if(off+2u>=0x4000u)return GK326_BAD_CONTEXT;
  ea=u16(arg,rom[bankstart+off+2u]);
  if(op==0x99)ea=(uint16_t)(ea+cpu->y);
  if(op==0x9d)ea=(uint16_t)(ea+cpu->x);
  if(op==0x8e)value=cpu->x;
  if(op==0x8c)value=cpu->y;
  break;
 default:return GK326_UNSUPPORTED_OPCODE;
 }
 unsigned base=GK326_COUNTRY_BASE;
 unsigned end=base+(unsigned)GK326_COUNTRY_BYTES*cpu->country_count;
 if((unsigned)ea<base||(unsigned)ea>=end)return GK326_NOT_COUNTRY;
 unsigned relative=(unsigned)ea-base;
 unsigned physical=(unsigned)ea-0x6000u;
 if(physical>=GK326_RAM_BYTES)return GK326_NOT_COUNTRY;
 Gk326StoreReceipt result={0};
 result.cpu_address=ea;result.physical_offset=(uint16_t)physical;
 result.country_one_based=(uint8_t)(relative/GK326_COUNTRY_BYTES+1u);
 result.offset_in_country=(uint8_t)(relative%GK326_COUNTRY_BYTES);
 result.opcode=op;result.old_value=ram[physical];result.new_value=value;
 result.backing=cpu->condition;result.source_address_decoded=1u;
 /* No side effects until all validation is complete. */
 ram[physical]=value;*receipt=result;return GK326_OK;
}

Gk326Status gk326_apply_stock_country_store(const uint8_t *rom,size_t size,
 const Gk326CpuSource *cpu,uint8_t ram[GK326_RAM_BYTES],Gk326StoreReceipt *out){
 static const uint8_t expected[32]={
  0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
  0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3};
 if(!rom||!cpu||!ram||!out)return GK326_BAD_ARGUMENT;
 if(size!=GK326_ORIGINAL_ROM_SIZE)return GK326_BAD_ROM;
 uint8_t digest[32];gk326_sha256(rom,size,digest);
 if(memcmp(expected,digest,32)!=0)return GK326_BAD_ROM;
 Gk326StoreReceipt temp;
 Gk326Status status=gk326_apply_country_store(rom,size,cpu,ram,&temp);
 if(status!=GK326_OK)return status;
 temp.original_rom_hash_verified=1u;
 *out=temp;return GK326_OK;
}
