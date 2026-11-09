#include "gk106_ppu_bus.h"
#include <limits.h>
#include <string.h>
void gk106_ppu_bus_reset(Gk106PpuBus *bus){if(bus)memset(bus,0,sizeof *bus);}
int gk106_ppu_bus_cpu_write(Gk106PpuBus *bus,uint8_t value){
 if(!bus||bus->cpu_writes==UINT_MAX)return 0;
 bus->value=value;bus->known=1u;bus->cpu_writes++;return 1;
}
int gk106_ppu_bus_low5(const Gk106PpuBus *bus,uint8_t *low5){
 if(!bus||!low5||!bus->known)return 0;
 *low5=(uint8_t)(bus->value&0x1fu);return 1;
}
int gk106_ppu_bus_status_read(Gk106PpuBus *bus,uint8_t high3,uint8_t *value){
 if(!bus||!value||!bus->known||bus->status_reads==UINT_MAX||(high3&0x1fu))return 0;
 uint8_t next=(uint8_t)((high3&0xe0u)|(bus->value&0x1fu));
 *value=next;bus->value=next;bus->status_reads++;return 1;
}
