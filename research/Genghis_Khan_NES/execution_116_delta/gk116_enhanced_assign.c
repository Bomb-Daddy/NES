#include "gk116_enhanced_assign.h"
#include "gk1_source_identity.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

static uint16_t *pool(Gk1CountryTemplate *c,uint8_t i){
 switch(i){
 case 0:return &c->troops;
 case 1:return &c->town;
 case 2:return &c->masons;
 case 3:return &c->artisans;
 case 4:return &c->food_workers;
 default:return NULL;
 }
}
Gk116Result gk116_worker_math(Gk1LoadedScenario *world,Gk71Budget *budget,
 uint8_t country_index,uint8_t from,uint8_t to,uint16_t quantity){
 if(!world||!budget||from>=5u||to>=5u||from==to||quantity==0u||
    world->phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE||
    world->country_count!=(world->scenario_id==GK1_MONGOL_CONQUEST?14u:
                          world->scenario_id==GK1_WORLD_CONQUEST?27u:0u)||
    country_index>=world->country_count||
    budget->scenario!=world->scenario_id||
    budget->country_id!=country_index+1u||
    budget->governor!=GK71_BASE||budget->season!=GK71_SPRING||
    budget->allowance!=3u||budget->playable!=1u)return GK116_INPUT;
 if(budget->ended||budget->remaining==0u)return GK116_NO_ORDERS;
 Gk1CountryTemplate *c=&world->countries[country_index];
 if(c->country_id!=country_index+1u||c->index!=country_index||
    c->scenario_id!=world->scenario_id)return GK116_INPUT;
 uint16_t *a=pool(c,from),*b=pool(c,to);
 if(!a||!b||*a<quantity)return GK116_NO_WORKER;
 if((unsigned)*b+quantity>999u)return GK116_INPUT;
 unsigned total=(unsigned)c->troops+c->town+c->masons+
                c->artisans+c->food_workers;
 if(total!=c->drafted_population||
    total+c->undrafted_reserve!=c->combined_population)return GK116_INPUT;
 Gk71Budget next=*budget;
 if(gk71_commit_completed(&next,1u)!=GK71_OK)return GK116_INPUT;
 *a=(uint16_t)(*a-quantity);*b=(uint16_t)(*b+quantity);
 *budget=next;return GK116_OK;
}
static int original_valid(const Gk79Checkpoint *stock){
 return stock&&stock->valid==1u&&stock->source_verified==1u&&
  stock->orders.season==GK71_SPRING&&stock->orders.governor==GK71_BASE&&
  stock->orders.allowance==3u&&stock->orders.completed==0u&&
  stock->orders.remaining==3u&&stock->orders.ended==0u&&
  stock->orders.country_id==stock->player_index+1u&&
  stock->year==(stock->world.scenario_id==GK1_MONGOL_CONQUEST?1175u:1206u)&&
  stock->rate>=1u&&stock->rate<=999u&&
  gk85_raw_source_intact(stock)&&gk88_stock_projection_intact(stock);
}
Gk116Result gk116_open(const Gk79Checkpoint *stock,uint8_t enhanced,
 Gk116EnhancedAssign *out){
 if(!out||enhanced>1u)return GK116_INPUT;
 if(!enhanced)return GK116_CLASSIC_LOCKED;
 if(!original_valid(stock))return GK116_NEEDS_ORIGINAL;
 Gk116EnhancedAssign next={0};
 next.mutable_world=stock->world;next.budget=stock->orders;
 next.profile=stock->profile;next.player_index=stock->player_index;
 next.year=stock->year;next.rate=stock->rate;
 memcpy(next.source_sha,stock->raw_sha256,sizeof next.source_sha);
 next.active=1u;
 *out=next;return GK116_OK;
}
int gk116_attestation_intact(const Gk116EnhancedAssign *s,
 const Gk79Checkpoint *stock){
 if(!s||s->active!=1u||!original_valid(stock)||
    s->profile!=stock->profile||s->player_index!=stock->player_index||
    s->year!=stock->year||s->rate!=stock->rate||
    memcmp(s->source_sha,stock->raw_sha256,32u)||
    s->mutable_world.scenario_id!=stock->world.scenario_id||
    s->mutable_world.country_count!=stock->world.country_count||
    s->mutable_world.phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE||
    s->budget.scenario!=stock->orders.scenario||
    s->budget.country_id!=stock->orders.country_id||
    s->budget.governor!=GK71_BASE||s->budget.season!=GK71_SPRING||
    s->budget.playable!=1u||s->budget.allowance!=3u||
    s->action_count>GK116_MAX_ACTIONS||
    s->budget.completed!=s->action_count||
    s->budget.remaining+s->action_count!=3u||
    s->budget.ended!=(s->budget.remaining==0u))return 0;
 /* Source remains unchanged in every nonplayer country. The only permitted
  * difference is five drafted labor pools of the player's own country. */
 for(unsigned i=0;i<stock->world.country_count;i++){
  const Gk1CountryTemplate *a=&s->mutable_world.countries[i];
  const Gk1CountryTemplate *b=&stock->world.countries[i];
  if(i==s->player_index){
   Gk1CountryTemplate reconstructed=*a;
   reconstructed.troops=b->troops;reconstructed.town=b->town;
   reconstructed.masons=b->masons;reconstructed.artisans=b->artisans;
   reconstructed.food_workers=b->food_workers;
   if(memcmp(&reconstructed,b,sizeof reconstructed))return 0;
  }else if(memcmp(a,b,sizeof *a))return 0;
 }
 /* Replay exact recorded actions from immutable original into a fresh copy.
  * This prevents callers altering mutable workers or budgets behind the UI. */
 Gk1LoadedScenario replay=stock->world;
 Gk71Budget replay_budget=stock->orders;
 for(unsigned i=0;i<s->action_count;i++){
  const Gk116LaborAction *action=&s->actions[i];
  if(gk116_worker_math(&replay,&replay_budget,s->player_index,
      action->from,action->to,action->quantity)!=GK116_OK)return 0;
 }
 return memcmp(&replay,&s->mutable_world,sizeof replay)==0&&
        memcmp(&replay_budget,&s->budget,sizeof replay_budget)==0;
}
Gk116Result gk116_transfer(Gk116EnhancedAssign *s,
 const Gk79Checkpoint *original,uint8_t from,uint8_t to,uint16_t quantity){
 if(!s||!original)return GK116_INPUT;
 if(!gk116_attestation_intact(s,original))return GK116_SOURCE_CHANGED;
 if(s->action_count>=GK116_MAX_ACTIONS)return GK116_NO_ORDERS;
 /* Bounded atomic candidate; even the failed worker/turn transaction leaves
  * country fields, source record, action history and budget untouched. */
 Gk116EnhancedAssign candidate=*s;
 Gk116Result r=gk116_worker_math(&candidate.mutable_world,&candidate.budget,
    candidate.player_index,from,to,quantity);
 if(r!=GK116_OK)return r;
 candidate.actions[candidate.action_count++]=(Gk116LaborAction){from,to,quantity};
 *s=candidate;return GK116_OK;
}
#define RECEIPT_LEN 96u
static void encode(const Gk116EnhancedAssign *s,uint8_t b[RECEIPT_LEN]){
 memset(b,0,RECEIPT_LEN);
 memcpy(b,"GA16",4u);b[4]=1u;b[5]=s->profile;
 b[6]=(uint8_t)s->mutable_world.scenario_id;b[7]=s->player_index;
 b[8]=s->mutable_world.country_count;b[9]=(uint8_t)s->year;
 b[10]=(uint8_t)(s->year>>8);b[11]=(uint8_t)s->rate;
 b[12]=(uint8_t)(s->rate>>8);b[13]=s->action_count;
 memcpy(b+16,s->source_sha,32u);
 for(unsigned i=0;i<s->action_count;i++){
  b[48+4*i]=s->actions[i].from;b[49+4*i]=s->actions[i].to;
  b[50+4*i]=(uint8_t)s->actions[i].quantity;
  b[51+4*i]=(uint8_t)(s->actions[i].quantity>>8);
 }
 uint8_t sha[32];gk1_sha256_bytes(b,80u,sha);memcpy(b+80,sha,16u);
}
Gk116Result gk116_save_new(const char *path,const Gk79Checkpoint *stock,
 const Gk116EnhancedAssign *s){
 if(!path||!*path||!gk116_attestation_intact(s,stock))return GK116_SOURCE_CHANGED;
 uint8_t raw[RECEIPT_LEN];encode(s,raw);
 errno=0;FILE *f=fopen(path,"wbx");
 if(!f)return errno==EEXIST?GK116_EXISTS:GK116_IO;
 size_t n=fwrite(raw,1,RECEIPT_LEN,f);
 int failed=ferror(f),closed=fclose(f);
 if(n!=RECEIPT_LEN||failed||closed){(void)remove(path);return GK116_IO;}
 return GK116_OK;
}
Gk116Result gk116_cold_load(const char *path,const Gk79Checkpoint *stock,
 uint8_t enhanced,Gk116EnhancedAssign *out){
 if(!path||!*path||!out)return GK116_INPUT;
 Gk116EnhancedAssign fresh;
 Gk116Result r=gk116_open(stock,enhanced,&fresh);
 if(r!=GK116_OK)return r;
 FILE *f=fopen(path,"rb");if(!f)return GK116_IO;
 uint8_t raw[RECEIPT_LEN+1u];size_t n=fread(raw,1,sizeof raw,f);
 int failed=ferror(f),closed=fclose(f);
 if(failed||closed)return GK116_IO;
 if(n!=RECEIPT_LEN)return GK116_CORRUPT;
 unsigned count=raw[13];if(count>GK116_MAX_ACTIONS)return GK116_CORRUPT;
 for(unsigned i=0;i<count;i++){
  uint16_t qty=(uint16_t)((unsigned)raw[50+4*i]|((unsigned)raw[51+4*i]<<8));
  r=gk116_transfer(&fresh,stock,raw[48+4*i],raw[49+4*i],qty);
  if(r!=GK116_OK)return GK116_CORRUPT;
 }
 uint8_t expected[RECEIPT_LEN];encode(&fresh,expected);
 if(memcmp(raw,expected,RECEIPT_LEN)!=0)return GK116_CORRUPT;
 *out=fresh;return GK116_OK;
}
