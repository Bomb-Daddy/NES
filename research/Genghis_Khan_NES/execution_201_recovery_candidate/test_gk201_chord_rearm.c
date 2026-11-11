#include "gk1_pad_router.h"
#include "gk99_xinput_bridge.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static unsigned checks=0;
#define CK(x) do { ++checks; if (!(x)) { fprintf(stderr,"GK201 FAIL line %d: %s\n",__LINE__,#x); return 1; } } while (0)
static size_t send(Gk1PadRouter *r,Gk1PadSample *s,unsigned buttons,Gk1PadAction *e){
 s->buttons=(uint16_t)buttons;
 return gk1_pad_step(r,s,16,e,12);
}
static int pair_test(uint16_t a,uint16_t b,Gk1PadAction act_a,Gk1PadAction act_b){
 Gk1PadRouter r;Gk1PadSample s={0};Gk1PadAction out[12];
 gk1_pad_init(&r);
 CK(send(&r,&s,0,out)==0);
 CK(send(&r,&s,a|b,out)==0);
 CK(send(&r,&s,a,out)==0); /* partial release is NOT A or X */
 CK(send(&r,&s,b,out)==0); /* switching retained side also suppressed */
 CK(send(&r,&s,a|b,out)==0);
 CK(send(&r,&s,0,out)==0); /* complete release, rearm but NO phantom */
 CK(send(&r,&s,a,out)==1&&out[0]==act_a);
 CK(send(&r,&s,a,out)==0);
 CK(send(&r,&s,0,out)==0);
 CK(send(&r,&s,b,out)==1&&out[0]==act_b);
 CK(send(&r,&s,0,out)==0);
 /* A second chord reached while holding A must also require release. */
 CK(send(&r,&s,a,out)==1);
 CK(send(&r,&s,a|b,out)==0);
 CK(send(&r,&s,b,out)==0);
 CK(send(&r,&s,0,out)==0);
 CK(send(&r,&s,b,out)==1&&out[0]==act_b);
 return 0;
}
static int xinput_test(void){
 Gk1PadRouter r;Gk1PadSample s={0};Gk1PadAction out[12];
 Gk99XinputRaw raw={0};gk1_pad_init(&r);
 CK(gk1_pad_step(&r,&s,16,out,12)==0);
 raw.buttons=0xC000u; /* actual Xbox X+Y raw XInput */
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==0);
 raw.buttons=0x4000u; /* Y released; X remains: DO NOT CREATE SAVE */
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==0);
 raw.buttons=0;
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==0);
 raw.buttons=0x4000u;
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==1&&out[0]==GK1_PAD_ACT_X);
 /* Disconnect while chord is held resets all latches and seeds raw held
  * buttons on reconnect. No load/save until complete release and re-press. */
 gk1_pad_disconnect(&r);
 raw.buttons=0xC000u;
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==0);
 raw.buttons=0x4000u;
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==0);
 raw.buttons=0;
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==0);
 raw.buttons=0x8000u;
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,out,12)==1&&out[0]==GK1_PAD_ACT_Y);
 return 0;
}
int main(void){
 CK(pair_test(GK1_PAD_A,GK1_PAD_B,GK1_PAD_ACT_A,GK1_PAD_ACT_B)==0);
 CK(pair_test(GK1_PAD_X,GK1_PAD_Y,GK1_PAD_ACT_X,GK1_PAD_ACT_Y)==0);
 CK(pair_test(GK1_PAD_LB,GK1_PAD_RB,GK1_PAD_ACT_LB,GK1_PAD_ACT_RB)==0);
 CK(pair_test(GK1_PAD_UP,GK1_PAD_DOWN,GK1_PAD_ACT_UP,GK1_PAD_ACT_DOWN)==0);
 CK(pair_test(GK1_PAD_LEFT,GK1_PAD_RIGHT,GK1_PAD_ACT_LEFT,GK1_PAD_ACT_RIGHT)==0);
 CK(xinput_test()==0);
 /* Blocking one pair cannot mute unrelated controls (e.g. START). */
 Gk1PadRouter r; Gk1PadSample s={0}; Gk1PadAction out[12];
 gk1_pad_init(&r); CK(send(&r,&s,0,out)==0);
 CK(send(&r,&s,GK1_PAD_X|GK1_PAD_Y|GK1_PAD_MENU,out)==1&&out[0]==GK1_PAD_ACT_MENU);
 CK(send(&r,&s,GK1_PAD_X|GK1_PAD_MENU,out)==0);
 CK(send(&r,&s,0,out)==0);
 CK(send(&r,&s,GK1_PAD_X,out)==1&&out[0]==GK1_PAD_ACT_X);
 printf("GK201 native Linux XInput/joypad chord rearm %u assertions PASS (physical Xbox and Windows NOT RUN)\n",checks);
 return 0;
}
