#include "gk1_pad_router.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static int checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %s line %d\n",#x,__LINE__);exit(1);}}while(0)
static size_t step(Gk1PadRouter *r,Gk1PadSample *s,uint32_t dt,Gk1PadAction *actions){
 return gk1_pad_step(r,s,dt,actions,8);
}
int main(void){
 Gk1PadRouter r;Gk1PadSample s={0};Gk1PadAction a[8];
 gk1_pad_init(&r);
 CHECK(step(&r,&s,16,a)==0);CHECK(r.connected);
 s.buttons=GK1_PAD_A;
 CHECK(step(&r,&s,16,a)==1);CHECK(a[0]==GK1_PAD_ACT_A);
 for(int i=0;i<100;i++)CHECK(step(&r,&s,16,a)==0); /* holding A never spams */
 s.buttons=0;CHECK(step(&r,&s,16,a)==0);
 s.buttons=GK1_PAD_X|GK1_PAD_Y;
 CHECK(step(&r,&s,16,a)==0); /* simultaneous X/Y suppressed */
 CHECK(step(&r,&s,900,a)==0); /* neither saving nor loading repeats */
 s.buttons=0;CHECK(step(&r,&s,16,a)==0);
 s.buttons=GK1_PAD_DOWN;
 CHECK(step(&r,&s,16,a)==1);CHECK(a[0]==GK1_PAD_ACT_DOWN);
 CHECK(step(&r,&s,300,a)==0);CHECK(step(&r,&s,60,a)==1);CHECK(a[0]==GK1_PAD_ACT_DOWN);
 CHECK(step(&r,&s,119,a)==0);CHECK(step(&r,&s,1,a)==1);
 CHECK(step(&r,&s,1000,a)==1); /* at most one repeat during stalled frame */
 s.buttons=0;CHECK(step(&r,&s,16,a)==0);
 /* Xbox analogue hysteresis and release */
 s.ly=-18000;CHECK(step(&r,&s,16,a)==1);CHECK(a[0]==GK1_PAD_ACT_UP);
 s.ly=-13500;CHECK(step(&r,&s,16,a)==0);CHECK(r.analog_y==-1);
 s.ly=-11000;CHECK(step(&r,&s,16,a)==0);CHECK(r.analog_y==0);
 /* Hat axis (typical Linux Xbox DPad axes 6 and 7). */
 s.hx=32767;CHECK(step(&r,&s,16,a)==1);CHECK(a[0]==GK1_PAD_ACT_RIGHT);
 s.hx=0;CHECK(step(&r,&s,16,a)==0);
 /* Dpad wins over opposing analogue, no opposite duplicate. */
 s.lx=20000;s.buttons=GK1_PAD_LEFT;
 CHECK(step(&r,&s,16,a)==1);CHECK(a[0]==GK1_PAD_ACT_LEFT);
 s.buttons=GK1_PAD_LEFT|GK1_PAD_RIGHT;s.lx=0;
 CHECK(step(&r,&s,16,a)==0);CHECK(!(r.held&(GK1_PAD_LEFT|GK1_PAD_RIGHT)));
 s.buttons=0;CHECK(step(&r,&s,16,a)==0);
 /* Bounds: no write at all when no capacity, or null output. */
 s.buttons=GK1_PAD_A|GK1_PAD_B|GK1_PAD_X|GK1_PAD_Y;
 memset(a,0xa5,sizeof a);
 CHECK(gk1_pad_step(&r,&s,16,a,1)==0); /* simultaneous A/B and X/Y suppressed */
 CHECK(gk1_pad_step(&r,&s,16,a,1)==0);
 s.buttons=0;CHECK(step(&r,&s,16,a)==0);
 s.buttons=GK1_PAD_UP|GK1_PAD_X;
 CHECK(gk1_pad_step(&r,&s,16,a,1)==1);CHECK(a[0]==GK1_PAD_ACT_UP);
 s.buttons=0;CHECK(step(&r,&s,16,a)==0);
 CHECK(gk1_pad_step(&r,&s,16,NULL,8)==0);
 /* Disconnect/reconnect: held button is seeded, never phantom-pressed. */
 gk1_pad_disconnect(&r);s.buttons=GK1_PAD_B;
 CHECK(step(&r,&s,16,a)==0);CHECK(step(&r,&s,16,a)==0);
 s.buttons=0;CHECK(step(&r,&s,16,a)==0);
 s.buttons=GK1_PAD_B;CHECK(step(&r,&s,16,a)==1);CHECK(a[0]==GK1_PAD_ACT_B);
 CHECK(gk1_pad_step(NULL,&s,16,a,8)==0);
 CHECK(gk1_pad_step(&r,NULL,16,a,8)==0);
 printf("EXEC0043 controller routing assertions PASS: %d\n",checks);
 return 0;
}
