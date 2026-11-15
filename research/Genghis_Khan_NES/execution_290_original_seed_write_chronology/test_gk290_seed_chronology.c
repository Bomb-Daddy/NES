#include "gk1_app_state.h"
#include "gk290_seed_chronology.h"
#include "gk137_stock_rng.h"
#include "gk1_source_identity.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"GK290 FAIL line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static void seed_writes(Gk290Event e[],uint8_t seed[6]){
 for(unsigned i=0;i<6;i++){
  e[i]=(Gk290Event){0};e[i].kind=GK290_WRITE;
  e[i].cycle=i+1u;e[i].pc=0x8030u;
  e[i].bank_known=1u;e[i].bank=0u;
  e[i].address=(uint16_t)(0x84u+i);e[i].value=seed[i];
 }
}
static int add_call(Gk290Event *e,uint64_t cycle,uint8_t seed[6]){
 Gk137StockRng rng={0};uint16_t draw=0;
 if(gk137_from_nes_bytes(&rng,seed)!=GK137_OK||
    gk137_draw15(&rng,&draw)!=GK137_OK)return 0;
 memset(e,0,sizeof *e);e->kind=GK290_CALL;
 e->cycle=cycle;e->pc=0xc289u;e->bank=15;e->bank_known=1;
 memcpy(e->before,seed,6u);e->draw15=draw;
 gk137_to_nes_bytes(&rng,e->after);
 memcpy(seed,e->after,6u);return 1;
}
int main(int argc,char **argv){
 unsigned checks=0;CK(argc==2);
 CK(gk1_verified_original_csv(argv[1]));
 Gk290Event e[10]={0};
 uint8_t s[6]={0x12,0x23,0x34,0x45,0x56,0x67};
 seed_writes(e,s);CK(add_call(&e[6],7u,s));
 e[7]=(Gk290Event){0};e[7].kind=GK290_WRITE;
 e[7].cycle=8u;e[7].pc=0x9a20u;e[7].bank=2;e[7].bank_known=1u;
 e[7].address=0x86u;e[7].value=0x9fu;s[2]=0x9fu;
 CK(add_call(&e[8],9u,s));CK(add_call(&e[9],10u,s));
 Gk290Report report;memset(&report,0x77,sizeof report);
 CK(gk290_verify_written_seed_timeline(e,10u,&report)==GK290_OK);
 CK(report.observed_seed_writes==7u);
 CK(report.interleaved_seed_writes==1u);
 CK(report.validated_c289_calls==3u);
 CK(report.first_draw_cycle==7u&&report.last_draw_cycle==10u);
 CK(report.six_source_writes_before_first_draw&&report.source_math_parity);
 CK(!report.real_mesen_proven&&!report.stock_first_spring_generated&&
    !report.orders_granted);
 CK(memcmp(report.final_seed,e[9].after,6u)==0);
 CK(memcmp(report.first_explicit_seed,e[6].before,6u)==0);
 Gk290Report original=report;
 CK(gk290_verify_written_seed_timeline(NULL,10,&report)==GK290_ARGUMENT);
 CK(gk290_verify_written_seed_timeline(e,0,&report)==GK290_ARGUMENT);
 CK(gk290_verify_written_seed_timeline(e,GK290_MAX_EVENTS+1,&report)==GK290_LIMIT);
 CK(!memcmp(&original,&report,sizeof report));
 Gk290Event n[10];memcpy(n,e,sizeof n);
 n[0].cycle=0u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 n[0]=e[0];n[1].cycle=n[0].cycle;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 n[1]=e[1];n[2].address=0x87u; /* missing $86 first write */
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_SEED_UNPROVEN);
 n[2]=e[2];n[2].bank_known=0u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 n[2]=e[2];n[2].pc=0x2000u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 n[2]=e[2];n[2].address=0x2006u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 n[2]=e[2];n[6].before[2]^=1u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_PARITY);
 n[6]=e[6];n[8].draw15^=1u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_PARITY);
 n[8]=e[8];n[9].after[4]^=1u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_PARITY);
 n[9]=e[9];n[7].value^=1u; /* hidden unobserved RNG state */
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_PARITY);
 n[7]=e[7];n[8].pc=0xc288u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 n[8]=e[8];n[9].bank=14u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 n[9]=e[9];n[9].cycle=9u;
 CK(gk290_verify_written_seed_timeline(n,10,&report)==GK290_CHRONOLOGY);
 CK(!memcmp(&original,&report,sizeof report));
 /* Catch forged traces that omit all six actual seed-writer events. */
 CK(gk290_verify_written_seed_timeline(e+6,4u,&report)==GK290_SEED_UNPROVEN);
 CK(!memcmp(&original,&report,sizeof report));
 /* Native game control: previous source setup is NOT advanced by an audit. */
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 Gk1App *app=calloc(1,sizeof *app),*old=malloc(sizeof *old);
 CK(ds&&app&&old);gk1_app_init(app);
 CK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(app,ds));
 app->active_world=0;app->rules_profile=0;
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[0]=3;
 memcpy(old,app,sizeof *old);
 CK(gk1_app_original_seed_chronology(app,e,10,&report)==GK290_OK);
 CK(!memcmp(app,old,sizeof *app));
 CK(!report.stock_first_spring_generated&&!report.orders_granted);
 app->rules_profile=1u;
 CK(gk1_app_original_seed_chronology(app,e,10,&report)==GK290_CHRONOLOGY);
 app->rules_profile=0u;app->worlds[0].countries[0].gold++;
 CK(gk1_app_original_seed_chronology(app,e,10,&report)==GK290_CHRONOLOGY);
 CK(!memcmp(&original,&report,sizeof report));
 free(ds);free(app);free(old);
 printf("GK290 %u source-seed timeline assertions PASS; actual stock Mesen NOT RUN; no Spring/turn unlocked\n",checks);
 return 0;
}
