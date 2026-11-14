#include "gk282_stock_rng_trace.h"
#include "gk178_source_rng.h"
#include "gk82_stock_cartridge.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdlib.h>
/* Authenticated stock C289 fixed bank table starts at C2EC: not distributed. */
#define GK282_COEFF_OFFSET (16L+15L*16384L+0x02ecL)
#define GK282_HEADER "GK1_C289_TRACE_V1"
static int hex6(const char *s,uint8_t out[6]){
 if(strlen(s)!=12u)return 0;
 for(unsigned i=0;i<6u;i++){
  unsigned hi=(unsigned char)s[i*2],lo=(unsigned char)s[i*2+1];
  if(!isxdigit((int)hi)||!isxdigit((int)lo))return 0;
  hi=hi>='0'&&hi<='9'?hi-'0':(hi|32u)-'a'+10u;
  lo=lo>='0'&&lo<='9'?lo-'0':(lo|32u)-'a'+10u;
  out[i]=(uint8_t)(hi*16u+lo);
 }
 return 1;
}
Gk282Status gk282_check_c289_trace(const char *rom,
 const char *trace,Gk282Report *out){
 if(!rom||!*rom||!trace||!*trace||!out)return GK282_ARGUMENT;
 Gk82RomBootstrap rb={0};
 if(gk82_probe_stock_rom(rom,&rb)!=GK82_ROM_OK)return GK282_ROM;
 FILE *rp=fopen(rom,"rb");if(!rp)return GK282_ROM;
 uint8_t coeff[6];
 int loaded=fseek(rp,GK282_COEFF_OFFSET,SEEK_SET)==0&&fread(coeff,1,6,rp)==6;
 int read_err=ferror(rp),close_err=fclose(rp);
 if(!loaded||read_err||close_err){memset(coeff,0,sizeof coeff);return GK282_ROM;}
 FILE *fp=fopen(trace,"rb");if(!fp){memset(coeff,0,sizeof coeff);return GK282_TRACE;}
 /* Keep every earlier observed C289 candidate. The old latest-only logic
  * could miss a source draw separated from a Rate write by intervening draws.
  * A match is only a possible association, not evidence of which CPU write
  * consumed it. Bound storage to the existing 8192-call limit. */
 uint16_t (*history)[5]=calloc(GK282_MAX_CALLS,sizeof *history);
 if(!history){fclose(fp);memset(coeff,0,sizeof coeff);return GK282_LIMIT;}
 Gk282Report report={0};
 uint8_t prior_after[6]={0};int have_prev=0;
 uint16_t rate_candidates[5]={0};uint32_t candidate_call[5]={0};
 uint64_t last_event_cycle=0;unsigned last_frame=0;int have_frame=0;
 char line[512];unsigned line_no=0;int header=0;
 Gk282Status err=GK282_OK;
 while(fgets(line,sizeof line,fp)){
  line_no++;
  size_t ln=strlen(line);
  if(!strchr(line,'\n')&&!feof(fp)){err=GK282_TRACE;break;}
  while(ln&&(line[ln-1]=='\n'||line[ln-1]=='\r'))line[--ln]=0;
  if(line_no==1){
   char typ[32],scenario[32],extra;
   if(sscanf(line,"%31[^,],%31[^,],%c",typ,scenario,&extra)!=2||
      strcmp(typ,GK282_HEADER)!=0||
      (strcmp(scenario,"fresh-workram")!=0&&
       strcmp(scenario,"archived-saveram")!=0)) {err=GK282_TRACE;break;}
   report.scenario=(uint8_t)(strcmp(scenario,"fresh-workram")==0?1:2);
   header=1;continue;
  }
  if(line[0]=='C'){
   uint64_t cycle=0;unsigned frame=0,draw=0;char before[32],after[32],tail;
   if(sscanf(line,"C,%u,%" SCNu64 ",%31[^,],%31[^,],%u%c",
             &frame,&cycle,before,after,&draw,&tail)!=5){err=GK282_TRACE;break;}
   uint8_t pre[6],post[6];
   if(!hex6(before,pre)||!hex6(after,post)||draw>32767u||
      !cycle||cycle<=last_event_cycle||(have_frame&&frame<last_frame)){
    err=GK282_TRACE;break;
   }
   if(report.calls>=GK282_MAX_CALLS){err=GK282_LIMIT;break;}
   Gk178RngResult next={0};
   if(gk178_step(pre,coeff,&next)!=GK178_OK||
      memcmp(next.after,post,6u)!=0||
      (((unsigned)next.result_67<<8)|next.result_66)!=draw){
    err=GK282_SOURCE_PARITY;break;
   }
   if(have_prev&&memcmp(prior_after,pre,6u)!=0)report.seed_discontinuities++;
   memcpy(prior_after,post,6u);have_prev=1;
   report.calls++;
   last_frame=frame;have_frame=1;
   if(report.calls==1)report.first_cycle=cycle;
   report.last_cycle=cycle;last_event_cycle=cycle;
   for(unsigned d=1;d<=5;d++){
    rate_candidates[d-1]=(uint16_t)(50u+draw%(94u+7u*d));
    candidate_call[d-1]=report.calls;
    history[report.calls-1u][d-1u]=rate_candidates[d-1u];
   }
  }else if(line[0]=='V'){
   uint64_t cycle=0;unsigned frame=0,rate=0;char tail;
   if(sscanf(line,"V,%u,%" SCNu64 ",%u%c",&frame,&cycle,&rate,&tail)!=3||
      !cycle||rate>999u||!report.calls||cycle<=last_event_cycle||
      (have_frame&&frame<last_frame)){
    err=GK282_TRACE;break;
   }
   if(report.rate_writes>=GK282_MAX_CALLS){err=GK282_LIMIT;break;}
   report.rate_writes++;report.last_written_rate=(uint16_t)rate;
   last_event_cycle=cycle;last_frame=frame;have_frame=1;
   uint32_t pair_count=0,one_call=0;uint8_t one_diff=0;
   for(uint32_t call=0;call<report.calls;call++){
    for(unsigned d=0;d<5;d++)if(history[call][d]==(uint16_t)rate){
      pair_count++;
      if(!report.first_possible_call)report.first_possible_call=call+1u;
      report.last_possible_call=call+1u;
      one_call=call+1u;one_diff=(uint8_t)(d+1u);
    }
   }
   report.possible_history_pairs+=pair_count;
   if(pair_count==0u)report.unmatched_writes++;
   else if(pair_count==1u){report.uniquely_matched_writes++;
    report.last_unique_call=one_call;report.last_unique_difficulty=one_diff;}
   else report.ambiguously_matched_writes++;
   for(unsigned d=0;d<5;d++)if(rate_candidates[d]==rate){
    report.matching_rate_candidates++;
    if(!report.first_matching_call)report.first_matching_call=candidate_call[d];
    report.last_matching_call=candidate_call[d];
   }
  }else {err=GK282_TRACE;break;}
 }
 if(ferror(fp))err=GK282_TRACE;
 if(fclose(fp)!=0)err=GK282_TRACE;
 free(history);
 memset(coeff,0,sizeof coeff);
 if(err!=GK282_OK)return err;
 if(!header||!report.calls||!report.rate_writes)return GK282_TRACE;
 report.observed_trace_claimed=1u;
 report.validated_source_steps=1u;
 /* This is arithmetic validation of a file CLAIMING Mesen provenance,
  * not independently witnessed emulator execution or a Spring generator. */
 report.generated_first_spring=0u;
 *out=report;return GK282_OK;
}
