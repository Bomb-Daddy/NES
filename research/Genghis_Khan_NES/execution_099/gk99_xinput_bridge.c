#include "gk99_xinput_bridge.h"
#include <stdint.h>
/* Documented XInput GAMEPAD constants; kept self-contained for Linux QA. */
enum {
 XI_UP=0x0001, XI_DOWN=0x0002, XI_LEFT=0x0004, XI_RIGHT=0x0008,
 XI_START=0x0010, XI_BACK=0x0020, XI_LB=0x0100, XI_RB=0x0200,
 XI_A=0x1000, XI_B=0x2000, XI_X=0x4000, XI_Y=0x8000
};
int gk99_xinput_sample(const Gk99XinputRaw *raw,Gk1PadSample *out){
 if(!raw||!out)return 0;
 Gk1PadSample s={0};
 static const struct {uint16_t raw,port;} mapping[]={
  {XI_UP,GK1_PAD_UP},{XI_DOWN,GK1_PAD_DOWN},
  {XI_LEFT,GK1_PAD_LEFT},{XI_RIGHT,GK1_PAD_RIGHT},
  {XI_A,GK1_PAD_A},{XI_B,GK1_PAD_B},{XI_X,GK1_PAD_X},{XI_Y,GK1_PAD_Y},
  {XI_START,GK1_PAD_MENU},{XI_BACK,GK1_PAD_VIEW},
  {XI_LB,GK1_PAD_LB},{XI_RB,GK1_PAD_RB}
 };
 for(unsigned i=0;i<sizeof mapping/sizeof mapping[0];i++)
  if(raw->buttons & mapping[i].raw)s.buttons|=mapping[i].port;
 s.lx=raw->thumb_lx;
 /* XInput Y positive means UP; portable router Y positive means DOWN.
  * Saturate negation of INT16_MIN to avoid signed arithmetic overflow. */
 s.ly=raw->thumb_ly==INT16_MIN?INT16_MAX:(int16_t)-raw->thumb_ly;
 *out=s;
 return 1;
}
int gk99_xinput_slot(uint8_t mask,uint8_t previous,uint8_t *chosen){
 if(!chosen|| (mask&0xf0u)|| (previous>3u&&previous!=0xffu))return 0;
 uint8_t selected=0xffu;
 if(previous<4u && (mask&(1u<<previous)))selected=previous;
 else for(uint8_t i=0;i<4u;i++)if(mask&(1u<<i)){selected=i;break;}
 *chosen=selected;
 return 1;
}
