#include "gk187_classic_assign.h"
#include <string.h>
/* Manual p.14 ASSIGN reallocates five drafted occupations; verified original
 * 83-byte checkpoint is a starting snapshot, NOT an editable ROM state.
 * Worker transfer arithmetic is inherited from the independently tested
 * native 3-order planner. Exact NES behavior still needs runtime comparison.
 */
static int source_initial(const Gk79Checkpoint *cp){
 return cp&&cp->valid==1u&&cp->source_verified==1u&&
  cp->orders.playable==1u&&cp->orders.governor==GK71_BASE&&
  cp->orders.season==GK71_SPRING&&cp->orders.allowance==3u&&
  cp->orders.remaining==3u&&cp->orders.completed==0u&&
  cp->orders.ended==0u&&cp->player_index<cp->world.country_count&&
  cp->orders.country_id==cp->player_index+1u&&
  gk85_raw_source_intact(cp)&&gk88_stock_projection_intact(cp);
}
Gk187Result gk187_open(const Gk79Checkpoint *cp,Gk187ClassicAssign *out){
 if(!cp||!out)return GK187_INVALID;
 if(!source_initial(cp))return GK187_SOURCE;
 Gk187ClassicAssign next={0};
 next.mutable_world=cp->world;next.budget=cp->orders;
 next.source_profile=cp->profile;next.player_index=cp->player_index;
 memcpy(next.source_sha,cp->raw_sha256,32u);
 next.active=1u;
 *out=next;return GK187_OK;
}
int gk187_intact(const Gk187ClassicAssign *s,const Gk79Checkpoint *cp){
 if(!s||!source_initial(cp)||s->active!=1u||s->passed>1u||
    s->action_count>3u||s->player_index!=cp->player_index||
    s->source_profile!=cp->profile||
    memcmp(s->source_sha,cp->raw_sha256,32u)||
    s->mutable_world.scenario_id!=cp->world.scenario_id||
    s->mutable_world.country_count!=cp->world.country_count||
    s->budget.allowance!=3u||s->budget.governor!=GK71_BASE||
    s->budget.season!=GK71_SPRING||s->budget.playable!=1u||
    s->budget.scenario!=cp->orders.scenario||
    s->budget.country_id!=cp->orders.country_id||
    s->budget.completed!=s->action_count||
    (s->passed?(s->budget.remaining!=0u||s->action_count>=3u):
               (s->budget.remaining+s->action_count!=3u)))return 0;
 Gk1LoadedScenario replay=cp->world;
 Gk71Budget budget=cp->orders;
 for(unsigned i=0;i<s->action_count;i++){
  Gk187Action a=s->actions[i];
  if(gk116_worker_math(&replay,&budget,cp->player_index,
    a.from,a.to,a.quantity)!=GK116_OK)return 0;
 }
 if(s->passed&&gk71_pass(&budget)!=GK71_OK)return 0;
 return memcmp(&replay,&s->mutable_world,sizeof replay)==0&&
        memcmp(&budget,&s->budget,sizeof budget)==0;
}
Gk187Result gk187_transfer(Gk187ClassicAssign *s,const Gk79Checkpoint *cp,
 uint8_t from,uint8_t to,uint16_t quantity){
 if(!gk187_intact(s,cp))return GK187_SOURCE;
 if(s->passed||s->budget.ended||s->action_count>=3u)return GK187_EXHAUSTED;
 Gk187ClassicAssign next=*s;
 Gk116Result r=gk116_worker_math(&next.mutable_world,&next.budget,
   next.player_index,from,to,quantity);
 if(r==GK116_NO_WORKER)return GK187_NO_WORKERS;
 if(r==GK116_NO_ORDERS)return GK187_EXHAUSTED;
 if(r!=GK116_OK)return GK187_INVALID;
 next.actions[next.action_count++]=(Gk187Action){from,to,quantity};
 if(!gk187_intact(&next,cp))return GK187_SOURCE;
 *s=next;return GK187_OK;
}
Gk187Result gk187_pass(Gk187ClassicAssign *s,const Gk79Checkpoint *cp){
 if(!gk187_intact(s,cp))return GK187_SOURCE;
 if(s->passed||s->budget.ended||s->budget.remaining==0u)return GK187_EXHAUSTED;
 Gk187ClassicAssign next=*s;
 if(gk71_pass(&next.budget)!=GK71_OK)return GK187_INVALID;
 next.passed=1u;
 if(!gk187_intact(&next,cp))return GK187_SOURCE;
 *s=next;return GK187_OK;
}

/* EXEC0190 — controller correction only. Never mutate authentic original
 * source, never construct a new original first Spring, and never refund a
 * completed/pass-ended season. Replay from the exact source checkpoint,
 * atomically publishing the reconstructed previous state only on success. */
Gk187Result gk187_undo_last(Gk187ClassicAssign *s,const Gk79Checkpoint *cp){
 if(!gk187_intact(s,cp))return GK187_SOURCE;
 if(s->passed||s->budget.ended)return GK187_EXHAUSTED;
 if(s->action_count==0u)return GK187_NO_ACTION;
 Gk187ClassicAssign prior;
 if(gk187_open(cp,&prior)!=GK187_OK)return GK187_SOURCE;
 for(unsigned i=0;i+1u<s->action_count;i++){
  const Gk187Action a=s->actions[i];
  if(gk187_transfer(&prior,cp,a.from,a.to,a.quantity)!=GK187_OK)
   return GK187_SOURCE;
 }
 if(!gk187_intact(&prior,cp))return GK187_SOURCE;
 *s=prior;
 return GK187_OK;
}
