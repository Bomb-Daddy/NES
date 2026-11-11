#include "gk194_classic_storage_gate.h"
int gk194_classic_storage_allowed(const Gk1App *a){
 if(!a||a->rules_profile!=0u||!a->stock_checkpoint.valid||
    a->stock_checkpoint.source_verified!=1u||
    !gk85_raw_source_intact(&a->stock_checkpoint)||
    !gk88_stock_projection_intact(&a->stock_checkpoint)||
    a->enhanced_assign.active||a->enhanced_summer.active||
    a->enhanced_fall.active||a->enhanced_winter.active||
    a->enhanced_year2.active||a->enhanced_continuation.active)return 0;
 return !a->classic_assign.active||
         gk187_intact(&a->classic_assign,&a->stock_checkpoint);
}
