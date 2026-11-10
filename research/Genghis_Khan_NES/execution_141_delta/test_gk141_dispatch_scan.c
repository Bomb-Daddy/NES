#include "gk141_dispatch_scan.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(c) do{checks++;if(!(c)){fprintf(stderr,"FAIL line %d\n",__LINE__);exit(1);}}while(0)
static void marker(uint8_t *rom,unsigned bank,unsigned position,uint8_t selector){
 const unsigned p=16u+bank*GK141_BANK_BYTES+position;
 rom[p]=0x20u;rom[p+1]=0x85u;rom[p+2]=0xefu;rom[p+3]=selector;
}
int main(int argc,char **argv){
 uint8_t *rom=calloc(1,GK141_IMAGE_SIZE);CHECK(rom!=NULL);
 rom[0]='N';rom[1]='E';rom[2]='S';rom[3]=0x1a;rom[4]=16;rom[6]=0x12;
 const size_t bank15=16u+15u*GK141_BANK_BYTES;
 for(unsigned i=0;i<256u;i++){
  rom[bank15+0x3788u+i]=(uint8_t)i;
  rom[bank15+0x3888u+i]=0xefu;
 }
 marker(rom,0,0,0);
 marker(rom,3,100,254);
 marker(rom,15,15,1);
 /* Last bank byte cannot contain a whole call+selector. */
 marker(rom,14,GK141_BANK_BYTES-4u,2);
 /* Bank-end partial JSR with missing post-call selector: refuse to interpret
  * bytes from a different PRG bank as the actual in-bank selector. */
 const size_t boundary=16u+6u*GK141_BANK_BYTES+GK141_BANK_BYTES-3u;
 rom[boundary]=0x20u;rom[boundary+1u]=0x85u;rom[boundary+2u]=0xefu;
 Gk141DispatchMap map={0};CHECK(gk141_scan_image(rom,GK141_IMAGE_SIZE,&map)==GK141_OK);
 CHECK(map.candidate_total==4u);
 CHECK(map.by_bank[0]==1&&map.by_bank[3]==1&&map.by_bank[14]==1&&map.by_bank[15]==1);
 CHECK(map.by_selector[0]==1&&map.by_selector[1]==1&&map.by_selector[2]==1&&map.by_selector[254]==1);
 CHECK(map.first_candidate_pc==0x8000u&&map.first_bank==0);
 CHECK(map.dispatch_target[254]==0xeffeu);
 CHECK(map.target_below_fixed==0);
 CHECK(map.candidates_boundary_rejected==1u);
 Gk141DispatchMap keep=map;
 CHECK(gk141_scan_image(NULL,GK141_IMAGE_SIZE,&map)==GK141_BAD_INPUT);
 CHECK(memcmp(&keep,&map,sizeof map)==0);
 CHECK(gk141_scan_image(rom,GK141_IMAGE_SIZE-1,&map)==GK141_BAD_INPUT);
 CHECK(memcmp(&keep,&map,sizeof map)==0);
 rom[6]=0;CHECK(gk141_scan_image(rom,GK141_IMAGE_SIZE,&map)==GK141_BAD_INPUT);
 CHECK(memcmp(&keep,&map,sizeof map)==0);rom[6]=0x12;
 CHECK(gk141_scan_authenticated("/this/path/does/not/exist.nes",&map)==GK141_SOURCE_UNVERIFIED);
 CHECK(memcmp(&keep,&map,sizeof map)==0);
 /* Dense synthetic pattern: bound checks, no overflow and canonical counts. */
 for(unsigned bank=0;bank<15;bank++)for(unsigned p=200u;p<5000u;p+=8u)
  marker(rom,bank,p,(uint8_t)(bank+p));
 Gk141DispatchMap dense;
 CHECK(gk141_scan_image(rom,GK141_IMAGE_SIZE,&dense)==GK141_OK);
 CHECK(dense.candidate_total>8500u);
 for(unsigned i=0,sum=0;i<256;i++){
  sum+=dense.by_selector[i];if(i==255)CHECK(sum==dense.candidate_total);
 }
 if(argc==3&&strcmp(argv[1],"--private-stock")==0){
  Gk141DispatchMap original;
  CHECK(gk141_scan_authenticated(argv[2],&original)==GK141_OK);
  /* Authenticated physical source condition, NOT a claimed execution. */
  CHECK(original.candidate_total==986u);
  CHECK(original.by_selector[0u]==400u);
  CHECK(original.by_selector[254u]==161u);
  CHECK(original.by_bank[15u]==163u);
  CHECK(original.target_below_fixed==0u);
  printf("VERIFIED_STATIC original stock JSR $EF85 raw-byte candidates: %u; "
         "selectors[00]=%u [FE]=%u; fixed bank candidates=%u; "
         "target_below_fixed=%u\n",original.candidate_total,
         original.by_selector[0],original.by_selector[254],
         original.by_bank[15],original.target_below_fixed);
 }
 Gk1App *app=calloc(1,sizeof *app);CHECK(app!=NULL);
 gk1_app_init(app);app->active_world=0;app->setup_difficulty[0]=2u;
 Gk1App *unchanged=malloc(sizeof *unchanged);CHECK(unchanged!=NULL);
 memcpy(unchanged,app,sizeof *app);
 CHECK(gk1_app_source_dispatch_scan(app,"/not/a/rom",&map)==GK141_BAD_INPUT);
 CHECK(memcmp(unchanged,app,sizeof *app)==0);
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 memcpy(unchanged,app,sizeof *app);
 CHECK(gk1_app_source_dispatch_scan(app,"/not/a/rom",&map)==GK141_SOURCE_UNVERIFIED);
 CHECK(memcmp(unchanged,app,sizeof *app)==0);
 if(argc==3&&strcmp(argv[1],"--private-stock")==0){
  CHECK(gk1_app_source_dispatch_scan(app,argv[2],&map)==GK141_OK);
  CHECK(map.candidate_total>0u&&memcmp(unchanged,app,sizeof *app)==0);
 }
 free(unchanged);free(app);
 free(rom);printf("GK141 PASS (%u assertions)\n",checks);return 0;
}
