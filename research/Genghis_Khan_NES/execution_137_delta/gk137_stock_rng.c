#include "gk137_stock_rng.h"
#include <limits.h>
#define GK137_MASK UINT64_C(0xffffffffffff)
#define GK137_MULT UINT64_C(0xe0900e0187c5)
Gk137Result gk137_from_nes_bytes(Gk137StockRng *out,const uint8_t bytes[6]){
 if(!out||!bytes)return GK137_BAD_INPUT;
 Gk137StockRng n={0};
 for(unsigned i=0;i<6;i++)n.seed48=(n.seed48<<8)|bytes[i];
 *out=n;return GK137_OK;
}
void gk137_to_nes_bytes(const Gk137StockRng *rng,uint8_t bytes[6]){
 if(!rng||!bytes)return;
 for(unsigned i=0;i<6;i++)bytes[i]=(uint8_t)(rng->seed48>>(8u*(5u-i)));
}
Gk137Result gk137_draw15(Gk137StockRng *rng,uint16_t *out){
 if(!rng||!out||rng->seed48>GK137_MASK||rng->draws==UINT64_MAX)
  return GK137_BAD_INPUT;
 /* Unsigned wrapping multiplication is defined by C11; its low 48 bits
  * equal the original mod-2^48 hardware computation. */
 uint64_t product=((rng->seed48|UINT64_C(1))*GK137_MULT)&GK137_MASK;
 uint64_t next=(UINT64_C(0)-product)&GK137_MASK;
 uint16_t value=(uint16_t)((next>>32u)&0x7fffu);
 rng->seed48=next;rng->draws++;
 *out=value;return GK137_OK;
}
Gk137Result gk137_bounded(Gk137StockRng *rng,uint16_t n,uint16_t *out){
 if(!rng||!out||rng->seed48>GK137_MASK)return GK137_BAD_INPUT;
 if(!n){*out=0u;return GK137_OK;}
 uint16_t v=0;
 if(gk137_draw15(rng,&v)!=GK137_OK)return GK137_BAD_INPUT;
 *out=(uint16_t)(v%n);return GK137_OK;
}
Gk137Result gk137_spring_rate(Gk137StockRng *rng,uint8_t difficulty,
                              uint16_t *out_rate){
 if(!rng||!out_rate||difficulty<1u||difficulty>5u)
  return GK137_BAD_INPUT;
 uint16_t draw;
 if(gk137_bounded(rng,(uint16_t)(94u+7u*difficulty),&draw)!=GK137_OK)
  return GK137_BAD_INPUT;
 *out_rate=(uint16_t)(50u+draw);return GK137_OK;
}
Gk137Result gk137_rate_decline_one_draw(Gk137StockRng *rng,uint16_t current,
                                        uint16_t *out_rate,uint8_t *percent){
 if(!rng||!out_rate||!percent||current<50u||current>999u)
  return GK137_BAD_INPUT;
 uint16_t roll;
 if(gk137_bounded(rng,9u,&roll)!=GK137_OK)return GK137_BAD_INPUT;
 uint8_t p=(uint8_t)(1u+roll);
 uint16_t lost=(uint16_t)((uint32_t)current*p/100u);
 uint16_t next=(uint16_t)(current-lost);
 *out_rate=next<50u?50u:next;
 *percent=p;return GK137_OK;
}
