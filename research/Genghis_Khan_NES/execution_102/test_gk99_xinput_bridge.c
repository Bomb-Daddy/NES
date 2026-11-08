#include "gk99_xinput_bridge.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define CHECK(a) do{checks++;if(!(a)){fprintf(stderr,"FAIL %s line %d\n",#a,__LINE__);exit(1);}}while(0)
static uint16_t independent_oracle(uint16_t v){
 const uint16_t in[]={0x0001,0x0002,0x0004,0x0008,0x1000,0x2000,0x4000,0x8000,0x0010,0x0020,0x0100,0x0200};
 uint16_t out=0;
 for(unsigned i=0;i<12u;i++)if(v&in[i])out|=(uint16_t)(1u<<i);
 return out;
}
int main(void){
 Gk99XinputRaw raw={0};Gk1PadSample sample={0};
 /* Exhaustive raw 16-bit button tests, including unused bits, all combinations. */
 for(unsigned b=0;b<65536u;b++){
  raw.buttons=(uint16_t)b;
  CHECK(gk99_xinput_sample(&raw,&sample)==1);
  CHECK(sample.buttons==independent_oracle((uint16_t)b));
  CHECK(sample.hx==0&&sample.hy==0&&sample.lx==0&&sample.ly==0);
 }
 for(int y=-32768;y<=32767;y++){
  raw.buttons=0;raw.thumb_ly=(int16_t)y;raw.thumb_lx=(int16_t)y;
  CHECK(gk99_xinput_sample(&raw,&sample)==1);
  CHECK(sample.ly==(y==-32768?32767:-y));CHECK(sample.lx==y);
 }
 /* Host errors/NULL never overwrite a potentially live controller sample. */
 Gk1PadSample sentinel={0};memset(&sentinel,0x5a,sizeof sentinel);
 sample=sentinel;
 CHECK(gk99_xinput_sample(NULL,&sample)==0);
 CHECK(memcmp(&sentinel,&sample,sizeof sample)==0);
 CHECK(gk99_xinput_sample(&raw,NULL)==0);
 for(unsigned mask=0;mask<16u;mask++){
  for(unsigned prev=0;prev<=4u;prev++){
   uint8_t old=prev==4u?0xffu:(uint8_t)prev,selected=0x5a;
   CHECK(gk99_xinput_slot((uint8_t)mask,old,&selected));
   uint8_t expected=0xffu;
   if(old<4 && (mask&(1u<<old)))expected=old;
   else for(unsigned k=0;k<4;k++)if(mask&(1u<<k)){expected=(uint8_t)k;break;}
   CHECK(selected==expected);
  }
 }
 uint8_t selected=0x5a;
 CHECK(!gk99_xinput_slot(0x10,0xff,&selected)&&selected==0x5a);
 CHECK(!gk99_xinput_slot(0x01,0x04,&selected)&&selected==0x5a);
 CHECK(!gk99_xinput_slot(0x01,0xff,NULL));
 /* Controller route integration: both menu and A mapped to one-shot edges;
  * reconnection seeds held START, never launches a phantom menu command. */
 Gk1PadRouter router;Gk1PadAction actions[16];Gk1PadSample none={0};
 gk1_pad_init(&router);CHECK(gk1_pad_step(&router,&none,16,actions,16)==0);
 raw.buttons=0x0010;raw.thumb_ly=0;raw.thumb_lx=0;
 CHECK(gk99_xinput_sample(&raw,&sample));
 CHECK(gk1_pad_step(&router,&sample,16,actions,16)==1);
 CHECK(actions[0]==GK1_PAD_ACT_MENU);
 CHECK(gk1_pad_step(&router,&sample,16,actions,16)==0);
 gk1_pad_disconnect(&router);
 CHECK(gk1_pad_step(&router,&sample,16,actions,16)==0);
 raw.buttons=0;CHECK(gk99_xinput_sample(&raw,&sample));
 CHECK(gk1_pad_step(&router,&sample,16,actions,16)==0);
 raw.buttons=0x1000;CHECK(gk99_xinput_sample(&raw,&sample));
 CHECK(gk1_pad_step(&router,&sample,16,actions,16)==1);
 CHECK(actions[0]==GK1_PAD_ACT_A);
 CHECK(gk1_pad_step(&router,&sample,16,actions,16)==0);
 printf("EXEC0099 platform-neutral XInput + 4-slot integration PASS: %u assertions\n",checks);
 return 0;
}
