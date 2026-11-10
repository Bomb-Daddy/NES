#include "gk145_fresh_progress.h"
#include "gk1_app_state.h"
#include <stdlib.h>
#define assert(x) do { if(!(x)) {fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);} }while(0)
#include <stdio.h>
#include <string.h>
int main(void){
 /* Synthetic values test logic. This does NOT reproduce original ROM RNG. */
 Gk145FreshObservation stage[]={
  {10u,1000u,0,0,0,0,0},
  {20u,2000u,1174,14,300,0,0},
  {30u,3000u,1174,14,300,0,0},
  {40u,4000u,1175,14,300,137,0},
  {50u,5000u,1175,14,325,137,0},
  {60u,6000u,1175,14,325,137,3}
 };
 Gk145FreshProgress r={0};assert(gk145_trace_fresh_progress(stage,6,1175,14,&r)==GK145_OK);
 assert(r.samples==6&&r.first_world_setup==2&&r.first_spring_year==4&&
        r.first_nonzero_rate==4&&r.first_gold_change==5&&
        r.first_three_orders==6&&r.first_full_spring==6&&r.temporal_separation_observed==1);
 Gk145FreshProgress guard=r;
 stage[3].cpu_cycles=stage[2].cpu_cycles;
 assert(gk145_trace_fresh_progress(stage,6,1175,14,&r)==GK145_INVALID_CHRONOLOGY);
 assert(memcmp(&r,&guard,sizeof r)==0);stage[3].cpu_cycles=4000;
 stage[5].orders=2;
 assert(gk145_trace_fresh_progress(stage,6,1175,14,&r)==GK145_AMBIGUOUS_PROGRESS);
 assert(memcmp(&r,&guard,sizeof r)==0);stage[5].orders=3;
 stage[4].country_count=27;
 assert(gk145_trace_fresh_progress(stage,6,1175,14,&r)==GK145_INVALID_CHRONOLOGY);
 assert(memcmp(&r,&guard,sizeof r)==0);stage[4].country_count=14;
 stage[3].rate=0; stage[4].rate=0;
 assert(gk145_trace_fresh_progress(stage,6,1175,14,&r)==GK145_OK);
 assert(r.first_nonzero_rate==6&&r.first_full_spring==6);
 assert(gk145_trace_fresh_progress(NULL,6,1175,14,&r)==GK145_ARGUMENT);
 assert(gk145_trace_fresh_progress(stage,6,1175,27,&r)==GK145_ARGUMENT);
 Gk1App *app=(Gk1App*)calloc(1,sizeof *app);assert(app!=NULL);
 gk1_app_init(app);
 Gk145FreshProgress locked={0};
 assert(gk1_app_source_fresh_progress(app,stage,6,1175,14,&locked)==GK145_ARGUMENT);
 assert(memcmp(&locked,&(Gk145FreshProgress){0},sizeof locked)==0);
 free(app);
 puts("GK145 source-progression + app PRE_SETUP negative controls PASS; OBSERVATION_ONLY");
 return 0;
}
