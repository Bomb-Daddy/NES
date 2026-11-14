#include "gk282_stock_rng_trace.h"
#include "gk178_source_rng.h"
#include "gk1_app_state.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do { if(!(e)){fprintf(stderr,"GK284 FAIL line %d: %s\n", __LINE__, #e);return 1;} checks++; } while(0)
static void hex6(const uint8_t b[6],char s[13]){
 const char *hex="0123456789abcdef";
 for(unsigned i=0;i<6;i++){s[2*i]=hex[b[i]>>4u];s[2*i+1]=hex[b[i]&15u];}
 s[12]=0;
}
/* Construct a real fixed-bank source-authenticated arithmetic trace, not
 * invented PRG bytes. It is a SYNTHETIC trace, never observed Mesen runtime. */
static int generate(const char *rom,const char *path,int scenario,int fault,
                    uint16_t *target){
 FILE *f=fopen(path,"wb"); if(!f)return 0;
 fprintf(f,"GK1_C289_TRACE_V1,%s\n",scenario?"archived-saveram":"fresh-workram");
 uint8_t seed[6]={3,7,11,19,23,29};uint16_t draws[4]={0};
 for(unsigned i=0;i<4;i++){
  Gk178RngResult r={0};
  if(gk178_step_from_authenticated_stock(rom,seed,&r)!=GK178_OK){fclose(f);return 0;}
  char pre[13],post[13];hex6(seed,pre);hex6(r.after,post);
  draws[i]=(uint16_t)(((unsigned)r.result_67<<8)|r.result_66);
  unsigned frame=10u+i;
  if(fault==1 && i==3)frame=1u; /* relative-frame rewind */
  fprintf(f,"C,%u,%u,%s,%s,%u\n",frame,100u+30u*i,pre,post,(unsigned)draws[i]);
  memcpy(seed,r.after,6);
 }
 *target=(uint16_t)(50u+draws[0]%(94u+7u));
 /* Early draw is still a POSSIBLE rate candidate even after three more draws.
  * Search earlier call unconditionally; don't assert a unique causal origin. */
 if(fault==2)fprintf(f,"V,5,250,%u\n",(unsigned)*target); /* frame rewind */
 else if(fault==3)fprintf(f,"V,20,250,999\n"); /* valid but unassociated */
 else fprintf(f,"V,20,250,%u\n",(unsigned)*target);
 return fclose(f)==0;
}
int main(int argc,char **argv){
 int checks=0;CHECK(argc==3);
 const char *rom=argv[1],*trace=argv[2];
 Gk282Report out={0};
 if(!*rom){
  CHECK(gk282_check_c289_trace(rom,trace,&out)==GK282_ARGUMENT);
  CHECK(!out.generated_first_spring&&!out.possible_history_pairs);
  puts("GK284 public CI private-ROM absent refusal PASS");return 0;
 }
 uint16_t early=0;
 CHECK(generate(rom,trace,0,0,&early));
 CHECK(gk282_check_c289_trace(rom,trace,&out)==GK282_OK);
 CHECK(out.calls==4u&&out.rate_writes==1u);
 CHECK(out.possible_history_pairs>=1u&&out.first_possible_call==1u);
 CHECK(out.last_written_rate==early);
 CHECK(out.uniquely_matched_writes+out.ambiguously_matched_writes==1u);
 CHECK(out.generated_first_spring==0u&&!out.seed_discontinuities);
 Gk282Report prior=out;
 CHECK(generate(rom,trace,1,0,&early));
 CHECK(gk282_check_c289_trace(rom,trace,&out)==GK282_OK);
 CHECK(out.scenario==2u&&out.first_possible_call==1u);
 CHECK(generate(rom,trace,0,1,&early));
 CHECK(gk282_check_c289_trace(rom,trace,&out)==GK282_TRACE);
 CHECK(out.scenario==2u&&out.first_possible_call==1u); /* atomic refusal */
 CHECK(generate(rom,trace,0,2,&early));
 CHECK(gk282_check_c289_trace(rom,trace,&out)==GK282_TRACE);
 CHECK(out.scenario==2u);
 CHECK(generate(rom,trace,0,3,&early));
 CHECK(gk282_check_c289_trace(rom,trace,&out)==GK282_OK);
 CHECK(out.unmatched_writes==1u&&out.possible_history_pairs==0u);
 CHECK(out.generated_first_spring==0u);
 CHECK(gk282_check_c289_trace("/no/private/original.nes",trace,&out)==GK282_ROM);
 CHECK(out.unmatched_writes==1u); /* atomic refusal */
 CHECK(prior.generated_first_spring==0u);
 remove(trace);
 printf("GK284 source-authenticated POSSIBLE Rate chronology %d assertions PASS; Spring NOT implemented\n",checks);
 return 0;
}
