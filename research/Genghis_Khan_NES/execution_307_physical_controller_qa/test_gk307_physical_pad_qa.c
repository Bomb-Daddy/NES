#include "gk307_physical_pad_qa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned tested;
#define C(x) do{tested++;if(!(x)){fprintf(stderr,"FAIL %s:%d %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
static Gk99XinputRaw sample(uint16_t buttons,int16_t x){
 Gk99XinputRaw r={buttons,x,0};return r;
}
static void push(Gk307PhysicalQa *s,uint8_t slot,uint16_t btn,int16_t x){
 Gk99XinputRaw r=sample(btn,x);C(gk307_physical_step(s,slot,&r));
}
int main(void){
 Gk307PhysicalQa q;gk307_physical_init(&q);
 C(q.active_slot==255u&&q.step==0u&&!q.complete);
 C(strstr(gk307_physical_instruction(&q),"CONNECT")!=NULL);
 /* Malformed callbacks never alter existing gamepad QA state. */
 Gk307PhysicalQa before=q;
 Gk99XinputRaw a=sample(0x1000u,0);
 C(!gk307_physical_step(NULL,0,&a));C(!gk307_physical_step(&q,4,&a));
 C(!gk307_physical_step(&q,0,NULL));C(memcmp(&q,&before,sizeof q)==0);
 /* Held-A when a newly attached controller appears must not satisfy A. */
 push(&q,0,0x1000u,0);C(q.step==0u&&q.accepted_edges==0);
 push(&q,0,0,0);C(q.step==1u);
 /* No wrong key or combo advances. Analog drift above dead zone rejects. */
 push(&q,0,0x1000u,0);push(&q,0,0x0001u|0x0002u,0);
 push(&q,0,0x0001u,8000);C(q.step==1u);
 const uint16_t keys[]={0x0001u,0x0002u,0x1000u,0x2000u,0x0010u,0x0020u};
 for(unsigned i=0;i<6;i++){
  push(&q,0,keys[i],0);C(q.step==i+2u&&q.accepted_edges==i+1u);
  push(&q,0,keys[i],0);C(q.step==i+2u);
  push(&q,0,0,0);C(!q.wait_release);
 }
 push(&q,0,0,-22000);C(q.step==7u); /* strict endpoint */
 push(&q,0,0,-30000);C(q.step==8u&&q.accepted_edges==7u);
 push(&q,0,0,30000);C(q.step==8u); /* must release first */
 push(&q,0,0,0);C(!q.wait_release);
 push(&q,0,0,30000);C(q.complete&&q.accepted_edges==8u);
 push(&q,0,0,0);C(q.complete);
 C(strstr(gk307_physical_instruction(&q),"8/8")!=NULL);
 /* Removing or changing a controller invalidates completion and forces
  * neutral, rather than attributing achievements to another physical pad. */
 push(&q,1,0x1000u,0);C(!q.complete&&q.step==0u&&q.active_slot==1u);
 push(&q,1,0,0);C(q.step==1u);
 gk307_physical_disconnect(&q);C(!q.complete&&q.active_slot==255u);
 C(strstr(gk307_physical_instruction(&q),"CONNECT")!=NULL);
 /* Repeated physical QA input is independent of all gameplay and ROMs. */
 printf("EXEC0307 physical XInput diagnostic FSM PASS (%u assertions; hardware NOT RUN)\n",tested);
 return 0;
}
