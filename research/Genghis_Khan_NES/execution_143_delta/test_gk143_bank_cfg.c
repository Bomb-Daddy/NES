#include "gk143_bank_cfg.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned tests;
#define CHECK(v) do{tests++;if(!(v)){fprintf(stderr,"GK143 assertion %d: %s\n",__LINE__,#v);return 1;}}while(0)
static void put(uint8_t *b,unsigned bank,uint16_t pc,uint8_t x){b[16u+bank*16384u+(pc-(pc>=0xc000u?0xc000u:0x8000u))]=x;}
static void vector(uint8_t *b,uint16_t addr,uint16_t pc){put(b,15,addr,(uint8_t)pc);put(b,15,addr+1u,(uint8_t)(pc>>8));}
int main(int argc,char **argv){
 uint8_t *img=calloc(GK141_IMAGE_SIZE,1);CHECK(img);
 img[0]='N';img[1]='E';img[2]='S';img[3]=0x1a;img[4]=16;img[6]=0x12;
 vector(img,0xfffa,0xc100);vector(img,0xfffc,0x8000);vector(img,0xfffe,0xc120);
 put(img,15,0xc100,0x60);put(img,15,0xc120,0x40);
 put(img,15,0xef85,0x60);
 /* Boot bank 0 dispatch, and an independent other-bank decoy. */
 put(img,0,0x8000,0x20);put(img,0,0x8001,0x85);put(img,0,0x8002,0xef);put(img,0,0x8003,0x42);
 put(img,0,0x8004,0x20);put(img,0,0x8005,0x10);put(img,0,0x8006,0x80);
 put(img,0,0x8007,0x60);
 put(img,0,0x8010,0x8d);put(img,0,0x8011,0x00);put(img,0,0x8012,0x80);
 put(img,0,0x8013,0x20);put(img,0,0x8014,0x85);put(img,0,0x8015,0xef);put(img,0,0x8016,0x55); /* behind mapper barrier */
 put(img,1,0x8000,0x20);put(img,1,0x8001,0x85);put(img,1,0x8002,0xef);put(img,1,0x8003,0x66);put(img,1,0x8004,0x60);
 Gk143BankCfg r;CHECK(gk143_scan_bank_hypothesis(img,GK141_IMAGE_SIZE,0,&r)==GK141_OK);
 CHECK(r.assumed_boot_bank==0u&&r.vector_reset==0x8000u);
 CHECK(r.by_selector[0x42]==1u&&r.by_selector[0x55]==0u&&r.by_selector[0x66]==0u);
 CHECK(r.bank_write_barriers==1u&&r.decoded_low>0&&r.decoded_fixed>0);
 Gk143BankCfg one;CHECK(gk143_scan_bank_hypothesis(img,GK141_IMAGE_SIZE,1,&one)==GK141_OK);
 CHECK(one.by_selector[0x66]==1u&&one.by_selector[0x42]==0u);
 CHECK(one.bank_write_barriers==0u);
 Gk143BankCfg old=r;
 CHECK(gk143_scan_bank_hypothesis(img,GK141_IMAGE_SIZE-1u,0,&r)==GK141_BAD_INPUT);
 CHECK(memcmp(&old,&r,sizeof r)==0);
 CHECK(gk143_scan_bank_hypothesis(img,GK141_IMAGE_SIZE,15,&r)==GK141_BAD_INPUT);
 CHECK(memcmp(&old,&r,sizeof r)==0);
 CHECK(gk143_scan_bank_hypothesis(NULL,GK141_IMAGE_SIZE,0,&r)==GK141_BAD_INPUT);
 CHECK(memcmp(&old,&r,sizeof r)==0);
 CHECK(gk143_scan_authenticated("/not/here",0,&r)==GK141_SOURCE_UNVERIFIED);
 CHECK(memcmp(&old,&r,sizeof r)==0);
 Gk1App *app=calloc(1,sizeof *app),*saved=malloc(sizeof *app);
 CHECK(app&&saved);gk1_app_init(app);memcpy(saved,app,sizeof *app);
 CHECK(gk1_app_source_bank_cfg(app,"/not/here",0,&r)==GK141_BAD_INPUT);
 CHECK(memcmp(saved,app,sizeof *app)==0);
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;memcpy(saved,app,sizeof *app);
 CHECK(gk1_app_source_bank_cfg(app,"/not/here",0,&r)==GK141_SOURCE_UNVERIFIED);
 CHECK(memcmp(saved,app,sizeof *app)==0);
 if(argc==3&&strcmp(argv[1],"--private-stock")==0){
  /* Do not print or copy any original ROM bytes. The fixed-bank scanner
   * couldn't reach the reset root in its bank; here test ALL hypotheses. */
  for(unsigned bank=0;bank<15u;bank++){
   CHECK(gk143_scan_authenticated(argv[2],(uint8_t)bank,&r)==GK141_OK);
   CHECK(r.assumed_boot_bank==bank);
   printf("VERIFIED_STATIC bank-hypothesis=%u instructions=%u low=%u fixed=%u inline-dispatch=%u mapper-barriers=%u indirect=%u\n",
    bank,r.decoded_instructions,r.decoded_low,r.decoded_fixed,r.dispatch_candidates,r.bank_write_barriers,r.unknown_indirect_jumps);
  }
  CHECK(gk1_app_source_bank_cfg(app,argv[2],0,&r)==GK141_OK);
  CHECK(memcmp(saved,app,sizeof *app)==0);
 }
 free(app);free(saved);free(img);
 printf("GK143 PASS %u assertions (hypotheses NOT Mesen runtime)\n",tests);return 0;
}
