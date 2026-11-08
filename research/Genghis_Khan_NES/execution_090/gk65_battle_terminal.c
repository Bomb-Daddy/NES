#include "gk65_battle_terminal.h"
Gk65Status gk65_begin(uint8_t orders,Gk65Terminal *out){
 if(!out||orders<1||orders>3)return GK65_REJECTED;
 const Gk65Terminal fresh={(uint8_t)(orders*30u),0,GK65_ATTACKER_PHASE,GK65_UNDECIDED,0,0};
 *out=fresh;return GK65_OK;
}
Gk65Status gk65_apply(Gk65Terminal *state,Gk65Event event,Gk65Victim victim){
 if(!state||state->max_days<30||state->max_days>90||state->max_days%30||
    state->days_complete>=state->max_days||state->phase>GK65_DEFENDER_PHASE||
    state->outcome!=GK65_UNDECIDED||event>GK65_BOTH_COMMANDERS_DESTROYED||
    (event==GK65_ACT_COMPLETE && victim!=GK65_NO_VICTIM)||
    (event==GK65_BOTH_COMMANDERS_DESTROYED && victim!=GK65_NO_VICTIM)||
    (event!=GK65_ACT_COMPLETE && event!=GK65_BOTH_COMMANDERS_DESTROYED &&
     victim!=GK65_ATTACKER_VICTIM && victim!=GK65_DEFENDER_VICTIM))
  return GK65_REJECTED;
 Gk65Terminal next=*state;
 if(event==GK65_BOTH_COMMANDERS_DESTROYED){
  next.outcome=GK65_DEFENDER_RETAINS_DOUBLE_KO;next.attacker_captured=1;
 }else if(event!=GK65_ACT_COMPLETE){
  /* Only explicitly externally reported victim determines winner.  A commander
   * may starve or flee during either player's action.  Never infer it from whose
   * turn is active, nor manufacture a damage/capture/random event. */
  next.outcome=(victim==GK65_DEFENDER_VICTIM)?GK65_ATTACKER_WON:GK65_DEFENDER_WON;
 }else if(state->phase==GK65_ATTACKER_PHASE){next.phase=GK65_DEFENDER_PHASE;}
 else {
  next.days_complete++;
  if(next.days_complete==next.max_days)next.outcome=GK65_ATTACKER_RETREATS_TIME;
  else next.phase=GK65_ATTACKER_PHASE;
 }
 if(next.outcome!=GK65_UNDECIDED){
  next.phase=GK65_DONE;next.terminal_event=(uint8_t)event;
 }
 *state=next;return GK65_OK;
}
