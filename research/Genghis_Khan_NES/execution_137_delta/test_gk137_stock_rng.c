#include "gk137_stock_rng.h"
#include "gk1_app_state.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
/* Independent base-256 48-bit multiplication oracle, not using the same
 * unsigned-64 multiply as production. Refuses neither carries nor top bytes. */
static uint64_t oracle(uint64_t state){
 uint8_t a[6],b[6],product[6]={0};
 uint64_t odd=state|1u;
 for(unsigned i=0;i<6;i++){
  a[i]=(uint8_t)(odd>>(8u*i));
  b[i]=(uint8_t)(UINT64_C(0xe0900e0187c5)>>(8u*i));
 }
 unsigned carry=0;
 for(unsigned k=0;k<6;k++){
  unsigned t=carry;
  for(unsigned j=0;j<=k;j++)t+=(unsigned)a[j]*b[k-j];
  product[k]=(uint8_t)t;carry=t>>8u;
 }
 uint64_t p=0;
 for(unsigned k=0;k<6;k++)p|=(uint64_t)product[k]<<(8u*k);
 return (UINT64_C(0)-p)&UINT64_C(0xffffffffffff);
}
static unsigned checks;
#define CHECK(c) do{checks++;if(!(c)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);abort();}}while(0)
int main(void){
 static const uint64_t seeds[]={0,1,2,3,0x123456789abcULL,0xffffffffffffULL,
  0x800000000000ULL,0x10000ULL,0x00000000008aULL,0x010203040506ULL};
 for(unsigned k=0;k<sizeof seeds/sizeof seeds[0];k++){
  uint8_t bytes[6];Gk137StockRng r={seeds[k],0};
  gk137_to_nes_bytes(&r,bytes);
  Gk137StockRng roundtrip={0};
  CHECK(gk137_from_nes_bytes(&roundtrip,bytes)==GK137_OK);
  CHECK(roundtrip.seed48==r.seed48&&roundtrip.draws==0);
  for(unsigned j=0;j<1000;j++){
   uint64_t want=oracle(r.seed48);
   uint16_t out=UINT16_MAX;
   CHECK(gk137_draw15(&r,&out)==GK137_OK);
   CHECK(r.seed48==want&&r.draws==j+1u);
   CHECK(out==((want>>32)&0x7fffu));
  }
 }
 for(unsigned difficulty=1;difficulty<=5;difficulty++){
  for(unsigned seed=0;seed<1024;seed++){
   Gk137StockRng r={seed,0};uint16_t rate=0;
   uint64_t expected=oracle(seed);
   CHECK(gk137_spring_rate(&r,(uint8_t)difficulty,&rate)==GK137_OK);
   CHECK(rate==50u+(((expected>>32)&0x7fffu)%(94u+7u*difficulty)));
   CHECK(rate>=50u&&rate<=143u+7u*difficulty);
   CHECK(r.draws==1u);
  }
 }
 for(unsigned starting=50;starting<=999;starting++){
  Gk137StockRng r={starting,0};uint8_t p=0;uint16_t rate=0;
  uint64_t expected=oracle(starting);
  uint8_t pct=(uint8_t)(1u+(((expected>>32)&0x7fffu)%9u));
  uint16_t declined=(uint16_t)(starting-(starting*pct)/100u);
  CHECK(gk137_rate_decline_one_draw(&r,(uint16_t)starting,&rate,&p)==GK137_OK);
  CHECK(p==pct&&rate==(declined<50?50:declined));
  CHECK(r.draws==1u);
 }
 /* Reject and zero-modulo cannot advance state or alter output. */
 Gk137StockRng r={0x9876543210abULL,15},before=r;
 uint16_t v=1234;uint8_t percent=87;
 CHECK(gk137_bounded(&r,0,&v)==GK137_OK&&v==0);
 CHECK(!memcmp(&r,&before,sizeof r));
 v=1234;CHECK(gk137_spring_rate(&r,0,&v)==GK137_BAD_INPUT&&v==1234);
 CHECK(!memcmp(&r,&before,sizeof r));
 CHECK(gk137_spring_rate(&r,6,&v)==GK137_BAD_INPUT&&v==1234);
 CHECK(gk137_rate_decline_one_draw(&r,49,&v,&percent)==GK137_BAD_INPUT);
 CHECK(v==1234&&percent==87&&!memcmp(&r,&before,sizeof r));
 CHECK(gk137_rate_decline_one_draw(&r,1000,&v,&percent)==GK137_BAD_INPUT);
 CHECK(!memcmp(&r,&before,sizeof r));
 r.seed48=UINT64_MAX;before=r;
 CHECK(gk137_draw15(&r,&v)==GK137_BAD_INPUT&&!memcmp(&r,&before,sizeof r));
 CHECK(gk137_bounded(&r,0,&v)==GK137_BAD_INPUT&&!memcmp(&r,&before,sizeof r));
 r.seed48=1;r.draws=UINT64_MAX;before=r;
 CHECK(gk137_draw15(&r,&v)==GK137_BAD_INPUT&&!memcmp(&r,&before,sizeof r));
 CHECK(gk137_spring_rate(&r,1,&v)==GK137_BAD_INPUT&&!memcmp(&r,&before,sizeof r));
 CHECK(gk137_from_nes_bytes(NULL,(uint8_t[6]){0})==GK137_BAD_INPUT);
 CHECK(gk137_from_nes_bytes(&r,NULL)==GK137_BAD_INPUT);
 /* Exercise the actual 1280x720 C11 application's setup-stage boundary:
  * controlled explicit-seed calculations MUST NOT unlock Spring or mutate
  * any of the original PRE_SETUP world, orders, ruler or RNG state. */
 Gk1App app;gk1_app_init(&app);
 app.active_world=0;app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app.setup_difficulty[0]=3u;
 Gk1App untouched=app;Gk137StockRng receipt;
 uint8_t external_seed[6]={0x01,0x23,0x45,0x67,0x89,0xab};
 uint16_t first=0;
 CHECK(gk1_app_source_rate_proof(&app,external_seed,&receipt,&first)==GK137_OK);
 CHECK(receipt.draws==1u&&first>=50u&&first<=164u);
 CHECK(!memcmp(&app,&untouched,sizeof app));
 app.setup_stage[0]=GK53_SETUP_ABILITIES;untouched=app;first=333;
 CHECK(gk1_app_source_rate_proof(&app,external_seed,&receipt,&first)==GK137_BAD_INPUT);
 CHECK(first==333&&!memcmp(&app,&untouched,sizeof app));
 puts("GK137 independent 48-bit RNG/source-rate/rollback/app-boundary checks PASS");
 printf("ASSERTIONS %u\n",checks);
 return 0;
}
