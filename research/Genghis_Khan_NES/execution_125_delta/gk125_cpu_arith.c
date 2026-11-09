#include "gk125_cpu_arith.h"
int gk125_decode(uint8_t op,Gk125Arithmetic *out){
 if(!out)return 0;
 Gk125Arithmetic d={GK125_ADC,GK125_IMM};
 switch(op){
 case 0x69:d.mode=GK125_IMM;break;
 case 0x65:d.mode=GK125_ZP;break;
 case 0x75:d.mode=GK125_ZPX;break;
 case 0x6d:d.mode=GK125_ABS;break;
 case 0x7d:d.mode=GK125_ABSX;break;
 case 0x79:d.mode=GK125_ABSY;break;
 case 0x61:d.mode=GK125_INDX;break;
 case 0x71:d.mode=GK125_INDY;break;
 case 0xe9:d.kind=GK125_SBC;d.mode=GK125_IMM;break;
 case 0xe5:d.kind=GK125_SBC;d.mode=GK125_ZP;break;
 case 0xf5:d.kind=GK125_SBC;d.mode=GK125_ZPX;break;
 case 0xed:d.kind=GK125_SBC;d.mode=GK125_ABS;break;
 case 0xfd:d.kind=GK125_SBC;d.mode=GK125_ABSX;break;
 case 0xf9:d.kind=GK125_SBC;d.mode=GK125_ABSY;break;
 case 0xe1:d.kind=GK125_SBC;d.mode=GK125_INDX;break;
 case 0xf1:d.kind=GK125_SBC;d.mode=GK125_INDY;break;
 default:return 0;
 }
 *out=d;return 1;
}
uint8_t gk125_cycles(uint8_t opcode,uint8_t crossed){
 Gk125Arithmetic d;
 if(!gk125_decode(opcode,&d))return 0u;
 switch(d.mode){
 case GK125_IMM:return 2u;
 case GK125_ZP:return 3u;
 case GK125_ZPX:return 4u;
 case GK125_ABS:return 4u;
 case GK125_ABSX:case GK125_ABSY:return (uint8_t)(4u+(crossed!=0));
 case GK125_INDX:return 6u;
 case GK125_INDY:return (uint8_t)(5u+(crossed!=0));
 }
 return 0u;
}
int gk125_eval(Gk125Kind kind,uint8_t a,uint8_t operand,uint8_t status,
               uint8_t *out_a,uint8_t *out_status){
 if((kind!=GK125_ADC&&kind!=GK125_SBC)||!out_a||!out_status)return 0;
 unsigned value=kind==GK125_SBC?(unsigned)(operand^0xffu):(unsigned)operand;
 unsigned total=(unsigned)a+value+(unsigned)(status&1u);
 uint8_t result=(uint8_t)total;
 unsigned overflow=(~((unsigned)a^value)&((unsigned)a^(unsigned)result)&0x80u)!=0u;
 uint8_t flags=(uint8_t)((total>255u?1u:0u)|(result==0u?2u:0u)|
                             (overflow?0x40u:0u)|(result&0x80u));
 *out_a=result;
 *out_status=(uint8_t)((status&0x3cu)|flags);
 return 1;
}
