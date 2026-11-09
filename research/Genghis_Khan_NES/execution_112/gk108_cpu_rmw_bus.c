#include "gk108_cpu_rmw_bus.h"
int gk108_cpu_rmw_pair(uint64_t final_cpu_cycle,uint16_t address,
                       uint8_t old_value,uint8_t new_value,
                       Gk108WriteBus writer,void *ctx){
 if(!writer||!ctx||final_cpu_cycle==0u)return 0;
 writer(ctx,address,old_value,final_cpu_cycle-1u);
 writer(ctx,address,new_value,final_cpu_cycle);
 return 1;
}
