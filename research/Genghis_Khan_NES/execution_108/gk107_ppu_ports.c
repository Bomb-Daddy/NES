#include "gk107_ppu_ports.h"
Gk107PpuPortResult gk107_ppu_read_write_only(const Gk106PpuBus *bus,
 uint16_t address,uint64_t cpu_cycle,uint64_t last_full_drive_cycle,
 uint8_t *out){
 if(!bus||!out||address<0x2000u||address>=0x4000u)
  return GK107_PORT_INVALID;
 switch(address&7u){
  case 0u:case 1u:case 3u:case 5u:case 6u:break;
  default:return GK107_PORT_INVALID;
 }
 if(!bus->known)return GK107_PORT_UNKNOWN;
 if(cpu_cycle<last_full_drive_cycle||
    cpu_cycle-last_full_drive_cycle>GK107_PPU_BUS_MAX_FRESH_CYCLES)
  return GK107_PORT_STALE;
 *out=bus->value;
 return GK107_PORT_OK;
}
