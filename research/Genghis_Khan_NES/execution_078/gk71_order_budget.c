#include "gk71_order_budget.h"
#include <string.h>

static int valid_world(const Gk1LoadedScenario *w,uint8_t ix){
 if(!w)return 0;
 unsigned n=w->scenario_id==GK1_MONGOL_CONQUEST?14u:
            w->scenario_id==GK1_WORLD_CONQUEST?27u:0u;
 if(!n||w->country_count!=n||ix>=n)return 0;
 if((unsigned)w->phase!=GK1_PRE_SETUP_TEMPLATE &&
    (unsigned)w->phase!=GK71_RESERVED_VERIFIED_PLAYABLE_PHASE)return 0;
 const Gk1CountryTemplate *c=&w->countries[ix];
 return c->scenario_id==(uint8_t)w->scenario_id && c->index==ix &&
        c->country_id==ix+1u && c->combined_population>=c->troops;
}
static int valid_budget(const Gk71Budget *b){
 if(!b||b->scenario<1||b->scenario>2||b->country_id<1||
    b->country_id>(b->scenario==1?14u:27u)||b->season>3||
    b->governor>2||b->playable>1||b->ended>1)return 0;
 unsigned max=b->governor==GK71_BASE?3u:b->governor==GK71_DIRECT?1u:0u;
 return b->allowance==max && b->remaining<=max &&
        b->completed<=max && b->remaining+b->completed<=max &&
        (b->governor!=GK71_PRINCE || (b->remaining==0&&b->completed==0)) &&
        (!b->ended||b->remaining==0);
}
Gk71Result gk71_open(const Gk1LoadedScenario *world,uint8_t index,
                     Gk71Season season,Gk71Governor govt,Gk71Budget *out){
 if(!out||!valid_world(world,index)||(unsigned)season>3u||(unsigned)govt>2u)
  return GK71_INVALID;
 Gk71Budget next;memset(&next,0,sizeof next);
 next.scenario=(uint8_t)world->scenario_id;
 next.country_id=(uint8_t)(index+1u);
 next.season=(uint8_t)season;
 next.governor=(uint8_t)govt;
 next.allowance=next.remaining=(uint8_t)(govt==GK71_BASE?3u:
                                         govt==GK71_DIRECT?1u:0u);
 next.playable=(uint8_t)((unsigned)world->phase==GK71_RESERVED_VERIFIED_PLAYABLE_PHASE);
 *out=next;
 return next.playable?GK71_OK:GK71_LOCKED;
}
Gk71Result gk71_commit_completed(Gk71Budget *b,uint8_t completed_effect){
 if(!valid_budget(b)||completed_effect>1)return GK71_INVALID;
 if(!b->playable||!completed_effect)return GK71_LOCKED;
 if(b->governor==GK71_PRINCE)return GK71_PRINCE_AI;
 if(b->ended||!b->remaining)return GK71_EXHAUSTED;
 Gk71Budget next=*b;
 next.remaining--;next.completed++;
 if(!next.remaining)next.ended=1;
 *b=next;return GK71_OK;
}
Gk71Result gk71_pass(Gk71Budget *b){
 if(!valid_budget(b))return GK71_INVALID;
 if(!b->playable)return GK71_LOCKED;
 if(b->governor==GK71_PRINCE)return GK71_PRINCE_AI;
 if(b->ended||!b->remaining)return GK71_EXHAUSTED;
 Gk71Budget next=*b;next.remaining=0;next.ended=1;*b=next;
 return GK71_OK;
}
Gk71Result gk71_next_season(const Gk1LoadedScenario *world,
                           Gk71Budget *b,Gk71Season next_season){
 if(!valid_budget(b)||!valid_world(world,(uint8_t)(b->country_id-1u))||
    (unsigned)next_season>3u||world->scenario_id!=(Gk1ScenarioId)b->scenario)
  return GK71_INVALID;
 if(!b->playable || (unsigned)world->phase!=GK71_RESERVED_VERIFIED_PLAYABLE_PHASE)
  return GK71_LOCKED;
 if(!b->ended)return GK71_LOCKED; /* scheduler must explicitly end season */
 if(next_season!=(Gk71Season)((b->season+1u)%4u))return GK71_INVALID;
 Gk71Budget updated;
 Gk71Result r=gk71_open(world,(uint8_t)(b->country_id-1u),next_season,
                             (Gk71Governor)b->governor,&updated);
 if(r==GK71_OK)*b=updated;
 return r;
}
