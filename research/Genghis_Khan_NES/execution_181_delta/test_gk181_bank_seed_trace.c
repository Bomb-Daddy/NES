#include "gk181_bank_seed_trace.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define REQUIRE(x) do {checks++;if(!(x)){fprintf(stderr,"failed %u at %d: %s\n",checks,__LINE__,#x);return 1;}}while(0)
static void emit(uint8_t *p,unsigned bank,uint16_t pc,const uint8_t *code,size_t n){
 size_t off=16u+(pc>=0xc000u?15u:bank)*16384u+
  (pc>=0xc000u?(size_t)pc-0xc000u:(size_t)pc-0x8000u);
 memcpy(p+off,code,n);
}
static const Gk181Site *find(const Gk181Map *r,uint16_t pc,unsigned bank){
 for(uint32_t k=0;k<r->source_sites;k++)
  if(r->sites[k].pc==pc&&r->sites[k].bank==bank)return &r->sites[k];
 return NULL;
}
int main(int argc,char **argv){
 uint8_t *image=calloc(1,GK180_ROM_BYTES);REQUIRE(image!=NULL);
 memcpy(image,"NES\032",4);image[4]=16u;image[6]=0x12u;
 /* Synthetic only. Fixed branch enters a low-bank call at $8100.
  * Bank 0 writes seed84; alternate bank1 writes seed85. */
 for(int b=0;b<16;b++)memset(image+16u+b*16384u,0x60,16384u);
 static const uint8_t fixed[]={0x20,0x00,0x81,0x85,0x86,0x60};
 static const uint8_t low0[]={0x85,0x84,0x60};
 static const uint8_t low1[]={0xe6,0x85,0x60};
 static const uint8_t nmi[]={0x66,0x87,0x60};
 static const uint8_t irq[]={0x8d,0x89,0x00,0x60};
 emit(image,0,0xc100,fixed,sizeof fixed);
 emit(image,0,0x8100,low0,sizeof low0);
 emit(image,1,0x8100,low1,sizeof low1);
 emit(image,0,0xc200,nmi,sizeof nmi);
 emit(image,0,0xc300,irq,sizeof irq);
 unsigned high=16u+15u*16384u;
 image[high+0x3ffc]=0x00;image[high+0x3ffd]=0xc1;
 image[high+0x3ffa]=0x00;image[high+0x3ffb]=0xc2;
 image[high+0x3ffe]=0x00;image[high+0x3fff]=0xc3;
 Gk181Map r={0};REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES,0,&r)==GK181_OK);
 REQUIRE(r.assumed_low_bank==0u&&r.source_sites==4u);
 REQUIRE(r.low_seed_candidates==1u&&r.high_seed_candidates==3u);
 const Gk181Site *s=find(&r,0x8100,0u);
 REQUIRE(s&&s->seed_addr==0x84u&&s->kind==GK180_STORE&&s->roots==1u);
 REQUIRE(find(&r,0xc103,15u)&&find(&r,0xc200,15u)&&find(&r,0xc300,15u));
 REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES,1,&r)==GK181_OK);
 s=find(&r,0x8100,1u);
 REQUIRE(s&&s->seed_addr==0x85u&&s->kind==GK180_RMW);
 REQUIRE(!find(&r,0x8100,0u));
 /* Mapper write is a hard source-flow barrier; code after it is not proven
  * reachable under the same bank mapping. */
 static const uint8_t barrier[]={0x8d,0x00,0xe0,0x85,0x88,0x60};
 emit(image,1,0x8100,barrier,sizeof barrier);
 REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES,1,&r)==GK181_OK);
 REQUIRE(r.mapper_barriers>0u&&!find(&r,0x8103,1u));
 REQUIRE(r.low_seed_candidates==0u);
 /* Hostile mutation: remove the bank-register store and the previously
  * blocked direct candidate must become visible. Reinsert original after. */
 static const uint8_t no_mapper[]={0xea,0xea,0xea,0x85,0x88,0x60};
 emit(image,1,0x8100,no_mapper,sizeof no_mapper);
 REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES,1,&r)==GK181_OK);
 s=find(&r,0x8103,1u);
 REQUIRE(s&&s->seed_addr==0x88u&&r.low_seed_candidates==1u);
 /* An indexed write may reach a seed but must NOT be counted as a direct
  * seed writer without X/Y state. */
 static const uint8_t indexed[]={0x95,0x84,0x60};
 emit(image,1,0x8100,indexed,sizeof indexed);
 REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES,1,&r)==GK181_OK);
 REQUIRE(r.unresolved_indexed_writes>0u&&!find(&r,0x8100,1u));
 Gk181Map old=r;
 REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES,15,&r)==GK181_BAD_INPUT);
 REQUIRE(memcmp(&old,&r,sizeof r)==0);
 REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES-1u,0,&r)==GK181_BAD_INPUT);
 REQUIRE(memcmp(&old,&r,sizeof r)==0);
 image[0]=0;REQUIRE(gk181_scan_image(image,GK180_ROM_BYTES,0,&r)==GK181_BAD_INPUT);
 REQUIRE(memcmp(&old,&r,sizeof r)==0);
 image[0]='N';REQUIRE(gk181_scan_authenticated("/not/a/stock/rom",0,&r)==GK181_SOURCE);
 REQUIRE(memcmp(&old,&r,sizeof r)==0);
 REQUIRE(gk181_scan_authenticated(NULL,0,&r)==GK181_BAD_INPUT);
 free(image);
 if(argc==2){
  /* Authenticated private ROM, 15 explicit hypotheses; not observed NES. */
  Gk181Map original={0};
  for(uint8_t b=0;b<15;b++){
   REQUIRE(gk181_scan_authenticated(argv[1],b,&original)==GK181_OK);
   REQUIRE(original.assumed_low_bank==b&&original.decoded[0]>0u);
   REQUIRE(original.source_sites<=GK181_MAX_SITES);
  }
  Gk1App app;gk1_app_init(&app);
  Gk181Map sentinel=original;
  REQUIRE(gk1_app_bank_seed_trace(&app,argv[1],0,&original)==GK181_BAD_INPUT);
  REQUIRE(memcmp(&sentinel,&original,sizeof original)==0);
  app.active_world=0;app.world_ready[0]=1;app.worlds[0].scenario_id=GK1_MONGOL_CONQUEST;
  app.worlds[0].country_count=14;app.worlds[0].phase=GK1_PRE_SETUP_TEMPLATE;
  app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;app.setup_difficulty[0]=3;
  REQUIRE(gk1_app_bank_seed_trace(&app,argv[1],0,&original)==GK181_OK);
  app.stock_checkpoint.valid=1;
  sentinel=original;
  REQUIRE(gk1_app_bank_seed_trace(&app,argv[1],0,&original)==GK181_BAD_INPUT);
  REQUIRE(memcmp(&sentinel,&original,sizeof original)==0);
  printf("VERIFIED_STATIC private authenticated 15 bank hypotheses PASS, low candidate=%u, mapper barriers=%u; no NES startup trace\n",
   (unsigned)sentinel.low_seed_candidates,(unsigned)sentinel.mapper_barriers);
 }
 printf("GK181 %u checks PASS\n",checks);return 0;
}
