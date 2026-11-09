#include "gk122_cpu_logic.h"
int gk122_decode(uint8_t op,Gk122Logic *out){
 if(!out)return 0;
 /* 6502 official ORA/AND/EOR addressing families have identical low 5-bit
  * mode encoding; only the high-five family bits differ. */
 uint8_t hi=(uint8_t)(op&0xe0u);
 if(hi!=0x00u&&hi!=0x20u&&hi!=0x40u)return 0;
 Gk122LogicMode mode;
 switch(op&0x1fu){
 case 0x09:mode=GK122_IMM;break;
 case 0x05:mode=GK122_ZP;break;
 case 0x15:mode=GK122_ZPX;break;
 case 0x0d:mode=GK122_ABS;break;
 case 0x1d:mode=GK122_ABSX;break;
 case 0x19:mode=GK122_ABSY;break;
 case 0x01:mode=GK122_INDX;break;
 case 0x11:mode=GK122_INDY;break;
 default:return 0;
 }
 out->kind=(Gk122LogicKind)(hi>>5u);out->mode=mode;
 return 1;
}
uint8_t gk122_result(Gk122LogicKind kind,uint8_t a,uint8_t memory){
 switch(kind){
 case GK122_ORA:return (uint8_t)(a|memory);
 case GK122_AND:return (uint8_t)(a&memory);
 case GK122_EOR:return (uint8_t)(a^memory);
 default:return a;
 }
}
uint8_t gk122_flags(uint8_t before,uint8_t value){
 return (uint8_t)((before&~0x82u)|(value&0x80u)|(value?0:0x02u));
}
uint16_t gk122_zpx(uint8_t base,uint8_t x){return (uint8_t)(base+x);}
uint16_t gk122_indx(uint8_t lo,uint8_t hi){return (uint16_t)(lo|((uint16_t)hi<<8u));}
uint16_t gk122_indy(uint8_t lo,uint8_t hi,uint8_t y){return (uint16_t)(gk122_indx(lo,hi)+y);}
