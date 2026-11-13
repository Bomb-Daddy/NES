#include "gk238_owner_startup.h"
Gk238Startup gk238_owner_startup(Gk1App *app,const char *canonical_csv,
 const char *private_json,int scenario_loaded,int diagnostic_mode){
 /* An empty/unset owner path cannot become an automatic source search. */
 if(!private_json||!*private_json)return GK238_UNREQUESTED;
 /* Protect both genuine NSIS-installed game smoke and CI from accidentally
  * consuming owner-private original bytes, even if an env var is inherited. */
 if(diagnostic_mode)return GK238_SMOKE_ISOLATED;
 if(!scenario_loaded||!app||!canonical_csv||!*canonical_csv)return GK238_REFUSED;
 return gk237_owner_fresh_boot(app,canonical_csv,private_json)==GK237_IMPORTED?
   GK238_OWNER_IMPORTED:GK238_REFUSED;
}
