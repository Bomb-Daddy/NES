#include "gk123_cpu_loadstore.h"
int gk123_decode(uint8_t opcode,Gk123LoadStore *out){
 if(!out)return 0;
 Gk123LoadStore v={GK123_A,GK123_IMM,0u};
 switch(opcode){
 /* LDA */
 case 0xa9:v.mode=GK123_IMM;break;
 case 0xa5:v.mode=GK123_ZP;break;
 case 0xb5:v.mode=GK123_ZPX;break;
 case 0xad:v.mode=GK123_ABS;break;
 case 0xbd:v.mode=GK123_ABSX;break;
 case 0xb9:v.mode=GK123_ABSY;break;
 case 0xa1:v.mode=GK123_INDX;break;
 case 0xb1:v.mode=GK123_INDY;break;
 /* LDX */
 case 0xa2:v.reg=GK123_X;v.mode=GK123_IMM;break;
 case 0xa6:v.reg=GK123_X;v.mode=GK123_ZP;break;
 case 0xb6:v.reg=GK123_X;v.mode=GK123_ZPY;break;
 case 0xae:v.reg=GK123_X;v.mode=GK123_ABS;break;
 case 0xbe:v.reg=GK123_X;v.mode=GK123_ABSY;break;
 /* LDY */
 case 0xa0:v.reg=GK123_Y;v.mode=GK123_IMM;break;
 case 0xa4:v.reg=GK123_Y;v.mode=GK123_ZP;break;
 case 0xb4:v.reg=GK123_Y;v.mode=GK123_ZPX;break;
 case 0xac:v.reg=GK123_Y;v.mode=GK123_ABS;break;
 case 0xbc:v.reg=GK123_Y;v.mode=GK123_ABSX;break;
 /* STA */
 case 0x85:v.store=1u;v.mode=GK123_ZP;break;
 case 0x95:v.store=1u;v.mode=GK123_ZPX;break;
 case 0x8d:v.store=1u;v.mode=GK123_ABS;break;
 case 0x9d:v.store=1u;v.mode=GK123_ABSX;break;
 case 0x99:v.store=1u;v.mode=GK123_ABSY;break;
 case 0x81:v.store=1u;v.mode=GK123_INDX;break;
 case 0x91:v.store=1u;v.mode=GK123_INDY;break;
 /* STX */
 case 0x86:v.store=1u;v.reg=GK123_X;v.mode=GK123_ZP;break;
 case 0x96:v.store=1u;v.reg=GK123_X;v.mode=GK123_ZPY;break;
 case 0x8e:v.store=1u;v.reg=GK123_X;v.mode=GK123_ABS;break;
 /* STY */
 case 0x84:v.store=1u;v.reg=GK123_Y;v.mode=GK123_ZP;break;
 case 0x94:v.store=1u;v.reg=GK123_Y;v.mode=GK123_ZPX;break;
 case 0x8c:v.store=1u;v.reg=GK123_Y;v.mode=GK123_ABS;break;
 default:return 0;
 }
 *out=v;return 1;
}
uint16_t gk123_zp_offset(uint8_t zp,uint8_t index){return (uint8_t)(zp+index);}
uint16_t gk123_address(uint8_t lo,uint8_t hi,uint8_t index){
 return (uint16_t)(((unsigned)lo|((unsigned)hi<<8u))+index);
}
uint8_t gk123_load_flags(uint8_t original_status,uint8_t loaded){
 return (uint8_t)((original_status&~0x82u)|(loaded&0x80u)|(loaded?0u:2u));
}
uint8_t gk123_cycles(uint8_t opcode,uint8_t page_crossed){
 Gk123LoadStore x;
 if(!gk123_decode(opcode,&x))return 0u;
 switch(x.mode){
 case GK123_IMM:return 2u;
 case GK123_ZP:return 3u;
 case GK123_ZPX:case GK123_ZPY:return 4u;
 case GK123_ABS:return 4u;
 case GK123_ABSX:case GK123_ABSY:return (uint8_t)((x.store?5u:4u)+(x.store?0u:(page_crossed?1u:0u)));
 case GK123_INDX:return 6u;
 case GK123_INDY:return (uint8_t)((x.store?6u:5u)+(x.store?0u:(page_crossed?1u:0u)));
 }
 return 0u;
}
