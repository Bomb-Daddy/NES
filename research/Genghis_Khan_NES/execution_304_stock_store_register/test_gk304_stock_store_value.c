#include "gk1_app_state.h"
#include "gk304_stock_store_value.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK304 failed line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static void store(Gk302SeedEvent *e,uint64_t cyc,uint16_t pc,
 uint8_t dst,uint8_t before,uint8_t after){
 memset(e,0,sizeof *e);e->kind=GK302_STORE;e->cpu_cycle=cyc;
 e->source_pc=pc;e->bank=15u;e->target=dst;
 e->before=before;e->after=after;
}
static void witness(Gk304Registers *r,const Gk302SeedEvent *e){
 memset(r,0,sizeof *r);r->cpu_cycle=e->cpu_cycle;r->pc=e->source_pc;
 r->mapped_prg_bank=e->bank;r->destination=e->target;
 r->a=e->after;r->x=(uint8_t)(e->after^0xffu);r->y=0x55u;
}
static int draw(Gk302SeedEvent *e,uint64_t cycle,const uint8_t seed[6]){
 memset(e,0,sizeof *e);e->kind=GK302_C289;e->cpu_cycle=cycle;
 memcpy(e->rng_before,seed,6u);
 Gk137StockRng r={0};uint16_t value=0;
 if(gk137_from_nes_bytes(&r,seed)!=GK137_OK||
    gk137_draw15(&r,&value)!=GK137_OK)return 0;
 e->draw15=value;gk137_to_nes_bytes(&r,e->rng_after);return 1;
}
int main(int argc,char **argv){
 unsigned checks=0u;
 CHECK(argc==2||argc==3);
 const uint8_t seed[6]={0x04u,0xd2u,0x04u,0xd2u,0x5au,0x7du};
 Gk302SeedEvent e[7]={0};Gk304Registers reg[4]={0};
 store(&e[0],100u,0xc0bau,0x84u,0x04u,0x04u);
 store(&e[1],200u,0xc0bdu,0x86u,0x04u,0x04u);
 store(&e[2],300u,0xc0c2u,0x85u,0xd2u,0xd2u);
 store(&e[3],400u,0xc0c5u,0x87u,0xd2u,0xd2u);
 for(unsigned j=0;j<4;j++)witness(&reg[j],&e[j]);
 e[4].kind=GK302_NMI;e[4].cpu_cycle=500u;e[4].gate=0u;
 const uint8_t post_nmi[6]={0x04u,0xd4u,0x04u,0xd4u,0x5au,0x7du};
 CHECK(draw(&e[5],600u,post_nmi));
 e[6].kind=GK302_NMI;e[6].cpu_cycle=700u;e[6].gate=7u;
 Gk304Report result,untouched;memset(&result,0xa5,sizeof result);
 untouched=result;
 CHECK(gk304_verify_store_values(NULL,seed,e,7u,reg,4u,&result)==GK304_ARGUMENT);
 CHECK(gk304_verify_store_values("/private/missing",seed,e,7u,reg,4u,&result)==GK304_STOCK_ROM);
 CHECK(gk304_verify_store_values("/private/missing",seed,e,7u,reg,0u,&result)==GK304_ARGUMENT);
 CHECK(!memcmp(&result,&untouched,sizeof result));
 if(argc==2){printf("GK304 public no-ROM refusal PASS %u checks (private ROM gates not run)\n",checks);return 0;}
 const char *rom=argv[2];
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_OK);
 CHECK(result.register_verified_writes==4u&&result.seed.seed_stores==4u);
 CHECK(result.sta_count_nonzero&&result.authentic_original_rom_verified);
 CHECK(result.claimed_register_consistency_only&&!result.original_stock_emulator_executed);
 CHECK(!result.playable_spring_created&&!result.orders_granted);
 CHECK(!memcmp(result.seed.final,e[5].rng_after,6u));
 /* Critical negative control: EXEC0302 accepted a claimed STORE value
  * disconnected from the CPU's A register. EXEC0304 must reject it. */
 e[3].after=0x73u;
 const uint8_t forged[6]={0x04u,0xd4u,0x04u,0x75u,0x5au,0x7du};
 CHECK(draw(&e[5],600u,forged));
 memset(&result,0xa5,sizeof result);untouched=result;
 CHECK(gk302_replay_stock_seed_timeline(rom,seed,e,7u,&result.seed)==GK302_OK);
 memset(&result,0xa5,sizeof result);untouched=result;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_VALUE);
 CHECK(!memcmp(&result,&untouched,sizeof result));e[3].after=0xd2u;
 CHECK(draw(&e[5],600u,post_nmi));
 /* No imaginary 'other register' may substitute for accumulator STA. */
 reg[1].a^=1u;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_VALUE);
 CHECK(!memcmp(&result,&untouched,sizeof result));reg[1].a^=1u;
 reg[1].x^=1u;CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_OK);reg[1].x^=1u;
 reg[0].pc^=1u;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_VALUE);
 reg[0].pc^=1u;
 reg[0].cpu_cycle++;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_VALUE);
 reg[0].cpu_cycle--;
 reg[0].mapped_prg_bank=14u;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_VALUE);
 reg[0].mapped_prg_bank=15u;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,3u,&result)==GK304_REGISTER_COUNT);
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,5u,&result)==GK304_REGISTER_COUNT);
 reg[0].destination=0x88u;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_VALUE);
 reg[0].destination=0x84u;
 /* Timeline source and RNG refusals remain intact. */
 e[1].source_pc=0xc0bau;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_TIMELINE);
 e[1].source_pc=0xc0bdu;
 e[5].draw15^=1u;
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_TIMELINE);
 e[5].draw15^=1u;
 e[4].gate=1u;CHECK(draw(&e[5],600u,seed));
 CHECK(gk304_verify_store_values(rom,seed,e,7u,reg,4u,&result)==GK304_OK);
 CHECK(result.seed.nmi_gated==2u&&result.seed.nmi_enabled==0u);
 e[4].gate=0u;CHECK(draw(&e[5],600u,post_nmi));
 /* Both original country scenarios must remain pristine PRE_SETUP and no
  * private observation becomes an automatic player-ready season. */
 Gk1App *app=malloc(sizeof *app),*baseline=malloc(sizeof *baseline);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 CHECK(app&&baseline&&dataset);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,dataset));
 app->rules_profile=0u;
 for(unsigned w=0;w<2;w++){
  app->active_world=(uint8_t)w;app->setup_stage[w]=GK53_SETUP_AWAIT_SOURCE;
  app->setup_difficulty[w]=3u;
  memcpy(baseline,app,sizeof *app);
  CHECK(gk1_app_original_store_values(app,rom,seed,e,7u,reg,4u,&result)==GK304_OK);
  CHECK(result.claimed_register_consistency_only&&!result.playable_spring_created);
  CHECK(!memcmp(baseline,app,sizeof *app));
  app->rules_profile=1u;
  CHECK(gk1_app_original_store_values(app,rom,seed,e,7u,reg,4u,&result)==GK304_STAGE);
  app->rules_profile=0u;
 }
 free(app);free(baseline);free(dataset);
 printf("GK304 PASS %u real-original-ROM assertions including falsely claimed CPU accumulator STORE, chronology and 14/27 PRE_SETUP guards; no Spring minted\n",checks);
 return 0;
}
