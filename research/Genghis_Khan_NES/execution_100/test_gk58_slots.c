#include "gk58_slots.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static int checks=0;
#define CHECK(e) do{checks++;if(!(e)){fprintf(stderr,"SLOT FAIL line %d: %s\n",__LINE__,#e);return 2;}}while(0)
int main(void){
 char a[1100],shortbuf[4];
 CHECK(gk58_slot_path("prior_campaign.gkp",1,a,sizeof a));
 CHECK(strcmp(a,"prior_campaign.gkp")==0);
 CHECK(gk58_slot_path("prior_campaign.gkp",2,a,sizeof a));
 CHECK(strcmp(a,"prior_campaign.gkp.slot2")==0);
 CHECK(gk58_slot_path("prior_campaign.gkp",3,a,sizeof a));
 CHECK(strcmp(a,"prior_campaign.gkp.slot3")==0);
 CHECK(!gk58_slot_path("prior_campaign.gkp",4,a,sizeof a));
 CHECK(!gk58_slot_path("prior_campaign.gkp",0,a,sizeof a));
 CHECK(!gk58_slot_path("prior_campaign.gkp",3,shortbuf,sizeof shortbuf));
 CHECK(!gk58_slot_path("",1,a,sizeof a));
 CHECK(!gk58_slot_path("a",1,a,0));
 CHECK(!gk58_slot_path("a",1,a,1));
 CHECK(!gk58_slot_path("a",1,a,2));
 CHECK(!gk58_slot_path(a,1,a,sizeof a));
 CHECK(gk58_slot_cycle(1,1)==2);
 CHECK(gk58_slot_cycle(2,1)==3);
 CHECK(gk58_slot_cycle(3,1)==1);
 CHECK(gk58_slot_cycle(1,-1)==3);
 CHECK(gk58_slot_cycle(2,-1)==1);
 CHECK(gk58_slot_cycle(3,-1)==2);
 CHECK(gk58_slot_cycle(1,0)==1);
 CHECK(gk58_slot_cycle(4,1)==0);
 Gk1PadRouter r;Gk1PadSample s={0};Gk1PadAction e[8];
 gk1_pad_init(&r);CHECK(gk1_pad_step(&r,&s,16,e,8)==0);
 s.buttons=GK1_PAD_RB;CHECK(gk1_pad_step(&r,&s,16,e,8)==1);CHECK(e[0]==GK1_PAD_ACT_RB);
 CHECK(gk1_pad_step(&r,&s,1000,e,8)==0);
 s.buttons=0;CHECK(gk1_pad_step(&r,&s,16,e,8)==0);
 s.buttons=GK1_PAD_LB;CHECK(gk1_pad_step(&r,&s,16,e,8)==1);CHECK(e[0]==GK1_PAD_ACT_LB);
 s.buttons=0;CHECK(gk1_pad_step(&r,&s,16,e,8)==0);
 s.buttons=GK1_PAD_LB|GK1_PAD_RB;CHECK(gk1_pad_step(&r,&s,16,e,8)==0);
 puts("EXEC0058 TESTED_PORT_LINUX: slot paths, invalid names/slots and LB/RB edges PASS");
 printf("EXEC0058 checks=%d\n",checks);
 return 0;
}
