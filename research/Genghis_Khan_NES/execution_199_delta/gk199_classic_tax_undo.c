#include "gk199_classic_tax_undo.h"
#include <string.h>
Gk187Result gk199_undo_last_tax(Gk187ClassicAssign *state,
 const Gk79Checkpoint *source, uint8_t *selection){
 if(!state||!source||!selection)return GK187_INVALID;
 if(!gk187_intact(state,source))return GK187_SOURCE;
 if(state->passed)return GK187_EXHAUSTED;
 if(state->action_count==0u)return GK187_NO_ACTION;
 const Gk187Action last=state->actions[state->action_count-1u];
 if(last.from!=5u||last.to!=5u||last.quantity<1u||last.quantity>100u)
  return GK187_INVALID;
 Gk187ClassicAssign restored=*state;
 Gk187Result result=gk187_undo_last(&restored,source);
 if(result!=GK187_OK)return result;
 if(restored.player_index>=restored.mutable_world.country_count ||
    !gk187_intact(&restored,source))return GK187_SOURCE;
 uint16_t rate=restored.mutable_world.countries[restored.player_index].tax;
 if(rate<1u||rate>100u)return GK187_SOURCE;
 /* Always publish state and selection together, only after replay succeeds. */
 memcpy(state,&restored,sizeof restored);
 *selection=(uint8_t)rate;
 return GK187_OK;
}
