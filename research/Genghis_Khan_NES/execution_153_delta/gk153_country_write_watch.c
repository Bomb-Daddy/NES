#include "gk153_country_write_watch.h"
#include <limits.h>
Gk153WatchResult gk153_note_executed_write(Gk153CountryWatch *watch,
 uint16_t address,uint16_t pc){
 if(!watch)return GK153_INVALID;
 if(address<GK153_COUNTRY_BASE||address>=GK153_WORLD_END)
  return GK153_OUTSIDE;
 if(watch->world_region_writes==UINT32_MAX ||
    (address<GK153_MONGOL_END && watch->mongol_region_writes==UINT32_MAX))
  return GK153_INVALID;
 Gk153CountryWatch next=*watch;
 if(!next.world_region_writes){
  next.first_cpu_address=address;
  next.first_source_pc=pc;
  next.first_country_index=(uint8_t)((address-GK153_COUNTRY_BASE)/GK153_COUNTRY_STRIDE+1u);
 }
 next.world_region_writes++;
 if(address<GK153_MONGOL_END)next.mongol_region_writes++;
 next.last_cpu_address=address;
 next.last_source_pc=pc;
 next.last_country_index=(uint8_t)((address-GK153_COUNTRY_BASE)/GK153_COUNTRY_STRIDE+1u);
 *watch=next;
 return GK153_RECORDED;
}
