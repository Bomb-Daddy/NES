#include "gk121_bit_flags.h"
uint8_t gk121_bit_status(uint8_t accumulator,uint8_t memory,uint8_t status){
  const uint8_t mask=0xc2u; /* N=0x80, V=0x40, Z=0x02 */
  uint8_t updates=(uint8_t)((memory&0xc0u)|((accumulator&memory)?0u:0x02u));
  return (uint8_t)((status&~mask)|updates);
}
