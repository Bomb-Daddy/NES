#include "gk183_checkpoint_isolation.h"
#include <string.h>
int gk183_classic_storage_isolated(const Gk1App *a){
 if(!a||a->rules_profile!=0u)return 0;
 return !a->enhanced_assign.active&&!a->enhanced_summer.active&&
   !a->enhanced_fall.active&&!a->enhanced_winter.active&&
   !a->enhanced_year2.active&&!a->enhanced_continuation.active;
}
void gk183_reset_checkpoint_derived(Gk1App *a){
 if(!a)return;
 memset(&a->enhanced_assign,0,sizeof a->enhanced_assign);
 memset(&a->enhanced_summer,0,sizeof a->enhanced_summer);
 memset(&a->enhanced_fall,0,sizeof a->enhanced_fall);
 memset(&a->enhanced_winter,0,sizeof a->enhanced_winter);
 memset(&a->enhanced_year2,0,sizeof a->enhanced_year2);
 memset(&a->enhanced_continuation,0,sizeof a->enhanced_continuation);
 a->stock_storage_cursor=0u;
 a->stock_storage_confirm=0u;
 a->stock_storage_request=0u;
}
