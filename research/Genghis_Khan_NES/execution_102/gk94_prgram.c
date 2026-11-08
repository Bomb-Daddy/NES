#include "gk94_prgram.h"
#include <string.h>
void gk94_prgram_reset_unknown(Gk94PrgRam *ram){
 if(ram)memset(ram,0,sizeof *ram);
}
Gk94RamResult gk94_prgram_read(const Gk94PrgRam *ram,uint16_t address,
                              uint8_t *value){
 if(!ram||!value||address<0x6000u||address>=0x8000u)return GK94_RAM_INVALID;
 unsigned at=(unsigned)address-0x6000u;
 if(!ram->known[at])return GK94_RAM_UNKNOWN;
 *value=ram->data[at];return GK94_RAM_OK;
}
Gk94RamResult gk94_prgram_write(Gk94PrgRam *ram,uint16_t address,uint8_t value){
 if(!ram||address<0x6000u||address>=0x8000u)return GK94_RAM_INVALID;
 unsigned at=(unsigned)address-0x6000u;
 ram->data[at]=value;ram->known[at]=1u;
 return GK94_RAM_OK;
}
Gk94RamResult gk94_prgram_seed_physical(Gk94PrgRam *ram,size_t at,
                                        const uint8_t *bytes,size_t count){
 if(!ram||!bytes||!count||at>=GK94_PRGRAM_BYTES||
    count>GK94_PRGRAM_BYTES-at)return GK94_RAM_INVALID;
 memcpy(ram->data+at,bytes,count);
 memset(ram->known+at,1,count);
 return GK94_RAM_OK;
}
