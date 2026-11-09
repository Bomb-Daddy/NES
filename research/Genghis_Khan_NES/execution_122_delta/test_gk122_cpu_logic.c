#include "gk122_cpu_logic.h"
#include "gk100_cpu_cycles.h"
#include <stdint.h>
#include <stdio.h>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

static const uint8_t family[3][8]={
 {0x09,0x05,0x15,0x0d,0x1d,0x19,0x01,0x11},
 {0x29,0x25,0x35,0x2d,0x3d,0x39,0x21,0x31},
 {0x49,0x45,0x55,0x4d,0x5d,0x59,0x41,0x51}
};
static uint8_t expected(uint8_t kind,uint8_t a,uint8_t b){
 if(kind==0)return (uint8_t)(a|b);
 if(kind==1)return (uint8_t)(a&b);
 return (uint8_t)(a^b);
}
int main(void){
 unsigned i,k,m;
 uint32_t decoded=0;
 unsigned long long combinations=0;
 for(i=0;i<256;i++){
  Gk122Logic logic={GK122_EOR,GK122_INDY};
  int found=gk122_decode((uint8_t)i,&logic),want=0;
  for(k=0;k<3;k++)for(m=0;m<8;m++)
   if(family[k][m]==i){want=1;assert(found);assert(logic.kind==(Gk122LogicKind)k);assert(logic.mode==(Gk122LogicMode)m);decoded++;}
  assert(found==want);
 }
 assert(!gk122_decode(0x09,NULL));
 assert(decoded==24u);
 for(k=0;k<3;k++)for(m=0;m<8;m++){
  uint8_t op=family[k][m];
  for(i=0;i<2;i++){
   uint8_t actual=gk100_cpu_cycles(op,0,(uint8_t)i);
   uint8_t basis=(uint8_t)(m==0?2:m==1?3:m==2?4:m==3?4:m==4||m==5?4:m==6?6:5);
   assert(actual==(uint8_t)(basis+((m==4||m==5||m==7)?i:0u)));
  }
 }
 for(k=0;k<3;k++)for(unsigned a=0;a<256u;a++)for(unsigned b=0;b<256u;b++){
  uint8_t val=gk122_result((Gk122LogicKind)k,(uint8_t)a,(uint8_t)b);
  assert(val==expected((uint8_t)k,(uint8_t)a,(uint8_t)b));
  for(unsigned preset=0;preset<4u;preset++){
   uint8_t flags=(uint8_t)(0x3du|((preset&1u)?0x80u:0u)|((preset&2u)?0x02u:0u));
   uint8_t next=gk122_flags(flags,val);
   assert((next&0x82u)==(uint8_t)((val&0x80u)|(val?0:0x02u)));
   assert((next&0x7du)==(flags&0x7du));
   combinations++;
  }
 }
 /* Exact zero-page wrap and 16-bit address wrap; no false 0x0100 read. */
 for(unsigned b=0;b<256u;b++)for(unsigned x=0;x<256u;x++)
  assert(gk122_zpx((uint8_t)b,(uint8_t)x)==(uint8_t)(b+x));
 assert(gk122_indx(0xff,0x80)==0x80ffu);
 assert(gk122_indy(0xfe,0x12,0x04)==0x1302u);
 assert(gk122_indy(0xff,0xff,1)==0x0000u);
 assert(gk100_page_crossed(0x12ffu,gk122_indy(0xff,0x12,1u)));
 assert(!gk100_page_crossed(0x1204u,gk122_indy(0x03,0x12,1u)));
 printf("GK122 PASS: %u official ORA/AND/EOR 2A03 opcodes, %llu ALU/flag comparisons and 65536 zero-page wraps. No first Spring, PPU or ROM parity claimed.\n",decoded,combinations);
 return 0;
}
