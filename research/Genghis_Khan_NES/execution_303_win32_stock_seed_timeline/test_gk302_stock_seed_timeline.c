#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"GK302 fail line %d: %s\n",__LINE__,#c);return 1;} checks++; } while(0)
static void store(Gk302SeedEvent *e,uint64_t cyc,uint16_t pc,
 uint8_t dst,uint8_t before,uint8_t after){
 memset(e,0,sizeof *e);
 e->kind=GK302_STORE;e->cpu_cycle=cyc;e->source_pc=pc;
 e->bank=15u;e->target=dst;e->before=before;e->after=after;
}
static uint16_t draw(Gk302SeedEvent *e,uint64_t cyc,const uint8_t seed[6]){
 memset(e,0,sizeof *e);e->cpu_cycle=cyc;e->kind=GK302_C289;
 memcpy(e->rng_before,seed,6u);
 Gk137StockRng g={0};uint16_t value=0;
 if(gk137_from_nes_bytes(&g,seed)!=GK137_OK||
    gk137_draw15(&g,&value)!=GK137_OK)return UINT16_MAX;
 e->draw15=value;gk137_to_nes_bytes(&g,e->rng_after);
 return value;
}
int main(int argc,char **argv){
 unsigned checks=0;
 CHECK(argc==2||argc==3);
 if(argc==2){
  const uint8_t dummy[6]={0};Gk302SeedEvent e={0};Gk302SeedResult out={0};
  CHECK(gk302_replay_stock_seed_timeline(NULL,dummy,&e,1u,&out)==GK302_ARGUMENT);
  CHECK(gk302_replay_stock_seed_timeline("/private/missing",NULL,&e,1u,&out)==GK302_ARGUMENT);
  CHECK(gk302_replay_stock_seed_timeline("/private/missing",dummy,&e,0u,&out)==GK302_ARGUMENT);
  CHECK(gk302_replay_stock_seed_timeline("/private/missing",dummy,&e,1u,&out)==GK302_ROM);
  printf("GK302 PASS %u public negative-control assertions; original-ROM gated tests not run\n",checks);
  return 0;
 }
 const uint8_t seed[6]={0x04,0xd2,0x04,0xd2,0x5a,0x7d};
 Gk302SeedEvent ev[7]={0};
 store(&ev[0],100u,0xc0bau,0x84u,0x04u,0x04u);
 store(&ev[1],200u,0xc0bdu,0x86u,0x04u,0x04u);
 store(&ev[2],300u,0xc0c2u,0x85u,0xd2u,0xd2u);
 store(&ev[3],400u,0xc0c5u,0x87u,0xd2u,0xd2u);
 ev[4].kind=GK302_NMI;ev[4].cpu_cycle=500u;ev[4].gate=0u;
 uint8_t pre[6]={0x04,0xd4,0x04,0xd4,0x5a,0x7d};
 uint16_t val=draw(&ev[5],600u,pre);CHECK(val<=32767u);
 ev[6].kind=GK302_NMI;ev[6].cpu_cycle=700u;ev[6].gate=7u;
 Gk302SeedResult result,sentinel;memset(&result,0xa5,sizeof result);sentinel=result;
 CHECK(gk302_replay_stock_seed_timeline(NULL,seed,ev,7,&result)==GK302_ARGUMENT);
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,NULL,7,&result)==GK302_ARGUMENT);
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,0,&result)==GK302_ARGUMENT);
 CHECK(!memcmp(&result,&sentinel,sizeof result));
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_OK);
 CHECK(result.event_count==7u&&result.seed_stores==4u&&result.c289_calls==1u);
 CHECK(result.nmi_enabled==1u&&result.nmi_gated==1u);
 CHECK(result.source_store_targets_mask==0x0fu);
 CHECK(!memcmp(result.final,ev[5].rng_after,6u));
 CHECK(result.original_rom_sha256_verified&&result.source_claims_only);
 CHECK(!result.original_first_spring_generated&&!result.original_orders_granted);
 /* Source-authenticated different history: a gated NMI changes pre-C289
  * independently of the same reset writers and identical initial tail. */
 ev[4].gate=1u;CHECK(draw(&ev[5],600u,seed)<=32767u);
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_OK);
 CHECK(result.nmi_gated==2u&&result.nmi_enabled==0u);
 CHECK(!memcmp(result.final,ev[5].rng_after,6u));
 ev[4].gate=0u;CHECK(draw(&ev[5],600u,pre)<=32767u);
 memset(&result,0xa5,sizeof result);sentinel=result;
 ev[2].cpu_cycle=100u;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_ORDER);
 CHECK(!memcmp(&result,&sentinel,sizeof result));ev[2].cpu_cycle=300u;
 ev[1].before^=1u;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_VALUE);
 CHECK(!memcmp(&result,&sentinel,sizeof result));ev[1].before^=1u;
 ev[0].source_pc=0xc0bdu;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_OPCODE);
 CHECK(!memcmp(&result,&sentinel,sizeof result));ev[0].source_pc=0xc0bau;
 ev[0].bank=1u;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_OPCODE);
 CHECK(!memcmp(&result,&sentinel,sizeof result));ev[0].bank=15u;
 ev[5].draw15^=1u;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_RNG);
 CHECK(!memcmp(&result,&sentinel,sizeof result));ev[5].draw15^=1u;
 ev[5].rng_before[5]^=1u;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,ev,7,&result)==GK302_VALUE);
 CHECK(!memcmp(&result,&sentinel,sizeof result));ev[5].rng_before[5]^=1u;
 CHECK(gk302_replay_stock_seed_timeline(argv[1],seed,ev,7,&result)==GK302_ROM);
 CHECK(!memcmp(&result,&sentinel,sizeof result));
 /* >64 source STORE events require exact source-attested chunking. */
 Gk302SeedEvent *longlog=calloc(133u,sizeof *longlog);CHECK(longlog!=NULL);
 for(size_t i=0;i<132;i++)store(&longlog[i],(i+1u)*100u,0xc0bau,0x84u,0x04u,0x04u);
 longlog[132].kind=GK302_NMI;longlog[132].cpu_cycle=13300u;longlog[132].gate=1u;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,longlog,133u,&result)==GK302_OK);
 CHECK(result.seed_stores==132u&&result.source_store_targets_mask==1u);
 longlog[100].source_pc=0xc0bdu;
 memset(&result,0xa5,sizeof result);sentinel=result;
 CHECK(gk302_replay_stock_seed_timeline(argv[2],seed,longlog,133u,&result)==GK302_OPCODE);
 CHECK(!memcmp(&result,&sentinel,sizeof result));free(longlog);
 /* The SAME inherited application, both scenarios, unchanged PRE_SETUP. */
 Gk1TemplateDataset *templates=malloc(sizeof *templates);
 Gk1App *app=malloc(sizeof *app),*before=malloc(sizeof *before);
 CHECK(templates&&app&&before);
 CHECK(gk1_template_load(argv[1],templates)==GK1_LOAD_OK);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,templates));
 app->rules_profile=0u;
 for(unsigned w=0;w<2u;w++){
  app->active_world=(uint8_t)w;app->setup_stage[w]=GK53_SETUP_AWAIT_SOURCE;
  app->setup_difficulty[w]=3u;memcpy(before,app,sizeof *app);
  CHECK(gk1_app_original_seed_timeline(app,argv[2],seed,ev,7,&result)==GK302_OK);
  CHECK(result.source_claims_only&&!result.original_first_spring_generated);
  CHECK(!memcmp(before,app,sizeof *app));
  app->rules_profile=1u;
  CHECK(gk1_app_original_seed_timeline(app,argv[2],seed,ev,7,&result)==GK302_STAGE);
  CHECK(memcmp(before,app,sizeof *app)!=0); /* profile intentionally changed */
  app->rules_profile=0u;
 }
 free(before);free(app);free(templates);
 printf("GK302 PASS %u assertions: real-ROM source-attested NMI + STORE + C289 chronology, multi-chunk replay, two scenario application gates; no original first Spring fabricated\n",checks);
 return 0;
}
