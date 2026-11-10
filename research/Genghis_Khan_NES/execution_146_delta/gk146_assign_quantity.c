#include "gk146_assign_quantity.h"
#include <stddef.h>
static const uint16_t values[]={1,5,10,25,50,100};
int gk146_quantity_is_valid(uint16_t value){
 for(size_t i=0;i<sizeof(values)/sizeof(values[0]);i++)
  if(values[i]==value)return 1;
 return 0;
}
Gk146QuantityResult gk146_quantity_step(uint16_t before,int direction,uint16_t *next){
 if(!next||(direction!=-1&&direction!=1))return GK146_QUANTITY_INVALID;
 for(size_t i=0;i<sizeof(values)/sizeof(values[0]);i++){
  if(values[i]==before){
   const size_t n=sizeof(values)/sizeof(values[0]);
   *next=values[(i+(direction==1?1u:n-1u))%n];
   return GK146_QUANTITY_OK;
  }
 }
 return GK146_QUANTITY_INVALID;
}
