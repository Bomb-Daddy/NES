#include "gk1_stage_window.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define IDX(a) ((a)-0x6000u)
static unsigned count=0;
#define CHECK(x) do{++count;if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
static uint8_t s[GK1_RAM_BYTES],a[GK1_RAM_BYTES],i[GK1_RAM_BYTES];
static const uint16_t pcs[4][3]={
 {0x945b,0xd917,0xcb55},
 {0x945b,0xe6c6,0xcb53},
 {0xcb5f,0xe6c4,0x8ff1},
 {0xcb8f,0xcc2b,0xcb60}};
static void unit(uint8_t *p,unsigned slot,unsigned troops,unsigned x,unsigned y,unsigned mob) {
 const unsigned base=IDX(GK1_UNIT_BASE)+GK1_UNIT_STRIDE*slot;
 p[base]=troops&255;p[base+1]=(troops>>8)&255;
 p[base+2]=(uint8_t)x;p[base+3]=(uint8_t)y;
 p[base+4]=1;p[base+5]=(uint8_t)mob;
}
static void word(uint8_t *p,unsigned w){p[IDX(0x613e)]=w&255;p[IDX(0x613f)]=w>>8;}
static Gk1StageProvenance provenance(unsigned c) {
 Gk1StageProvenance r={GK1_SOURCE_ROM_SHA256,GK1_SOURCE_CHECKPOINT_SHA256,1,1,1,
 pcs[c-1][0],pcs[c-1][1],pcs[c-1][2]};return r;
}
static void fixture(unsigned c) {
 memset(s,0,sizeof s);memset(a,0,sizeof a);memset(i,0,sizeof i);
 for(unsigned z=0;z<GK1_TILE_WIDTH*GK1_TILE_HEIGHT;z++)s[IDX(GK1_TILE_BASE)+z]=3;
 unit(s,0,40,7,7,c==GK1_STAGE_LOW_MOBILITY_BLOCK?2:c==GK1_STAGE_EXACT_MOBILITY?4:10);
 unit(s,1,40,c==GK1_STAGE_JOIN?8:6,c==GK1_STAGE_JOIN?7:9,10);
 s[IDX(0x614c)]=14;s[IDX(0x614e)]=5;
 memcpy(a,s,sizeof a);memcpy(i,s,sizeof i);
 if(c==GK1_STAGE_JOIN){unit(a,0,74,8,7,10);unit(a,1,0,8,7,10);word(a,2);a[IDX(0x614e)]=3;}
 if(c==GK1_STAGE_EXACT_MOBILITY){unit(a,0,34,8,7,10);word(a,1);a[IDX(0x614c)]=20;}
 memcpy(i,a,sizeof i);
 if(c==GK1_STAGE_JOIN)i[IDX(0x614c)]=22;
}
static int load(const char *dir,const char *c,const char *phase,uint8_t *buf) {
 char path[1500];snprintf(path,sizeof path,"%s/private/%s/%s.sram",dir,c,phase);
 FILE *f=fopen(path,"rb");if(!f)return 0;
 int ok=fread(buf,1,GK1_RAM_BYTES,f)==GK1_RAM_BYTES && fgetc(f)==EOF;
 fclose(f);return ok;
}
static void trial(unsigned c) {
 fixture(c);
 Gk1StageProvenance p=provenance(c);
 Gk1StageWindowResult out;
 memset(&out,0x5a,sizeof out);
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&out)==GK1_WINDOW_BOUNDED_MATCH);
 CHECK(out.selected_soldiers==40);
 CHECK(out.attempt_soldiers==(c==GK1_STAGE_JOIN?74:c==GK1_STAGE_EXACT_MOBILITY?34:40));
 CHECK(out.selected_completion==0);
 CHECK(out.attempt_completion==(c==GK1_STAGE_JOIN?2:c==GK1_STAGE_EXACT_MOBILITY?1:0));
 CHECK(out.idle_completion==out.attempt_completion);
 CHECK(out.join_like==(c==GK1_STAGE_JOIN));
 CHECK(out.moved==(c==GK1_STAGE_JOIN||c==GK1_STAGE_EXACT_MOBILITY));
 CHECK(out.idle_scratch_transition==(c==GK1_STAGE_JOIN));
 CHECK(out.percent_loss_and_random_expected==34);
 /* Hostile false identity and false source-program sequence assertions. */
 Gk1StageWindowResult guard;memset(&guard,0x6b,sizeof guard);
 Gk1StageWindowResult hold=guard;
 p.selected_vm_pc^=1;
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&guard)==GK1_WINDOW_UNVERIFIED);
 CHECK(memcmp(&guard,&hold,sizeof hold)==0);
 p=provenance(c);p.phases_attested=0;
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&guard)==GK1_WINDOW_UNVERIFIED);
 CHECK(memcmp(&guard,&hold,sizeof hold)==0);
 p=provenance(c);p.rom_sha256="0000";
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&guard)==GK1_WINDOW_UNVERIFIED);
 CHECK(memcmp(&guard,&hold,sizeof hold)==0);
 p=provenance(c);p.checkpoint_sha256="0000";
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&guard)==GK1_WINDOW_UNVERIFIED);
 p=provenance(c);p.intervention_disclosed=0;
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&guard)==GK1_WINDOW_UNVERIFIED);
 p=provenance(c);p.controller_capture_attested=0;
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&guard)==GK1_WINDOW_UNVERIFIED);
 p=provenance(c);
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,8191,a,8192,i,8192,&guard)==GK1_WINDOW_INVALID);
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,8192,a,8191,i,8192,&guard)==GK1_WINDOW_INVALID);
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,8192,a,8192,i,8191,&guard)==GK1_WINDOW_INVALID);
 CHECK(gk1_stage_check((Gk1StageCase)c,&p,NULL,8192,a,8192,i,8192,&guard)==GK1_WINDOW_INVALID);
 CHECK(memcmp(&guard,&hold,sizeof hold)==0);
 /* Deliberate corruption: hidden unit, map tile, soldiers, idle mutation,
  * completion high byte, opaque footprint, and changed phase. */
 for(unsigned mutation=0;mutation<9;mutation++){
  fixture(c);
  if(mutation==0)a[IDX(GK1_TILE_BASE)+17]^=1;
  if(mutation==1)a[IDX(GK1_UNIT_BASE)+GK1_UNIT_STRIDE*19]^=1;
  if(mutation==2)a[IDX(GK1_UNIT_BASE)]^=1;
  if(mutation==3)i[IDX(GK1_UNIT_BASE)]^=1;
  if(mutation==4)a[IDX(GK1_SCRATCH_WORD)+1]^=1;
  if(mutation==5)s[IDX(0x61d4)]^=1;
  if(mutation==6)a[IDX(GK1_UNIT_BASE)+GK1_UNIT_STRIDE*1]^=1;
  if(mutation==7)i[IDX(GK1_UNIT_BASE)+GK1_UNIT_STRIDE*19+7]^=1;
  if(mutation==8)a[IDX(GK1_UNIT_BASE)+3]^=1;
  CHECK(gk1_stage_check((Gk1StageCase)c,&p,s,8192,a,8192,i,8192,&guard)==GK1_WINDOW_MISMATCH);
  CHECK(memcmp(&guard,&hold,sizeof hold)==0);
 }
}
int main(int argc,char **argv) {
 for(unsigned c=1;c<=4;c++)trial(c);
 if(argc>1){
  const char *cases[]={"","friendly_occupied","enemy_occupied","insufficient_mobility","exact_mobility"};
  for(unsigned c=1;c<=4;c++){
   CHECK(load(argv[1],cases[c],"selected_destination",s));
   CHECK(load(argv[1],cases[c],"after_attempt",a));
   CHECK(load(argv[1],cases[c],"after_idle",i));
   Gk1StageProvenance p=provenance(c); Gk1StageWindowResult r={0};
   const int verdict=gk1_stage_check((Gk1StageCase)c,&p,s,sizeof s,a,sizeof a,i,sizeof i,&r);
   if(verdict!=GK1_WINDOW_BOUNDED_MATCH){fprintf(stderr,"ARCHIVED case %s verdict %d\n",cases[c],verdict);return 2;}
   CHECK(verdict==GK1_WINDOW_BOUNDED_MATCH);
   CHECK(r.join_like==(c==GK1_STAGE_JOIN));
   CHECK(r.idle_completion==r.attempt_completion);
   printf("PRIVATE_NATIVE_ARCHIVED %s PASS; soldier=%u partner=%u word=%u lag=%u\n",cases[c],r.attempt_soldiers,r.attempt_partner,r.attempt_completion,r.idle_scratch_transition);
  }
 }
 printf("PASS %u assertions; %s\n",count,argc>1?"PRIVATE native archive read-only replay":"SYNTHETIC source-only tests");
 return 0;
}
