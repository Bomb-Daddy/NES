#include "gk237_owner_fresh_win.h"
#include "gk166_fresh_entry.h"
#include <stdio.h>
#include <string.h>

Gk237OwnerResult gk237_owner_fresh_boot(Gk1App *a,
 const char *csv,const char *private_json){
 if(!a||!csv||!*csv)return GK237_INVALID_ARGUMENT;
 if(!private_json||!*private_json)return GK237_NOT_REQUESTED;
 /* Deliberately never interpret a file as starting a new campaign. Only a
  * pristine, already verified 14-country source template can receive this
  * original NES observation, and the World template remains untouched. */
 if(a->screen!=GK1_SCREEN_TITLE||a->stock_checkpoint.valid||
    !a->world_ready[0]||!a->world_ready[1]||
    a->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[0].country_count!=14u||
    a->worlds[1].country_count!=27u||a->rules_profile>1u)
  return GK237_REFUSED_CONTEXT;
 /* GK166 independently checks the exact original-source CSV and SHA-pinned
  * fresh JSON 14x83 record payload. All refusal paths leave app untouched.
  * No licensed bytes or imported observations are built into this program. */
 if(gk166_enter_original_fresh_json(a,csv,private_json,0u)!=GK166_OK)
  return GK237_REFUSED_SOURCE;
 a->rules_profile=0u;
 snprintf(a->status,sizeof a->status,
  "OWNER-PRIVATE FRESH 1175 RESTORED: 3 SOURCE ORDERS; NOT NES SETUP/RNG");
 return GK237_IMPORTED;
}
