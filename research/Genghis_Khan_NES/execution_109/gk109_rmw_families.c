#include "gk109_rmw_families.h"
int gk109_rmw_decode(uint8_t opcode,Gk109Spec *out){
 if(!out)return 0;
 /* Published NMOS 6502 memory-RMW opcode layout. Four addressing modes,
  * six documented operation families. Do not accidentally recognize
  * accumulator or undocumented opcodes as memory bus writes. */
 static const struct {Gk109Kind kind; uint8_t op[4];} families[]={
  {GK109_ASL,{0x06,0x16,0x0e,0x1e}},
  {GK109_LSR,{0x46,0x56,0x4e,0x5e}},
  {GK109_ROL,{0x26,0x36,0x2e,0x3e}},
  {GK109_ROR,{0x66,0x76,0x6e,0x7e}},
  {GK109_DEC,{0xc6,0xd6,0xce,0xde}},
  {GK109_INC,{0xe6,0xf6,0xee,0xfe}}
 };
 for(unsigned k=0;k<6u;k++)for(unsigned mode=0;mode<4u;mode++){
  if(opcode==families[k].op[mode]){
   Gk109Spec next={families[k].kind,(Gk109Mode)mode,
                   (uint8_t)(mode==GK109_ZP?5u:mode==GK109_ABS?6u:7u)};
   /* ZP indexed RMW is six cycles, absolute indexed is seven. */
   if(mode==GK109_ZPX)next.cycles=6u;
   *out=next;return 1;
  }
 }
 return 0;
}
int gk109_rmw_compute(Gk109Kind kind,uint8_t old,uint8_t carry_in,
                      Gk109Result *out){
 if(!out||carry_in>1u||kind<GK109_ASL||kind>GK109_INC)return 0;
 Gk109Result next={0};
 switch(kind){
 case GK109_ASL:next.value=(uint8_t)(old<<1u);next.carry=old>>7u;break;
 case GK109_LSR:next.value=(uint8_t)(old>>1u);next.carry=old&1u;break;
 case GK109_ROL:next.value=(uint8_t)((old<<1u)|carry_in);
                 next.carry=old>>7u;break;
 case GK109_ROR:next.value=(uint8_t)((old>>1u)|(carry_in<<7u));
                 next.carry=old&1u;break;
 case GK109_DEC:next.value=(uint8_t)(old-1u);next.carry=carry_in;break;
 case GK109_INC:next.value=(uint8_t)(old+1u);next.carry=carry_in;break;
 default:return 0;
 }
 next.n=(next.value>>7u)&1u;
 next.z=(uint8_t)(next.value==0u);
 *out=next;return 1;
}
