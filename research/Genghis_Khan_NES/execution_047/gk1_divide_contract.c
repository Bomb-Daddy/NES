#include "gk1_divide_contract.h"
#include <string.h>
#define GK47_SEED UINT64_C(0xc7c3f28f2363)
#define GK47_NEXT UINT64_C(0xa31e4ecd8fd1)
static int known_quantity(uint16_t q){return q==1u||q==6u||q==7u||q==8u;}
Gk47Result gk47_open(Gk47Divide *s){
 if(!s)return GK47_BAD;
 Gk47Divide next={0};next.phase=GK47_QUANTITY;next.source_soldiers=40;
 next.source_mobility=20;next.source_x=7;next.source_y=7;next.rng48=GK47_SEED;
 *s=next;return GK47_ACCEPTED;
}
Gk47Result gk47_choose_quantity(Gk47Divide *s,uint16_t q){
 if(!s)return GK47_BAD;
 if(s->phase!=GK47_QUANTITY || !known_quantity(q) || s->qty ||
    s->source_soldiers!=40 || s->source_mobility!=20 ||
    s->rng48!=GK47_SEED || s->source_x!=7 || s->source_y!=7 ||
    s->split_soldiers || s->completion_word)return GK47_UNKNOWN;
 Gk47Divide next=*s;next.qty=q;next.phase=GK47_DESTINATION;*s=next;
 return GK47_ACCEPTED;
}
Gk47Result gk47_move_to_observed_forest(Gk47Divide *s,uint8_t x,uint8_t y){
 if(!s)return GK47_BAD;
 if(s->phase!=GK47_DESTINATION || !known_quantity(s->qty) ||
    x!=8||y!=7||s->source_soldiers!=40||s->source_mobility!=20||
    s->completion_word!=0||s->split_soldiers!=0||s->rng48!=GK47_SEED ||
    s->source_x!=7||s->source_y!=7) return GK47_UNKNOWN;
 /* EXACT tested forest source algorithm: first stage floor(q*98/100),
  * then the original seeded rand(4)=2 adds 3, capped at remaining men.
  * Unseen numbers are deliberately unsupported. */
 uint16_t after=(uint16_t)(((uint32_t)s->qty*98u)/100u);
 uint16_t survivors=(uint16_t)(after>5u?after-5u:0u);
 Gk47Divide next=*s;
 next.source_soldiers=(uint16_t)(40u-s->qty);next.split_soldiers=survivors;
 next.split_x=8;next.split_y=7;next.split_type=0;next.split_mobility=10;
 next.rng48=GK47_NEXT;next.phase=survivors?GK47_PENDING:GK47_DEATH_PROMPT;
 *s=next;return survivors?GK47_ACCEPTED:GK47_DEAD_MESSAGE;
}
Gk47Result gk47_press_b(Gk47Divide *s){
 if(!s)return GK47_BAD;
 if(s->phase==GK47_DESTINATION){s->phase=GK47_DESTINATION_ERROR;return GK47_REFUSED;}
 if(s->phase==GK47_DESTINATION_ERROR){s->phase=GK47_DESTINATION;return GK47_ACCEPTED;}
 if(s->phase==GK47_DEATH_PROMPT){s->phase=GK47_PENDING;return GK47_ACCEPTED;}
 if(s->phase!=GK47_PENDING || !known_quantity(s->qty) ||
    s->source_soldiers!=(uint16_t)(40u-s->qty) ||
    s->source_mobility!=20||s->completion_word!=0||
    s->rng48!=GK47_NEXT || s->split_x!=8 || s->split_y!=7)
  return GK47_UNKNOWN;
 s->source_mobility=10;s->completion_word=1;s->phase=GK47_DONE;
 return GK47_FINISHED;
}
