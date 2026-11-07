#include "gk64_manual_rules.h"
#include <string.h>
uint16_t gk64_first_spring_year(Gk1ScenarioId id){
 return id==GK1_MONGOL_CONQUEST?1175u:id==GK1_WORLD_CONQUEST?1206u:0;
}
Gk64Status gk64_preview(const Gk1LoadedScenario *world,uint8_t index,
                        uint8_t remaining_orders,Gk64ManualPreview *out){
 if(!world||!out||index>=world->country_count||
    world->country_count>(world->scenario_id==GK1_MONGOL_CONQUEST?14u:27u)||
    world->phase!=GK1_PRE_SETUP_TEMPLATE||remaining_orders<1||remaining_orders>3||
    !gk64_first_spring_year(world->scenario_id))return GK64_INVALID;
 const Gk1CountryTemplate *c=&world->countries[index];
 Gk64ManualPreview next={0};
 next.first_order_year=gk64_first_spring_year(world->scenario_id);
 next.population=c->combined_population;
 next.feed_whole=(uint16_t)(c->combined_population/2u);
 next.feed_half=(uint8_t)(c->combined_population%2u);
 next.soldier_gold_wages=c->troops;
 next.troops=c->troops;next.current_gold=c->gold;next.current_food=c->goods[10];
 next.base_orders=3;next.direct_vassal_orders=1;
 next.days_for_remaining_orders=(uint8_t)(remaining_orders*30);
 *out=next;return GK64_OK;
}
Gk64Status gk64_battle_begin(uint8_t remaining_orders,Gk64BattleClock *out){
 if(!out||!remaining_orders||remaining_orders>3)return GK64_INVALID;
 Gk64BattleClock temp={0};temp.max_days=(uint8_t)(remaining_orders*30);
 *out=temp;return GK64_OK;
}
Gk64Status gk64_battle_day(Gk64BattleClock *clock,Gk64BattleResult resolution){
 if(!clock||clock->max_days<30||clock->max_days>90||clock->max_days%30||
    clock->used_days>=clock->max_days||clock->result!=GK64_NO_RESULT||
    resolution>GK64_DEFENDER_WINS_SURRENDER||resolution==GK64_ATTACKER_RETREAT_TIME)
  return GK64_INVALID;
 Gk64BattleClock next=*clock;
 next.used_days++;
 if(resolution!=GK64_NO_RESULT)next.result=resolution;
 else if(next.used_days==next.max_days)next.result=GK64_ATTACKER_RETREAT_TIME;
 *clock=next;
 return GK64_OK;
}
Gk64Status gk64_terrain_mobility(Gk64Terrain t,uint8_t *cost){
 static const uint8_t values[]={3,4,5,6,8,4,5};
 if(!cost||(unsigned)t>=sizeof values)return GK64_INVALID;
 *cost=values[t];return GK64_OK;
}
Gk64Status gk64_pass_mobility(uint8_t current,uint8_t cavalry,uint8_t *next){
 if(!next||cavalry>1||current>15)return GK64_INVALID;
 uint8_t cap=cavalry?15u:13u;
 if(current>cap)return GK64_INVALID;
 *next=(uint8_t)(current<cap?current+1:cap);return GK64_OK;
}
Gk64Status gk64_daughter_marriage(uint8_t age,uint8_t *eligible){
 /* Manual p7 >=6 vs p17 >=8: 6 and 7 MUST stay unresolved until NES test. */
 if(!eligible)return GK64_INVALID;
 if(age==6||age==7)return GK64_UNRESOLVED;
 *eligible=(uint8_t)(age>=8);return GK64_OK;
}
Gk64Status gk64_divide_cavalry(uint8_t enhanced,uint8_t mobility,uint8_t *eligible){
 if(!eligible||enhanced>1||mobility>15)return GK64_INVALID;
 /* Manual p29 Classic>=15; enhanced controlled Mesen 12 refused,13 accepted.
    Values 14/15 in Enhanced not separately re-tested; monotonic inference is
    NOT promoted to an original-Rom oracle. */
 if(!enhanced){*eligible=(uint8_t)(mobility>=15);return GK64_OK;}
 if(mobility==12||mobility==13){*eligible=(uint8_t)(mobility==13);return GK64_OK;}
 return GK64_UNRESOLVED;
}
