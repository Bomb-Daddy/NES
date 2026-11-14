/* EXEC0266 controller-only atomic command admission regression.
 * No licensed ROM bytes or simulated original Spring values. */
#include "gk1_pad_router.h"
#include "gk99_xinput_bridge.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CK(x) do { checks++; if(!(x)){fprintf(stderr,"GK266 failed line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static size_t send(Gk1PadRouter *r,Gk1PadSample *s,uint16_t buttons,Gk1PadAction *events){
 s->buttons=buttons;return gk1_pad_step(r,s,16u,events,8u);
}
static int check_pair(uint16_t left,uint16_t right,Gk1PadAction l,Gk1PadAction rr){
 Gk1PadRouter r;Gk1PadSample sample={0};Gk1PadAction out[8]={0};
 gk1_pad_init(&r);
 CK(send(&r,&sample,0,out)==0);
 CK(send(&r,&sample,left|right,out)==0); /* no double transition */
 CK(send(&r,&sample,left,out)==0);       /* partial-release inert */
 CK(send(&r,&sample,right,out)==0);
 CK(send(&r,&sample,0,out)==0);          /* full neutral rearms */
 CK(send(&r,&sample,left,out)==1 && out[0]==l);
 CK(send(&r,&sample,left|right,out)==0);/* chord while held */
 CK(send(&r,&sample,right,out)==0);
 CK(send(&r,&sample,0,out)==0);
 CK(send(&r,&sample,right,out)==1 && out[0]==rr);
 return 0;
}
int main(void){
 CK(check_pair(GK1_PAD_MENU,GK1_PAD_VIEW,GK1_PAD_ACT_MENU,GK1_PAD_ACT_VIEW)==0);
 CK(check_pair(GK1_PAD_A,GK1_PAD_MENU,GK1_PAD_ACT_A,GK1_PAD_ACT_MENU)==0);
 Gk1PadRouter r;Gk1PadSample s={0};Gk1PadAction e[8]={0};
 gk1_pad_init(&r);CK(send(&r,&s,0,e)==0);
 CK(send(&r,&s,GK1_PAD_X|GK1_PAD_Y|GK1_PAD_MENU,e)==1 && e[0]==GK1_PAD_ACT_MENU);
 CK(send(&r,&s,GK1_PAD_X|GK1_PAD_MENU,e)==0); /* X/Y quarantine independent */
 CK(send(&r,&s,0,e)==0);
 CK(send(&r,&s,GK1_PAD_VIEW,e)==1 && e[0]==GK1_PAD_ACT_VIEW);
 /* Authentic XInput bit mapping, independent of platform hardware. */
 gk1_pad_disconnect(&r);memset(&s,0,sizeof s);
 CK(send(&r,&s,0,e)==0);
 Gk99XinputRaw raw={0};raw.buttons=0x0030u; /* START | BACK */
 CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,e,8)==0);
 raw.buttons=0x0010u;CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,e,8)==0);
 raw.buttons=0;CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,e,8)==0);
 raw.buttons=0x0010u;CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,e,8)==1 && e[0]==GK1_PAD_ACT_MENU);
 /* Reconnection with held chord cannot emit a phantom A/START action. */
 gk1_pad_disconnect(&r);raw.buttons=0x1010u;
 CK(gk99_xinput_sample(&raw,&s));CK(gk1_pad_step(&r,&s,16,e,8)==0);
 raw.buttons=0x1000u;CK(gk99_xinput_sample(&raw,&s));
 CK(gk1_pad_step(&r,&s,16,e,8)==0);
 printf("EXEC0266 controller atomic chord: %u assertions PASS; physical Xbox not tested\n",checks);
 return 0;
}
