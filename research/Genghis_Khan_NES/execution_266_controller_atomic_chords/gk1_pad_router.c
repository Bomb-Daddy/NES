#include "gk1_pad_router.h"
#include <string.h>
#define ENGAGE 16384
#define RELEASE 11500
#define FIRST_REPEAT 360u
#define LATER_REPEAT 120u

static int8_t axis(int16_t value,int8_t was){
 /* Hysteresis: a held direction survives noise until below release. */
 if(was<0){if(value<=-RELEASE)return -1;}
 else if(was>0){if(value>=RELEASE)return 1;}
 if(value<=-ENGAGE)return -1;
 if(value>=ENGAGE)return 1;
 return 0;
}
/* Never turn the surviving half of a suppressed chord into a new edge.
 * A pair is rearmed only after the player has released BOTH buttons for at
 * least one sample. This protects Xbox X/Y storage and A/B confirmations.
 * Gating the physical direction buttons also works when analog sticks are
 * present, while leaving independent analog-only navigation unchanged. */
static void disarm_pair(Gk1PadRouter *r,uint16_t raw,
                        uint16_t *resolved,uint16_t pair,uint8_t flag){
 if((raw&pair)==pair)r->chord_blocked|=flag;
 if(r->chord_blocked&flag){
  *resolved=(uint16_t)(*resolved&~pair);
  if(!(raw&pair))r->chord_blocked=(uint8_t)(r->chord_blocked&~flag);
 }
}
static uint16_t resolve(Gk1PadRouter *r,const Gk1PadSample *s){
 r->analog_x=axis(s->lx,r->analog_x);
 r->analog_y=axis(s->ly,r->analog_y);
 r->hat_x=axis(s->hx,r->hat_x);
 r->hat_y=axis(s->hy,r->hat_y);
 uint16_t b=(uint16_t)(s->buttons&0xfffu);
 /* Physical D-pad overrides analogue direction in the corresponding axis. */
 if(!(b&(GK1_PAD_LEFT|GK1_PAD_RIGHT))){
  int x=r->hat_x?r->hat_x:r->analog_x;
  if(x<0)b|=GK1_PAD_LEFT;
  if(x>0)b|=GK1_PAD_RIGHT;
 }
 if(!(b&(GK1_PAD_UP|GK1_PAD_DOWN))){
  int y=r->hat_y?r->hat_y:r->analog_y;
  if(y<0)b|=GK1_PAD_UP;
  if(y>0)b|=GK1_PAD_DOWN;
 }
 /* Chord quarantine: hold and partial-release are inert until one wholly
  * neutral frame. This closes the unsafe A+B -> A / X+Y -> X transition. */
 uint16_t hardware=(uint16_t)(s->buttons&0x0fffu);
 disarm_pair(r,hardware,&b,GK1_PAD_A|GK1_PAD_B,0x01u);
 disarm_pair(r,hardware,&b,GK1_PAD_X|GK1_PAD_Y,0x02u);
 disarm_pair(r,hardware,&b,GK1_PAD_LB|GK1_PAD_RB,0x04u);
 disarm_pair(r,hardware,&b,GK1_PAD_UP|GK1_PAD_DOWN,0x08u);
 disarm_pair(r,hardware,&b,GK1_PAD_LEFT|GK1_PAD_RIGHT,0x10u);
 /* EXEC0266: START+BACK (setup + view) and A+START (confirm + setup)
  * are unsafe compound commands on the same controller frame. Previously
  * each emitted TWO sequential actions into potentially different screens.
  * Quarantine both buttons through partial release, exactly as A+B and
  * X+Y. Preserve unrelated X/Y+START navigation from EXEC0201. */
 disarm_pair(r,hardware,&b,GK1_PAD_MENU|GK1_PAD_VIEW,0x20u);
 disarm_pair(r,hardware,&b,GK1_PAD_A|GK1_PAD_MENU,0x40u);
 return b;
}
void gk1_pad_init(Gk1PadRouter *r){if(r)memset(r,0,sizeof *r);}
void gk1_pad_disconnect(Gk1PadRouter *r){gk1_pad_init(r);}
void gk1_pad_seed(Gk1PadRouter *r,const Gk1PadSample *s){
 if(!r||!s)return;
 gk1_pad_init(r);r->held=resolve(r,s);r->connected=1;
 for(int i=0;i<4;i++)if(r->held&(1u<<i))r->repeat_due_ms[i]=FIRST_REPEAT;
}
static void append(Gk1PadAction *out,size_t cap,size_t *count,int action){
 if(*count<cap && out)out[*count]=(Gk1PadAction)action;
 if(*count<cap && out)(*count)++;
}
size_t gk1_pad_step(Gk1PadRouter *r,const Gk1PadSample *s,uint32_t dt_ms,
                    Gk1PadAction *out,size_t cap){
 if(!r||!s)return 0;
 if(!r->connected){gk1_pad_seed(r,s);return 0;}
 uint16_t now=resolve(r,s),edges=(uint16_t)(now&~r->held);
 size_t count=0;
 if(dt_ms>1000u)dt_ms=1000u;
 for(int i=0;i<4;i++){
  uint16_t bit=(uint16_t)(1u<<i);
  if(!(now&bit)){r->repeat_due_ms[i]=0;continue;}
  if(edges&bit){append(out,cap,&count,i);r->repeat_due_ms[i]=FIRST_REPEAT;}
  else if(r->repeat_due_ms[i]<=dt_ms){
   append(out,cap,&count,i);
   r->repeat_due_ms[i]=LATER_REPEAT;
  }else r->repeat_due_ms[i]-=dt_ms;
 }
 for(int i=4;i<12;i++)if(edges&(1u<<i))append(out,cap,&count,i);
 r->held=now;
 return count;
}
