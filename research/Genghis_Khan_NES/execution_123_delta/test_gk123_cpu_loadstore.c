#include "gk123_cpu_loadstore.h"
#include "gk100_cpu_cycles.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {uint8_t op;Gk123Reg reg;Gk123Mode mode;uint8_t store;uint8_t cycles0,cycles1;} Spec;
static const Spec spec[]={
 {0xa9,GK123_A,GK123_IMM,0,2,2},{0xa5,GK123_A,GK123_ZP,0,3,3},
 {0xb5,GK123_A,GK123_ZPX,0,4,4},{0xad,GK123_A,GK123_ABS,0,4,4},
 {0xbd,GK123_A,GK123_ABSX,0,4,5},{0xb9,GK123_A,GK123_ABSY,0,4,5},
 {0xa1,GK123_A,GK123_INDX,0,6,6},{0xb1,GK123_A,GK123_INDY,0,5,6},
 {0xa2,GK123_X,GK123_IMM,0,2,2},{0xa6,GK123_X,GK123_ZP,0,3,3},
 {0xb6,GK123_X,GK123_ZPY,0,4,4},{0xae,GK123_X,GK123_ABS,0,4,4},
 {0xbe,GK123_X,GK123_ABSY,0,4,5},
 {0xa0,GK123_Y,GK123_IMM,0,2,2},{0xa4,GK123_Y,GK123_ZP,0,3,3},
 {0xb4,GK123_Y,GK123_ZPX,0,4,4},{0xac,GK123_Y,GK123_ABS,0,4,4},
 {0xbc,GK123_Y,GK123_ABSX,0,4,5},
 {0x85,GK123_A,GK123_ZP,1,3,3},{0x95,GK123_A,GK123_ZPX,1,4,4},
 {0x8d,GK123_A,GK123_ABS,1,4,4},{0x9d,GK123_A,GK123_ABSX,1,5,5},
 {0x99,GK123_A,GK123_ABSY,1,5,5},{0x81,GK123_A,GK123_INDX,1,6,6},
 {0x91,GK123_A,GK123_INDY,1,6,6},
 {0x86,GK123_X,GK123_ZP,1,3,3},{0x96,GK123_X,GK123_ZPY,1,4,4},
 {0x8e,GK123_X,GK123_ABS,1,4,4},
 {0x84,GK123_Y,GK123_ZP,1,3,3},{0x94,GK123_Y,GK123_ZPX,1,4,4},
 {0x8c,GK123_Y,GK123_ABS,1,4,4}
};
int main(void){
 unsigned count=0;
 const unsigned n=(unsigned)(sizeof(spec)/sizeof(spec[0]));
 assert(n==31);
 for(unsigned op=0;op<256;op++){
  const Spec *expect=0;
  for(unsigned i=0;i<n;i++)if(spec[i].op==op){assert(expect==0);expect=&spec[i];}
  Gk123LoadStore d={GK123_A,GK123_IMM,0};
  int got=gk123_decode((uint8_t)op,&d);
  assert(got==(expect!=0));
  if(expect){
   assert(d.reg==expect->reg&&d.mode==expect->mode&&d.store==expect->store);
   assert(gk123_cycles((uint8_t)op,0)==expect->cycles0);
   assert(gk123_cycles((uint8_t)op,1)==expect->cycles1);
   assert(gk100_cpu_cycles((uint8_t)op,0,0)==expect->cycles0);
   assert(gk100_cpu_cycles((uint8_t)op,0,1)==expect->cycles1);
   count++;
  }else assert(gk123_cycles((uint8_t)op,0)==0);
 }
 assert(!gk123_decode(0xa9,0));
 /* Preserve every non-N/Z status bit; validate all 65,536 combinations. */
 for(unsigned p=0;p<256;p++)for(unsigned v=0;v<256;v++){
  uint8_t actual=gk123_load_flags((uint8_t)p,(uint8_t)v);
  uint8_t expected=(uint8_t)((p&0x7du)|(v&0x80u)|(v==0?2u:0u));
  assert(actual==expected);
 }
 /* NMOS zero-page wraps pointer/index and pointer high-byte fetch. */
 for(unsigned a=0;a<256;a++)for(unsigned x=0;x<256;x++)
  assert(gk123_zp_offset((uint8_t)a,(uint8_t)x)==((a+x)&255u));
 assert(gk123_zp_offset(0xff,2)==1u);
 assert(gk123_address(0xff,0xff,2)==1u);
 assert(gk123_address(0xff,0x01,2)==0x201u);
 assert(gk100_page_crossed(0x01ffu,0x0200u)==1u);
 assert(gk100_page_crossed(0x0100u,0x01ffu)==0u);
 /* Non-cartridge proof: fresh source reads still use read_byte, do not
  * seed unknown RAM/PPU with guessed data or alter campaign. */
 printf("PASS: %u/31 opcode modes, 256 exclusion probes, 65536 flags, "
        "65536 zero-page wraps, cycle/page/store controls\n",count);
 return 0;
}
