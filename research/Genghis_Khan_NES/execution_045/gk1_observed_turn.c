#include "gk1_observed_turn.h"
#include <string.h>
#define GK45_SOURCE_SEED UINT64_C(0xc7c3f28f2363)
#define GK45_SOURCE_AFTER UINT64_C(0xa31e4ecd8fd1)
Gk45Result gk45_open(Gk45Case scenario, Gk45Turn *out) {
  if (!out) return GK45_BAD;
  if (scenario!=GK45_RAW7 && scenario!=GK45_RAW2 && scenario!=GK45_FRIEND_40) return GK45_UNKNOWN;
  Gk45Turn t={0};t.fixture=scenario;t.phase=GK45_BEFORE;
  t.soldiers=40;t.mobility=10;t.x=7;t.y=7;
  t.rng48=GK45_SOURCE_SEED;
  if (scenario==GK45_FRIEND_40) {
    t.ally1_soldiers=40;t.ally1_x=8;t.ally1_y=7;
  } else {
    t.ally1_soldiers=40;t.ally1_x=6;t.ally1_y=9;
  }
  *out=t;return GK45_ACCEPTED;
}
Gk45Result gk45_step_first(Gk45Turn *t) {
  if (!t) return GK45_BAD;
  if(t->phase!=GK45_BEFORE || t->soldiers!=40 || t->mobility!=10 ||
    t->x!=7 || t->y!=7 || t->rng48!=GK45_SOURCE_SEED || t->steps!=0 || t->completion_word!=0)
    return GK45_UNKNOWN;
  /* Check the occupation fixture before accepting a transition. */
  if(t->fixture==GK45_FRIEND_40) {
    if(t->ally1_soldiers!=40 || t->ally1_x!=8 || t->ally1_y!=7) return GK45_UNKNOWN;
  } else if(t->fixture==GK45_RAW2 || t->fixture==GK45_RAW7) {
    if(t->ally1_soldiers!=40 || t->ally1_x!=6 || t->ally1_y!=9) return GK45_UNKNOWN;
  } else return GK45_UNKNOWN;
  Gk45Turn next=*t;
  next.x=8;next.y=7;next.soldiers=34;next.mobility=6;
  next.rng48=GK45_SOURCE_AFTER;next.steps=1;
  if(t->fixture==GK45_FRIEND_40){
    /* Captured native write sequence: 40->39->34, ally40->0, mover->74,
     * then two completion-word writes 0->1->2 and mobility refill. */
    next.ally1_soldiers=0;next.soldiers=74;
    next.mobility=10;next.completion_word=2;next.phase=GK45_DONE;
    *t=next;return GK45_FINISHED;
  }
  next.phase=GK45_PENDING;*t=next;return GK45_ACCEPTED;
}
Gk45Result gk45_step_second(Gk45Turn *t) {
  if (!t) return GK45_BAD;
  if(t->phase!=GK45_PENDING || t->steps!=1 || t->x!=8 || t->y!=7 ||
    t->soldiers!=34 || t->mobility!=6 || t->completion_word!=0 || t->rng48!=GK45_SOURCE_AFTER)
    return GK45_UNKNOWN;
  if(t->fixture==GK45_RAW7){t->phase=GK45_REFUSAL_MESSAGE;return GK45_REFUSED;}
  if(t->fixture!=GK45_RAW2) return GK45_UNKNOWN;
  t->x=9;t->y=7;t->mobility=3;t->steps=2;
  /* Raw terrain2: no new soldier casualty, no movement casualty RNG draw. */
  return GK45_ACCEPTED;
}
Gk45Result gk45_press_b(Gk45Turn *t) {
  if (!t) return GK45_BAD;
  if(t->phase==GK45_REFUSAL_MESSAGE){t->phase=GK45_PENDING;return GK45_ACCEPTED;}
  if(t->phase!=GK45_PENDING || t->steps==0 || t->completion_word!=0 ||
    (t->fixture!=GK45_RAW2 && t->fixture!=GK45_RAW7)) return GK45_UNKNOWN;
  t->phase=GK45_DONE;t->mobility=10;t->completion_word=1;
  return GK45_FINISHED;
}
Gk45Result gk45_divide_entry(uint16_t mobility) {
  switch(mobility) {
    case 10: case 11: case 12: return GK45_REFUSED;
    case 13: case 14: case 15: case 16: case 20: return GK45_PROMPT_QUANTITY;
    default: return GK45_UNKNOWN; /* 17..19 not directly probed */
  }
}
