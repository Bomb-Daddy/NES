#include "gk138_rng_reverse.h"
#include "gk137_stock_rng.h"
#include "gk1_app_state.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#define M48 UINT64_C(0xffffffffffff)
#define MULT UINT64_C(0xe0900e0187c5)
static uint64_t check_count;
#define CHECK(x) do {check_count++;if(!(x)){fprintf(stderr,"GK138 FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);abort();}} while(0)
/* Independent base-256 oracle, no 64-bit direct multiplication. */
static uint64_t forward(uint64_t seed){
 uint8_t a[6],b[6],prod[6];
 for(unsigned i=0;i<6;i++){
  a[i]=(uint8_t)((seed|UINT64_C(1))>>(8u*i));
  b[i]=(uint8_t)(MULT>>(8u*i));
 }
 uint32_t carry=0;
 for(unsigned i=0;i<6;i++){
  uint32_t sum=carry;
  for(unsigned j=0;j<=i;j++)sum+=(uint32_t)a[j]*b[i-j];
  prod[i]=(uint8_t)sum;carry=sum>>8u;
 }
 uint64_t p=0;
 for(unsigned i=0;i<6;i++)p|=(uint64_t)prod[i]<<(8u*i);
 return (UINT64_C(0)-p)&M48;
}
static void to_bytes(uint64_t seed,uint8_t bytes[6]){
 for(unsigned i=0;i<6;i++)bytes[i]=(uint8_t)(seed>>(8u*(5u-i)));
}
int main(void){
 const uint64_t s[]={0,1,2,3,0x123456789abcULL,0x123456789abdULL,
  0xffffffffffffULL,0xfffffffffffEULL,0x800000000000ULL,0x010203040506ULL};
 for(unsigned k=0;k<sizeof s/sizeof s[0];k++){
  uint64_t state=s[k];
  Gk138Reverse rev={0};
  CHECK(gk138_reverse(state,0,&rev)==GK138_OK);
  CHECK(rev.candidate_even==state&&rev.candidate_odd==state&&rev.draws_back==0);
  for(uint32_t n=1;n<=300;n++){
   state=forward(state);
   CHECK(gk138_reverse(state,n,&rev)==GK138_OK);
   CHECK(rev.draws_back==n);
   CHECK(rev.candidate_odd==(s[k]|UINT64_C(1)));
   CHECK(rev.candidate_even==(s[k]&~UINT64_C(1)));
   uint64_t p=rev.candidate_odd;
   for(uint32_t i=0;i<n;i++)p=forward(p);
   CHECK(p==state);
  }
 }
 /* The original s|1 rule means distinct initial even/odd seeds collide
  * after one call and there is NO way to infer exact original parity. */
 for(uint64_t seed=0;seed<400;seed+=2){
  CHECK(forward(seed)==forward(seed+1));
  Gk138Reverse trace={0};
  CHECK(gk138_reverse(forward(seed),1,&trace)==GK138_OK);
  CHECK(trace.candidate_even==seed&&trace.candidate_odd==seed+1);
 }
 /* Deterministic reverse-rate search should find precisely the matches in
  * an independent forward-generated history, not a manufactured causal hit. */
 for(unsigned k=0;k<160;k++){
  uint64_t hist[65],initial=((UINT64_C(0x123456789abc)+k*UINT64_C(0x101010101))&M48);
  hist[0]=initial;
  for(unsigned j=1;j<=64;j++)hist[j]=forward(hist[j-1]);
  uint8_t diff=(uint8_t)(1+k%5);const unsigned bound=94u+7u*diff;
  uint16_t wanted=(uint16_t)(50u+(((hist[17]>>32u)&0x7fffu)%bound));
  Gk138RateScan result={0};
  CHECK(gk138_scan_spring_rate(hist[64],diff,wanted,64,&result)==GK138_OK);
  uint32_t count=0,closest=UINT32_MAX;
  for(unsigned j=0;j<64;j++){
   uint64_t st=hist[64-j];
   uint16_t observed=(uint16_t)(50u+(((st>>32u)&0x7fffu)%bound));
   if(observed==wanted){count++;if(closest==UINT32_MAX)closest=j;}
  }
  CHECK(result.inspected==64&&result.matches==count&&result.nearest_offset==closest);
  CHECK(result.matches>=1);
 }
 /* All refusal paths must remain atomic, including outputs. */
 Gk138Reverse r={17,19,23},r0=r;
 CHECK(gk138_reverse(2,1,&r)==GK138_BAD_INPUT);
 CHECK(gk138_reverse(M48+1,0,&r)==GK138_BAD_INPUT);
 CHECK(gk138_reverse(1,65537,&r)==GK138_BAD_INPUT);
 CHECK(gk138_reverse(1,1,NULL)==GK138_BAD_INPUT);
 CHECK(!memcmp(&r,&r0,sizeof r));
 Gk138RateScan scan={3,8,13},scan0=scan;
 CHECK(gk138_scan_spring_rate(2,1,100,20,&scan)==GK138_BAD_INPUT);
 CHECK(gk138_scan_spring_rate(1,0,100,20,&scan)==GK138_BAD_INPUT);
 CHECK(gk138_scan_spring_rate(1,6,100,20,&scan)==GK138_BAD_INPUT);
 CHECK(gk138_scan_spring_rate(1,1,49,20,&scan)==GK138_BAD_INPUT);
 CHECK(gk138_scan_spring_rate(1,1,151,20,&scan)==GK138_BAD_INPUT);
 CHECK(gk138_scan_spring_rate(1,1,100,0,&scan)==GK138_BAD_INPUT);
 CHECK(gk138_scan_spring_rate(1,1,100,65537,&scan)==GK138_BAD_INPUT);
 CHECK(gk138_scan_spring_rate(1,1,100,20,NULL)==GK138_BAD_INPUT);
 CHECK(!memcmp(&scan,&scan0,sizeof scan));
 /* Same real game's PRE_SETUP gate, and proof cannot mutate countries or
  * turn synthetic source seed into playable orders. */
 Gk1App app;gk1_app_init(&app);
 app.active_world=0;app.setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app.setup_difficulty[0]=3;
 Gk1App old=app;
 uint8_t bytes[6];to_bytes(forward(0x112233445566ULL),bytes);
 CHECK(gk1_app_source_rate_history(&app,bytes,121,32,&scan)==GK138_OK);
 CHECK(!memcmp(&app,&old,sizeof app));
 app.setup_stage[0]=GK53_SETUP_ABILITIES;old=app;scan0=scan;
 CHECK(gk1_app_source_rate_history(&app,bytes,121,32,&scan)==GK138_BAD_INPUT);
 CHECK(!memcmp(&scan,&scan0,sizeof scan)&&!memcmp(&app,&old,sizeof app));
 puts("GK138 original 48-bit reverse/possible-rate history, collapse, bounded negative controls, application guard PASS");
 printf("ASSERTIONS %llu\n",(unsigned long long)check_count);
 return 0;
}
