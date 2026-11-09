#include "gk125_cpu_arith.h"
#include "gk100_cpu_cycles.h"
#include "gk83_reset_trace.h"
#include "gk91_reset_zero.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
typedef struct { uint8_t op; Gk125Kind kind; Gk125Mode mode; uint8_t base,cross; } Spec;
static const Spec specs[]={
 {0x69,GK125_ADC,GK125_IMM,2,2},{0x65,GK125_ADC,GK125_ZP,3,3},
 {0x75,GK125_ADC,GK125_ZPX,4,4},{0x6d,GK125_ADC,GK125_ABS,4,4},
 {0x7d,GK125_ADC,GK125_ABSX,4,5},{0x79,GK125_ADC,GK125_ABSY,4,5},
 {0x61,GK125_ADC,GK125_INDX,6,6},{0x71,GK125_ADC,GK125_INDY,5,6},
 {0xe9,GK125_SBC,GK125_IMM,2,2},{0xe5,GK125_SBC,GK125_ZP,3,3},
 {0xf5,GK125_SBC,GK125_ZPX,4,4},{0xed,GK125_SBC,GK125_ABS,4,4},
 {0xfd,GK125_SBC,GK125_ABSX,4,5},{0xf9,GK125_SBC,GK125_ABSY,4,5},
 {0xe1,GK125_SBC,GK125_INDX,6,6},{0xf1,GK125_SBC,GK125_INDY,5,6}
};
static void check_modes(void){
 assert(sizeof specs/sizeof specs[0]==16u);
 for(unsigned op=0;op<256u;op++){
  const Spec *match=NULL;
  for(unsigned j=0;j<16u;j++)if(specs[j].op==op){assert(match==NULL);match=&specs[j];}
  Gk125Arithmetic d={GK125_SBC,GK125_INDY};
  int found=gk125_decode((uint8_t)op,&d);
  assert(found==(match!=NULL));
  if(!match){assert(d.kind==GK125_SBC&&d.mode==GK125_INDY);
   assert(gk125_cycles((uint8_t)op,0u)==0u);continue;}
  assert(d.kind==match->kind&&d.mode==match->mode);
  assert(gk125_cycles((uint8_t)op,0u)==match->base);
  assert(gk125_cycles((uint8_t)op,1u)==match->cross);
  assert(gk100_cpu_cycles((uint8_t)op,0u,0u)==match->base);
  assert(gk100_cpu_cycles((uint8_t)op,0u,1u)==match->cross);
 }
 assert(!gk125_decode(0x69u,NULL));
}
int main(int argc,char **argv){
 check_modes();
 /* Independent arithmetic oracle: unsigned 8-bit carry, signed overflow,
  * untouched bits 2..5. Does not call the implementation formula. */
 uint64_t comparisons=0u;
 for(unsigned a=0;a<256u;a++)for(unsigned b=0;b<256u;b++)
 for(unsigned carry=0;carry<2u;carry++)for(unsigned p=0;p<256u;p++){
  unsigned status=(p&~1u)|carry;
  for(unsigned kind=0;kind<2u;kind++){
   int sum=kind==GK125_ADC?(int)a+(int)b+(int)carry:
                                (int)a-(int)b-(int)(1u-carry);
   uint8_t expected_a=(uint8_t)sum;
   int signed_a=(int)(int8_t)(uint8_t)a;
   int signed_b=(int)(int8_t)(uint8_t)b;
   int signed_sum=kind==GK125_ADC?signed_a+signed_b+(int)carry:
                                            signed_a-signed_b-(int)(1u-carry);
   int signed_overflow=(signed_sum< -128 || signed_sum>127);
   unsigned expected_flags=((kind==GK125_ADC?sum>255:sum>=0)?1u:0u)|
          (expected_a==0u?2u:0u)|(signed_overflow?0x40u:0u)|
          (expected_a&0x80u);
   uint8_t result=0xffu,result_p=0xffu;
   assert(gk125_eval((Gk125Kind)kind,(uint8_t)a,(uint8_t)b,(uint8_t)status,&result,&result_p));
   assert(result==expected_a);
   assert(result_p==(uint8_t)((status&0x3cu)|expected_flags));
   comparisons++;
  }
 }
 assert(comparisons==67108864u);
 uint8_t a=0xb7u,p=0x2bu;
 assert(!gk125_eval((Gk125Kind)2u,0x32u,0x52u,0u,&a,&p));
 assert(a==0xb7u&&p==0x2bu);
 assert(!gk125_eval(GK125_ADC,0,0,0,NULL,&p));assert(p==0x2bu);
 assert(!gk125_eval(GK125_ADC,0,0,0,&a,NULL));assert(a==0xb7u);
 Gk83Trace output;memset(&output,0x5e,sizeof output);Gk83Trace before=output;
 assert(gk83_trace_original_reset("/missing/private/stock.nes",20000u,&output)==GK83_SOURCE_REFUSED);
 assert(memcmp(&output,&before,sizeof output)==0);
 if(argc>1){
  Gk83Stop stop=gk83_trace_original_reset(argv[1],20000u,&output);
  assert(stop==GK83_UNKNOWN_RAM_GATE);
  assert(output.first_unknown_ram_read==0x00f5u);
  assert(output.instructions==70u);
  Gk83Trace replay;memset(&replay,0,sizeof replay);
  assert(gk83_trace_original_reset(argv[1],20000u,&replay)==stop);
  assert(memcmp(&replay,&output,sizeof replay)==0);
  printf("VERIFIED_STATIC private exact stock gate at $00F5 after 70 instructions; no new Mesen execution.\n");
 }
 printf("EXEC0125 PASS: all 16 ADC/SBC modes, 67108864 independent binary/decimal-flag oracle checks; source gates retained.\n");
 return 0;
}
