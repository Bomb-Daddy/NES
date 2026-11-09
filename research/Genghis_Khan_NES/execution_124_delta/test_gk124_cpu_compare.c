#include "gk124_cpu_compare.h"
#include "gk100_cpu_cycles.h"
#include "gk83_reset_trace.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef struct {uint8_t op;Gk124CompareReg reg;Gk124CompareMode mode;
 uint8_t no_page,page;} Spec;
static const Spec cases[]={
 {0xc9,GK124_CMP_A,GK124_IMM,2,2},
 {0xc5,GK124_CMP_A,GK124_ZP,3,3},
 {0xd5,GK124_CMP_A,GK124_ZPX,4,4},
 {0xcd,GK124_CMP_A,GK124_ABS,4,4},
 {0xdd,GK124_CMP_A,GK124_ABSX,4,5},
 {0xd9,GK124_CMP_A,GK124_ABSY,4,5},
 {0xc1,GK124_CMP_A,GK124_INDX,6,6},
 {0xd1,GK124_CMP_A,GK124_INDY,5,6},
 {0xe0,GK124_CPX_X,GK124_IMM,2,2},
 {0xe4,GK124_CPX_X,GK124_ZP,3,3},
 {0xec,GK124_CPX_X,GK124_ABS,4,4},
 {0xc0,GK124_CPY_Y,GK124_IMM,2,2},
 {0xc4,GK124_CPY_Y,GK124_ZP,3,3},
 {0xcc,GK124_CPY_Y,GK124_ABS,4,4}
};
int main(void){
 const unsigned n=(unsigned)(sizeof cases/sizeof cases[0]);
 assert(n==14u);
 for(unsigned op=0;op<256u;op++){
  const Spec *expect=0;
  for(unsigned i=0;i<n;i++)if(cases[i].op==op){assert(!expect);expect=&cases[i];}
  Gk124Compare decoded={GK124_CPY_Y,GK124_INDY};
  int recognized=gk124_compare_decode((uint8_t)op,&decoded);
  assert(recognized==(expect!=0));
  if(expect){
   assert(decoded.reg==expect->reg&&decoded.mode==expect->mode);
   assert(gk124_compare_cycles((uint8_t)op,0u)==expect->no_page);
   assert(gk124_compare_cycles((uint8_t)op,1u)==expect->page);
   assert(gk100_cpu_cycles((uint8_t)op,0u,0u)==expect->no_page);
   assert(gk100_cpu_cycles((uint8_t)op,0u,1u)==expect->page);
  }else{
   assert(gk124_compare_cycles((uint8_t)op,0u)==0u);
   assert(decoded.reg==GK124_CPY_Y&&decoded.mode==GK124_INDY);
  }
 }
 assert(!gk124_compare_decode(0xc9,0));
 /* Independent binary subtraction oracle: C means unsigned >=, N is
  * bit seven of the eight-bit difference, Z means equal. V, D, I and
  * all other bits are untouched. Exercise every old status value. */
 uint64_t checks=0u;
 for(unsigned reg=0;reg<256u;reg++)for(unsigned value=0;value<256u;value++){
  uint8_t diff=(uint8_t)((reg+256u-value)&0xffu);
  unsigned flags=(reg>=value?0x01u:0u)|(reg==value?0x02u:0u)|
   ((diff>=128u)?0x80u:0u);
  for(unsigned p=0;p<256u;p++){
   uint8_t expected=(uint8_t)((p&0x7cu)|flags);
   uint8_t actual=gk124_compare_flags((uint8_t)p,(uint8_t)reg,(uint8_t)value);
   assert(expected==actual);
   assert(((actual^p)&0x7cu)==0u);
   checks++;
  }
 }
 /* Known boundary probes, including wrapped negative subtraction. */
 assert((gk124_compare_flags(0,0,1)&0x83u)==0x80u);
 assert((gk124_compare_flags(0,255,1)&0x83u)==0x81u);
 assert((gk124_compare_flags(0,0,0)&0x83u)==0x03u);
 assert((gk124_compare_flags(0,127,255)&0x83u)==0x80u);
 assert((gk124_compare_flags(0,128,127)&0x83u)==0x01u);
 /* The actual source-gated interpreter refuses absent/unapproved ROM;
  * the test suite must never simulate successful original Spring setup. */
 Gk83Trace output;memset(&output,0x63,sizeof output);
 Gk83Trace preserved=output;
 Gk83Stop result=gk83_trace_original_reset(
  "/no/such/private/original-stock-rom.nes",1000u,&output);
 assert(result==GK83_SOURCE_REFUSED);
 assert(memcmp(&output,&preserved,sizeof output)==0);
 printf("EXEC0124 PASS: 14 official CMP/CPX/CPY opcodes, 8 newly routed; "
  "%llu independent status comparisons; source gate unchanged.\n",
  (unsigned long long)checks);
 return 0;
}
