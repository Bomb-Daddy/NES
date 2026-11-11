#include "gk181_bank_seed_trace.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do{checks++;if(!(x)){fprintf(stderr,"GK186 FAILED %d %s\n",__LINE__,#x);return 1;}}while(0)
static int checks;
static void emit(uint8_t *rom,unsigned bank,unsigned pc,const uint8_t *p,size_t n){
 size_t offset=16u+(pc>=0xc000u?15u:bank)*16384u+(pc&0x3fffu);
 memcpy(rom+offset,p,n);
}
int main(int argc,char **argv){
 uint8_t *image=calloc(1,GK180_ROM_BYTES); CK(image!=NULL);
 memcpy(image,"NES\032",4);image[4]=16u;image[6]=0x12u;
 for(unsigned b=0;b<16;b++)memset(image+16u+b*16384u,0x60,16384u);
 static const uint8_t reset[]={0x20,0x00,0x81,0x20,0x89,0xc2,0x60};
 static const uint8_t low[]={0x20,0x89,0xc2,0x60};
 static const uint8_t nmi[]={0x20,0x89,0xc2,0x60};
 static const uint8_t irq[]={0x60};
 emit(image,0,0xc100,reset,sizeof reset);
 emit(image,0,0x8100,low,sizeof low);
 emit(image,1,0x8100,irq,sizeof irq);
 emit(image,0,0xc200,nmi,sizeof nmi);
 emit(image,0,0xc300,irq,sizeof irq);
 unsigned at=16u+15u*16384u;
 image[at+0x3ffc]=0x00;image[at+0x3ffd]=0xc1;
 image[at+0x3ffa]=0x00;image[at+0x3ffb]=0xc2;
 image[at+0x3ffe]=0x00;image[at+0x3fff]=0xc3;
 Gk181Map *r=calloc(1,sizeof *r);CK(r!=NULL);
 CK(gk181_scan_image(image,GK180_ROM_BYTES,0,r)==GK181_OK);
 CK(r->rng_call_count==3u && r->fixed_rng_call_count==2u && r->low_rng_call_count==1u);
 int saw_low=0,saw_fixed=0,saw_nmi=0;
 for(unsigned i=0;i<r->rng_call_count;i++){
  Gk181RngCall *x=&r->rng_calls[i];
  if(x->pc==0x8100u){CK(x->bank==0u&&x->roots==1u);saw_low=1;}
  if(x->pc==0xc103u){CK(x->bank==15u&&x->roots==1u);saw_fixed=1;}
  if(x->pc==0xc200u){CK(x->bank==15u&&x->roots==2u);saw_nmi=1;}
 }
 CK(saw_low&&saw_fixed&&saw_nmi);
 CK(gk181_scan_image(image,GK180_ROM_BYTES,1,r)==GK181_OK);
 CK(r->rng_call_count==2u&&r->fixed_rng_call_count==2u&&r->low_rng_call_count==0u);
 /* An out-of-control-flow raw JSR pattern is DATA, never a validated call. */
 emit(image,1,0x8120,low,sizeof low);
 CK(gk181_scan_image(image,GK180_ROM_BYTES,1,r)==GK181_OK);
 CK(r->rng_call_count==2u);
 Gk181Map *unchanged=malloc(sizeof *r);CK(unchanged!=NULL);
 memcpy(unchanged,r,sizeof *r);
 CK(gk181_scan_image(image,GK180_ROM_BYTES-1u,1,r)==GK181_BAD_INPUT);
 CK(memcmp(unchanged,r,sizeof *r)==0);
 CK(gk181_scan_authenticated("/invalid/original.nes",0,r)==GK181_SOURCE);
 CK(memcmp(unchanged,r,sizeof *r)==0);
 if(argc==2){
  unsigned total=0;
  for(unsigned b=0;b<15;b++){
   CK(gk181_scan_authenticated(argv[1],(uint8_t)b,r)==GK181_OK);
   CK(r->assumed_low_bank==b);
   CK((unsigned)r->fixed_rng_call_count+(unsigned)r->low_rng_call_count==r->rng_call_count);
   for(unsigned j=0;j<r->rng_call_count;j++)
    CK(r->rng_calls[j].roots!=0u&&r->rng_calls[j].bank<16u);
   total+=(unsigned)r->rng_call_count;
   printf("VERIFIED_STATIC ORIGINAL_BANK=%u conditional_rng_calls=%u fixed=%u low=%u\n",
     b,(unsigned)r->rng_call_count,(unsigned)r->fixed_rng_call_count,(unsigned)r->low_rng_call_count);
  }
  printf("VERIFIED_STATIC total CONDITIONAL mapped JSR candidates=%u, NOT executed Mesen history\n",total);
 }
 free(unchanged);free(r);free(image);
 printf("GK186 %d assertions PASS\n",checks);
 return 0;
}
