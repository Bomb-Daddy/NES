#include "gk142_fixed_cfg.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(c) do{checks++;if(!(c)){fprintf(stderr,"GK142 FAIL at %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void put(uint8_t *b,uint16_t addr,uint8_t byte){b[16u+15u*16384u+(addr-0xc000u)]=byte;}
static void vector(uint8_t *b,unsigned slot,uint16_t pc){put(b,(uint16_t)(0xfffau+slot*2u),(uint8_t)pc);put(b,(uint16_t)(0xfffbu+slot*2u),(uint8_t)(pc>>8));}
int main(int argc,char**argv){
 uint8_t *b=(uint8_t*)calloc(1,GK141_IMAGE_SIZE);CHECK(b!=NULL);
 b[0]='N';b[1]='E';b[2]='S';b[3]=0x1a;b[4]=16;b[6]=0x12;
 vector(b,0,0xc110);vector(b,1,0xc100);vector(b,2,0xc120);
 put(b,0xc100,0x20);put(b,0xc101,0x85);put(b,0xc102,0xef);put(b,0xc103,0x02); /* inline selector must be skipped */
 put(b,0xc104,0xea);put(b,0xc105,0x60);
 put(b,0xc110,0x4c);put(b,0xc111,0x00);put(b,0xc112,0xc2);
 put(b,0xc120,0x40);
 put(b,0xc200,0x20);put(b,0xc201,0x85);put(b,0xc202,0xef);put(b,0xc203,0x08);
 put(b,0xc204,0x20);put(b,0xc205,0x10);put(b,0xc206,0xc2);put(b,0xc207,0x60);
 put(b,0xc210,0xd0);put(b,0xc211,0x02);put(b,0xc212,0xea);put(b,0xc213,0x60);
 put(b,0xc214,0x20);put(b,0xc215,0x85);put(b,0xc216,0xef);put(b,0xc217,0xfe);put(b,0xc218,0x60);
 /* This raw candidate is data, never reached from vectors. */
 put(b,0xc300,0x20);put(b,0xc301,0x85);put(b,0xc302,0xef);put(b,0xc303,0x33);
 /* This raw pattern is in operands and must not become a decoded JSR. */
 put(b,0xc310,0xa9);put(b,0xc311,0x20);put(b,0xc312,0x85);put(b,0xc313,0xef);
 /* EF85 subroutine returns; source observations are conditional. */
 put(b,0xef85,0x60);
 Gk141DispatchMap raw;CHECK(gk141_scan_image(b,GK141_IMAGE_SIZE,&raw)==GK141_OK);
 CHECK(raw.candidate_total==5u);
 Gk142FixedCfg cfg;CHECK(gk142_scan_fixed_cfg(b,GK141_IMAGE_SIZE,&cfg)==GK141_OK);
 CHECK(cfg.reachable_dispatch_calls==3u);
 CHECK(cfg.reachable_by_selector[2]==1u&&cfg.reachable_by_selector[8]==1u&&cfg.reachable_by_selector[254]==1u);
 CHECK(cfg.reachable_by_selector[0x33]==0u&&cfg.fixed_entrypoints==3u);
 CHECK(cfg.decoded_instructions>=11u);
 Gk142FixedCfg keep=cfg;
 CHECK(gk142_scan_fixed_cfg(b,GK141_IMAGE_SIZE-1u,&cfg)==GK141_BAD_INPUT);
 CHECK(memcmp(&cfg,&keep,sizeof cfg)==0);
 CHECK(gk142_scan_fixed_cfg(NULL,GK141_IMAGE_SIZE,&cfg)==GK141_BAD_INPUT);
 CHECK(memcmp(&cfg,&keep,sizeof cfg)==0);
 CHECK(gk142_scan_fixed_cfg_authenticated("/not/a/private/rom",&cfg)==GK141_SOURCE_UNVERIFIED);
 CHECK(memcmp(&cfg,&keep,sizeof cfg)==0);
 vector(b,1,0x8000);CHECK(gk142_scan_fixed_cfg(b,GK141_IMAGE_SIZE,&cfg)==GK141_OK);
 CHECK(cfg.dynamic_entries==1u&&cfg.fixed_entrypoints==2u&&cfg.reachable_dispatch_calls==2u);
 vector(b,1,0xc100);
 Gk1App *app=(Gk1App*)calloc(1,sizeof *app);CHECK(app!=NULL);
 gk1_app_init(app);app->active_world=0;app->setup_difficulty[0]=2;
 Gk1App *unchanged=malloc(sizeof *app);CHECK(unchanged!=NULL);
 memcpy(unchanged,app,sizeof *app);
 CHECK(gk1_app_source_fixed_cfg(app,"/not/a/private/rom",&cfg)==GK141_BAD_INPUT);
 CHECK(memcmp(unchanged,app,sizeof *app)==0);
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 memcpy(unchanged,app,sizeof *app);
 CHECK(gk1_app_source_fixed_cfg(app,"/not/a/private/rom",&cfg)==GK141_SOURCE_UNVERIFIED);
 CHECK(memcmp(unchanged,app,sizeof *app)==0);
 if(argc==3&&strcmp(argv[1],"--private-stock")==0){
  CHECK(gk142_scan_fixed_cfg_authenticated(argv[2],&cfg)==GK141_OK);
  printf("VERIFIED_STATIC original stock fixed bank vector CFG decoded=%u candidate-calls=%u switchable-roots=%u unresolved-targets=%u\n",cfg.decoded_instructions,cfg.reachable_dispatch_calls,cfg.dynamic_entries,cfg.switchable_targets);
  CHECK(gk1_app_source_fixed_cfg(app,argv[2],&cfg)==GK141_OK);
  CHECK(memcmp(unchanged,app,sizeof *app)==0);
 }
 free(unchanged);free(app);free(b);
 printf("GK142 PASS (%u assertions)\n",checks);return 0;
}
