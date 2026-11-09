#include "gk121_bit_flags.h"
#include "gk100_cpu_cycles.h"
#include <stdint.h>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
/* Exhaustive 256 x 256 BIT pairs, with all four C/I combinations and two
 * overflow presets. Independent bit-level reference oracle and no mutation. */
int main(void){
 unsigned long long comparisons=0;
 for(unsigned a=0;a<256u;a++)for(unsigned m=0;m<256u;m++)
 for(unsigned ci=0;ci<4u;ci++)for(unsigned v=0;v<2u;v++){
  uint8_t before=(uint8_t)(0x20u|(ci&1u)|((ci&2u)?4u:0u)|
                              (v?0x40u:0u)|0x08u|0x10u|0x02u);
  uint8_t actual=gk121_bit_status((uint8_t)a,(uint8_t)m,before);
  uint8_t reference=before;
  if(a&m)reference&=(uint8_t)~2u;else reference|=2u;
  if(m&0x40u)reference|=0x40u;else reference&=(uint8_t)~0x40u;
  if(m&0x80u)reference|=0x80u;else reference&=(uint8_t)~0x80u;
  assert(actual==reference);
  assert((actual&0x3du)==(before&0x3du)); /* C,I,D,B,reserved */
  comparisons++;
 }
 for(unsigned m=0;m<256u;m++){
  uint8_t bit=gk121_bit_status(0u,(uint8_t)m,0x01u);
  assert((bit&2u)==2u && (bit&1u)==1u);
 }
 assert(gk100_cpu_cycles(0x24,0,0)==3u);
 assert(gk100_cpu_cycles(0x2c,0,0)==4u);
 assert(gk100_cpu_cycles(0xb8,0,0)==2u);
 for(unsigned op=0x50;op<=0x70;op+=0x20u){
  assert(gk100_cpu_cycles((uint8_t)op,0,0)==2u);
  assert(gk100_cpu_cycles((uint8_t)op,1,0)==3u);
  assert(gk100_cpu_cycles((uint8_t)op,1,1)==4u);
  assert(gk100_cpu_cycles((uint8_t)op,0,1)==2u);
 }
 assert(gk100_page_crossed(0x12feu,0x1301u));
 assert(!gk100_page_crossed(0x1201u,0x12feu));
 printf("GK121 PASS: %llu BIT status/flag cases, BIT zp/abs, CLV, BVC/BVS cycles and page crossings; original first Spring NOT claimed\n",comparisons);
 return 0;
}
