#include "gk72_battle_pass.h"
#include <string.h>
static int valid(const Gk72BattlePass *s){
 return s&&s->valid==1&&s->selected<=1&&
        s->mobility[0]<=15&&s->mobility[1]<=13&&
        s->used[0]<=1&&s->used[1]<=1;
}
Gk72Status gk72_pass_begin(Gk72BattlePass *s,uint8_t cavalry_mobility,
                          uint8_t other_mobility){
 if(!s||cavalry_mobility>15||other_mobility>13)return GK72_REJECTED;
 Gk72BattlePass next;memset(&next,0,sizeof next);
 next.mobility[0]=cavalry_mobility;
 next.mobility[1]=other_mobility;
 next.valid=1;
 *s=next;return GK72_OK;
}
Gk72Status gk72_pass_select(Gk72BattlePass *s,uint8_t selected){
 if(!valid(s)||selected>1)return GK72_REJECTED;
 s->selected=selected;return GK72_OK;
}
Gk72Status gk72_pass_apply(Gk72BattlePass *s){
 if(!valid(s)||s->used[s->selected])return GK72_REJECTED;
 Gk72BattlePass next=*s;
 uint8_t cap=next.selected==0?15u:13u;
 /* The printed manual explicitly caps mobility. A Pass at cap is legal
  * for this rule drill but makes no numeric change. */
 if(next.mobility[next.selected]<cap)next.mobility[next.selected]++;
 next.used[next.selected]=1;
 *s=next;return GK72_OK;
}
Gk72Status gk72_pass_new_activation(Gk72BattlePass *s){
 if(!valid(s))return GK72_REJECTED;
 s->used[0]=0;s->used[1]=0;return GK72_OK;
}
