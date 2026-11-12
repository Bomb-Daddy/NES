#include "gk207_seed_write_watch.h"
#include <limits.h>
Gk207Status gk207_note_completed_seed_store(Gk207SeedWatch *watch,
 uint16_t pc,uint16_t address,uint8_t value,
 uint8_t bank_known,uint8_t bank,uint64_t cycle){
 if(!watch||pc<0x8000u||bank_known>1u||bank>15u||!cycle)
  return GK207_INVALID;
 if(address>=0x2000u || (address&0x07ffu)<0x0084u ||
    (address&0x07ffu)>0x0089u)return GK207_OUTSIDE;
 if(watch->completed_writes==UINT32_MAX ||
    (watch->completed_writes>=GK207_MAX_STORED_EVENTS &&
     watch->dropped_details==UINT32_MAX))return GK207_INVALID;
 const uint8_t index=(uint8_t)((address&0x07ffu)-0x84u);
 const Gk207SeedEvent event={pc,address,index,value,bank_known,
                             bank_known?bank:0xffu,cycle};
 if(watch->completed_writes<GK207_MAX_STORED_EVENTS)
  watch->first[watch->completed_writes]=event;
 else watch->dropped_details++;
 watch->completed_writes++;
 watch->latest_written[index]=value;
 watch->written_mask=(uint8_t)(watch->written_mask|(1u<<index));
 watch->last=event;
 return GK207_RECORDED;
}
int gk207_source_written_complete(const Gk207SeedWatch *w){
 return w && w->written_mask==GK207_FULL_SIX_WRITES;
}
