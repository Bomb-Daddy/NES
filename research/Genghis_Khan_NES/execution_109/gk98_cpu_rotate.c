#include "gk98_cpu_rotate.h"
Gk98Rotate gk98_cpu_ror(uint8_t old_value,uint8_t carry_in){
 Gk98Rotate r={(uint8_t)((old_value>>1u)|((carry_in&1u)<<7u)),(uint8_t)(old_value&1u)};
 return r;
}
