#include "gk124_cpu_compare.h"
int gk124_compare_decode(uint8_t op,Gk124Compare *out){
 if(!out)return 0;
 Gk124Compare d={GK124_CMP_A,GK124_IMM};
 switch(op){
 /* CMP A */
 case 0xc9:d.mode=GK124_IMM;break;
 case 0xc5:d.mode=GK124_ZP;break;
 case 0xd5:d.mode=GK124_ZPX;break;
 case 0xcd:d.mode=GK124_ABS;break;
 case 0xdd:d.mode=GK124_ABSX;break;
 case 0xd9:d.mode=GK124_ABSY;break;
 case 0xc1:d.mode=GK124_INDX;break;
 case 0xd1:d.mode=GK124_INDY;break;
 /* CPX X */
 case 0xe0:d.reg=GK124_CPX_X;d.mode=GK124_IMM;break;
 case 0xe4:d.reg=GK124_CPX_X;d.mode=GK124_ZP;break;
 case 0xec:d.reg=GK124_CPX_X;d.mode=GK124_ABS;break;
 /* CPY Y */
 case 0xc0:d.reg=GK124_CPY_Y;d.mode=GK124_IMM;break;
 case 0xc4:d.reg=GK124_CPY_Y;d.mode=GK124_ZP;break;
 case 0xcc:d.reg=GK124_CPY_Y;d.mode=GK124_ABS;break;
 default:return 0;
 }
 *out=d;
 return 1;
}
uint8_t gk124_compare_flags(uint8_t p,uint8_t reg,uint8_t operand){
 uint8_t difference=(uint8_t)(reg-operand);
 return (uint8_t)((p&~0x83u)|(reg>=operand?1u:0u)|
   (difference==0u?2u:0u)|(difference&0x80u));
}
uint8_t gk124_compare_cycles(uint8_t op,uint8_t page_crossed){
 Gk124Compare d;
 if(!gk124_compare_decode(op,&d))return 0;
 switch(d.mode){
 case GK124_IMM:return 2u;
 case GK124_ZP:return 3u;
 case GK124_ZPX:return 4u;
 case GK124_ABS:return 4u;
 case GK124_ABSX:case GK124_ABSY:return (uint8_t)(4u+(page_crossed?1u:0u));
 case GK124_INDX:return 6u;
 case GK124_INDY:return (uint8_t)(5u+(page_crossed?1u:0u));
 }
 return 0u;
}
