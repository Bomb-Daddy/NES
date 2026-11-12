#include "gk220_labor_history.h"
#include <string.h>
#include <limits.h>
static uint16_t *pool(Gk1CountryTemplate *c,unsigned k){
 switch(k){
 case 0:return &c->troops;
 case 1:return &c->town;
 case 2:return &c->masons;
 case 3:return &c->artisans;
 case 4:return &c->food_workers;
 default:return NULL;
 }
}
static int valid_world(const Gk1LoadedScenario *src,
 const Gk1LoadedScenario *draft,unsigned w,unsigned idx){
 Gk219Comparison audit={0};
 return src&&draft&&w<2u&&idx<draft->country_count&&
  src->country_count==draft->country_count&&
  gk219_compare_finance(src,draft,idx,&audit)==GK219_OK;
}
Gk220Result gk220_labor_transfer(const Gk1LoadedScenario *source,
 Gk1LoadedScenario *draft,Gk220History *h,
 uint8_t world,uint8_t country,uint8_t from,uint8_t to){
 if(!source||!draft||!h||world>1u||from>=5u||to>=5u||from==to||
    h->count>GK220_HISTORY_CAP||country>=draft->country_count)return GK220_INVALID;
 if(!valid_world(source,draft,world,country))return GK220_SOURCE_DRIFT;
 if(h->count==GK220_HISTORY_CAP)return GK220_FULL;
 Gk1LoadedScenario next=*draft;
 uint16_t *p=pool(&next.countries[country],from);
 uint16_t *q=pool(&next.countries[country],to);
 if(!p||!q||!*p||*q>=999u)return GK220_INVALID;
 const Gk220Move move={world,country,from,to,*p,*q};
 --(*p);++(*q);
 if(!valid_world(source,&next,world,country))return GK220_SOURCE_DRIFT;
 *draft=next;
 h->moves[h->count++]=move;
 return GK220_OK;
}
Gk220Result gk220_labor_undo(const Gk1LoadedScenario *source,
 Gk1LoadedScenario *draft,Gk220History *h,
 uint8_t world,uint8_t country){
 if(!source||!draft||!h||world>1u||h->count>GK220_HISTORY_CAP||
    country>=draft->country_count)return GK220_INVALID;
 if(!valid_world(source,draft,world,country))return GK220_SOURCE_DRIFT;
 if(!h->count)return GK220_NO_HISTORY;
 Gk220Move move=h->moves[h->count-1u];
 if(move.world!=world||move.country!=country)return GK220_WRONG_COUNTRY;
 if(move.from>=5u||move.to>=5u||move.from==move.to||
    !move.from_before||move.to_before>=999u)return GK220_INVALID;
 Gk1LoadedScenario next=*draft;
 uint16_t *p=pool(&next.countries[country],move.from);
 uint16_t *q=pool(&next.countries[country],move.to);
 if(!p||!q||*p!=(uint16_t)(move.from_before-1u)||
    *q!=(uint16_t)(move.to_before+1u))return GK220_SOURCE_DRIFT;
 ++(*p);--(*q);
 if(!valid_world(source,&next,world,country))return GK220_SOURCE_DRIFT;
 *draft=next;
 h->count--;
 memset(&h->moves[h->count],0,sizeof h->moves[h->count]);
 return GK220_OK;
}
