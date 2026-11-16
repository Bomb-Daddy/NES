#include "gk331_stock_offer_rng.h"
#include "gk61_market.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
static unsigned tested=0;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAILED line %d: %s\n",__LINE__,#x);return 2;}tested++;}while(0)
static uint64_t to_num(const uint8_t a[6]){uint64_t n=0;for(int i=0;i<6;i++)n=(n<<8)|a[i];return n;}
static void seed_bytes(uint64_t n,uint8_t a[6]){for(int i=5;i>=0;i--){a[i]=(uint8_t)n;n>>=8;}}
static uint64_t ref_step(uint64_t n){return (0ULL-((n|1u)*0xe0900e0187c5ULL))&0xffffffffffffULL;}
static unsigned roll(uint64_t *n,unsigned mod){*n=ref_step(*n);return (unsigned)(((*n>>32)&0x7fffULL)%mod);}
/* Independent reference consumes the caller's PRIVATE original-ROM data,
 * never a serialized proprietary price/probability table in public tests. */
static uint8_t sells[3][11],buys[3][17];
static int read_reference(const char *rom){
 FILE *f=fopen(rom,"rb");if(!f)return 0;
 if(fseek(f,16L+16384L+0x2fb2L,SEEK_SET)){
  fclose(f);return 0;
 }
 for(unsigned m=0;m<3;m++)for(unsigned i=0;i<11;i++){
  int lo=fgetc(f),hi=fgetc(f);
  if(lo<0||hi!=0||lo>100){fclose(f);return 0;}
  sells[m][i]=(uint8_t)lo;
 }
 for(unsigned m=0;m<3;m++)for(unsigned i=0;i<17;i++){
  int lo=fgetc(f),hi=fgetc(f);
  if(lo<0||hi!=0||lo>100){fclose(f);return 0;}
  buys[m][i]=(uint8_t)lo;
 }
 return fclose(f)==0;
}
static int check_seed(const char *rom,uint64_t initial){
 uint8_t seed[6];seed_bytes(initial,seed);Gk331Offers got={0};
 CHECK(gk331_offer_masks(rom,seed,&got)==GK331_OK);
 CHECK(got.draws==84u&&got.source_verified&&got.offers_generated&&
       !got.merchant_presence_known);
 uint64_t ref=initial;
 for(unsigned m=0;m<3;m++){
  unsigned force=roll(&ref,11u),s=0;
  for(unsigned j=0;j<11;j++){
   unsigned expected=j==force?1u:roll(&ref,100u)<sells[m][j];
   CHECK(got.sell[m][j]==expected);s+=got.sell[m][j];
  }
  CHECK(s>0u);
  force=roll(&ref,17u);s=0;
  for(unsigned j=0;j<17;j++){
   unsigned expected=j==force?1u:roll(&ref,100u)<buys[m][j];
   CHECK(got.buy[m][j]==expected);s+=got.buy[m][j];
  }
  CHECK(s>0u);
 }
 CHECK(to_num(got.final_seed)==ref);
 Gk331Offers second={0};CHECK(gk331_offer_masks(rom,seed,&second)==GK331_OK);
 CHECK(memcmp(&got,&second,sizeof got)==0);
 return 0;
}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 const char *rom=argv[1];
 CHECK(read_reference(rom));
 for(unsigned k=0;k<40;k++)if(check_seed(rom,(uint64_t)k*0x123456789abULL))return 2;
 uint8_t fixed[6]={1,2,3,4,5,6};Gk331Offers unchanged;memset(&unchanged,0xa5,sizeof unchanged);
 Gk331Offers saved=unchanged;
 CHECK(gk331_offer_masks(rom,NULL,&unchanged)==GK331_BAD);
 CHECK(memcmp(&unchanged,&saved,sizeof saved)==0);
 CHECK(gk331_offer_masks(NULL,fixed,&unchanged)==GK331_BAD);
 CHECK(memcmp(&unchanged,&saved,sizeof saved)==0);
 CHECK(gk331_offer_masks("/file/does/not/exist",fixed,&unchanged)==GK331_UNVERIFIED_ROM);
 CHECK(memcmp(&unchanged,&saved,sizeof saved)==0);
 char path[512];snprintf(path,sizeof path,"/tmp/gk331_wrongrom_%ld.nes",(long)getpid());
 FILE *f=fopen(rom,"rb");CHECK(f!=NULL);FILE *g=fopen(path,"wb");CHECK(g!=NULL);
 unsigned char b[4096];size_t n;unsigned byte=0;
 while((n=fread(b,1,sizeof b,f))!=0){if(byte==0)b[16]^=1;CHECK(fwrite(b,1,n,g)==n);byte++;}
 CHECK(fclose(f)==0);CHECK(fclose(g)==0);
 CHECK(gk331_offer_masks(path,fixed,&unchanged)==GK331_UNVERIFIED_ROM);
 CHECK(memcmp(&unchanged,&saved,sizeof saved)==0);remove(path);
 printf("GK331_ORIGINAL_STOCK_OFFER_SOURCE_PASS assertions=%u seeds=40 draws_per_seed=84 no_merchants_fabricated\n",tested);
 return 0;
}
