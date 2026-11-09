#include "gk109_rmw_families.h"
#include "gk100_cpu_cycles.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdint.h>
int main(void){
 const uint8_t families[6][4]={
  {0x06,0x16,0x0e,0x1e},{0x46,0x56,0x4e,0x5e},
  {0x26,0x36,0x2e,0x3e},{0x66,0x76,0x6e,0x7e},
  {0xc6,0xd6,0xce,0xde},{0xe6,0xf6,0xee,0xfe}};
 unsigned count=0;
 for(unsigned op=0;op<256u;op++){
  int expected=0;
  for(unsigned f=0;f<6u;f++)for(unsigned mode=0;mode<4u;mode++)
   if(op==families[f][mode])expected=1;
  Gk109Spec spec={GK109_INC,GK109_ABSX,99};Gk109Spec unchanged=spec;
  int found=gk109_rmw_decode((uint8_t)op,&spec);
  assert(found==expected);
  if(!found){assert(memcmp(&spec,&unchanged,sizeof spec)==0);continue;}
  count++;
  assert(spec.kind>=GK109_ASL&&spec.kind<=GK109_INC);
  assert(spec.mode<=GK109_ABSX);
  assert(spec.cycles==gk100_cpu_cycles((uint8_t)op,0,0));
  assert(spec.cycles==(spec.mode==GK109_ZP?5u:spec.mode==GK109_ABSX?7u:6u));
 }
 assert(count==24u);
 /* Exhaustively crosscheck six documented transformations, both carries,
  * 256 old values. Ensure INC/DEC do not overwrite the incoming carry. */
 for(unsigned kind=1;kind<=6u;kind++)for(unsigned old=0;old<256u;old++)
 for(unsigned carry=0;carry<2u;carry++){
  Gk109Result r={0};
  assert(gk109_rmw_compute((Gk109Kind)kind,(uint8_t)old,(uint8_t)carry,&r));
  uint8_t expected=0,cout=(uint8_t)carry;
  switch(kind){
   case GK109_ASL:expected=(uint8_t)(old*2u);cout=(uint8_t)(old>>7u);break;
   case GK109_LSR:expected=(uint8_t)(old/2u);cout=(uint8_t)(old&1u);break;
   case GK109_ROL:expected=(uint8_t)((old*2u+carry)&255u);cout=(uint8_t)(old>>7u);break;
   case GK109_ROR:expected=(uint8_t)((old/2u)+(carry?128u:0u));cout=(uint8_t)(old&1u);break;
   case GK109_DEC:expected=(uint8_t)(old-1u);break;
   case GK109_INC:expected=(uint8_t)(old+1u);break;
  }
  assert(r.value==expected&&r.carry==cout);
  assert(r.z==(expected==0u));assert(r.n==((expected>>7u)&1u));
 }
 Gk109Result bad={12,34,56,78},copy=bad;
 assert(!gk109_rmw_compute(GK109_NONE,10,0,&bad));
 assert(!gk109_rmw_compute((Gk109Kind)7,10,0,&bad));
 assert(!gk109_rmw_compute(GK109_ASL,10,2,&bad));
 assert(!gk109_rmw_compute(GK109_ASL,10,0,NULL));
 assert(memcmp(&bad,&copy,sizeof bad)==0);
 assert(!gk109_rmw_decode(0x0a,NULL));
 /* Zero-page,X addressing wraps in uint8_t; absolute,X does not. */
 assert((uint8_t)(0xfeu+3u)==1u);
 assert((uint16_t)(0x12feu+3u)==0x1301u);
 return 0;
}
