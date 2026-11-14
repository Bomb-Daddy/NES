#include "gk282_stock_rng_trace.h"
#include "gk178_source_rng.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"EXEC0282 FAIL line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static void hexbytes(const uint8_t s[6],char out[13]){
 static const char d[]="0123456789abcdef";
 for(unsigned i=0;i<6;i++){out[i*2]=d[s[i]>>4];out[i*2+1]=d[s[i]&15u];}out[12]=0;
}
static int fixture(const char *rom,const char *path,int fault){
 FILE *fp=fopen(path,"wb");if(!fp)return 0;
 fprintf(fp,"GK1_C289_TRACE_V1,fresh-workram\n");
 uint8_t seed[6]={3,7,11,19,23,29};
 for(unsigned i=0;i<3;i++){
  Gk178RngResult source={0};
  if(gk178_step_from_authenticated_stock(rom,seed,&source)!=GK178_OK){fclose(fp);return 0;}
  char from[13],to[13];hexbytes(seed,from);hexbytes(source.after,to);
  unsigned draw=((unsigned)source.result_67<<8)|source.result_66;
  if(fault==1&&i==1)to[4]=to[4]=='0'?'1':'0';
  fprintf(fp,"C,%u,%u,%s,%s,%u\n",i+1,100u+i*50u,from,to,draw);
  if(i==2)fprintf(fp,"V,4,300,%u\n",50u+draw%(94u+7u));
  memcpy(seed,source.after,6);
 }
 if(fault==2)fputs("THIS_LINE_IS_FORGED\n",fp);
 if(fault==3)fputs("C,4,400,000000000000,000000000000,0\n",fp);
 return fclose(fp)==0;
}
int main(int argc,char **argv){
 int checks=0;
 CK(argc==3);
 const char *rom=argv[1],*trace=argv[2];
 Gk282Report r={0},out={0};
 if(!*rom){
  /* Public CI intentionally ships no private original ROM. Refusal is tested,
   * never treated as evidence that the authenticated original is missing. */
  CK(gk282_check_c289_trace(rom,trace,&out)==GK282_ARGUMENT);
  CK(out.calls==0u&&out.generated_first_spring==0u);
  printf("EXEC0282 public ROM-absent refusal: %d assertions PASS; private ROM not distributed\n",checks);
  return 0;
 }
 CK(fixture(rom,trace,0));
 CK(gk282_check_c289_trace(rom,trace,&out)==GK282_OK);
 CK(out.calls==3u&&out.rate_writes==1u&&out.matching_rate_candidates>=1u);
 CK(out.first_matching_call==3u&&out.generated_first_spring==0u);
 CK(out.seed_discontinuities==0u&&out.observed_trace_claimed==1u);
 r=out;
 CK(fixture(rom,trace,1));
 CK(gk282_check_c289_trace(rom,trace,&out)==GK282_SOURCE_PARITY);
 CK(memcmp(&r,&out,sizeof r)==0);
 CK(fixture(rom,trace,2));
 CK(gk282_check_c289_trace(rom,trace,&out)==GK282_TRACE);
 CK(memcmp(&r,&out,sizeof r)==0);
 CK(fixture(rom,trace,3));
 CK(gk282_check_c289_trace(rom,trace,&out)==GK282_SOURCE_PARITY);
 CK(memcmp(&r,&out,sizeof r)==0);
 CK(gk282_check_c289_trace("/nonexistent/private.nes",trace,&out)==GK282_ROM);
 CK(memcmp(&r,&out,sizeof r)==0);
 CK(gk282_check_c289_trace(NULL,trace,&out)==GK282_ARGUMENT);
 CK(memcmp(&r,&out,sizeof r)==0);
 /* Game state wrapper must refuse PRE_SETUP without the user's actual selection. */
 Gk1App a;
 gk1_app_init(&a);
 CK(gk1_app_original_c289_trace(&a,rom,trace,&out)==GK282_ARGUMENT);
 CK(memcmp(&r,&out,sizeof r)==0);
 remove(trace);
 printf("EXEC0282 stock C289 trace/source arithmetic: %d assertions PASS; no first Spring claimed\n",checks);
 return 0;
}
