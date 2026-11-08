#include "gk75_spring_economy.h"
#include <stddef.h>
static unsigned total(const Gk75SpringFields *s){
 return (unsigned)s->troops+s->town+s->masons+s->artisans+s->food_workers;
}
static int valid(const Gk75SpringFields *s){
 const uint16_t fields[]={s->troops,s->town,s->masons,s->artisans,s->food_workers,
                          s->food,s->gold,s->economy,s->morale,s->tax};
 for(unsigned i=0;i<sizeof fields/sizeof fields[0];i++)if(fields[i]>999u)return 0;
 return total(s)<=999u;
}
Gk75Result gk75_spring_food_tax_pay(const Gk75SpringFields *in,Gk75SpringResult *out){
 if(!in||!out||!valid(in))return GK75_BAD_INPUT;
 Gk75SpringResult tmp={0};tmp.after=*in;
 unsigned p=total(in),food=in->food;
 if(p>2u*food){
  /* Native 6502 repeatedly scales EACH group independently until P <= 2F.
   * Any shortage finishes with zero food, even if rounding frees units. */
  tmp.starved=1;
  for(unsigned step=0;p>2u*food;step++){
   if(step>1000u)return GK75_BAD_INPUT; /* never commit non-convergent input */
   unsigned q=(200u*food)/p;
   tmp.after.troops=(uint16_t)(tmp.after.troops*q/100u);
   tmp.after.town=(uint16_t)(tmp.after.town*q/100u);
   tmp.after.masons=(uint16_t)(tmp.after.masons*q/100u);
   tmp.after.artisans=(uint16_t)(tmp.after.artisans*q/100u);
   tmp.after.food_workers=(uint16_t)(tmp.after.food_workers*q/100u);
   unsigned next=total(&tmp.after);
   if(next>=p)return GK75_BAD_INPUT;
   p=next;
  }
  tmp.after.food=0;
 }else{
  tmp.after.food=(uint16_t)(food-p/2u);
 }
 tmp.food_consumed=(uint16_t)(food-tmp.after.food);
 /* ROM $8106: tax is credited BEFORE wages. Do not clamp the intermediate
  * treasury at 999; clamp only after 1 Gold per surviving troop payroll. */
 unsigned a=(in->economy*in->tax)/100u;
 unsigned f=12u+((in->morale/9u)<13u?(in->morale/9u):13u);
 unsigned gross=(a*f)/25u;
 unsigned available=in->gold+gross;
 unsigned surviving=tmp.after.troops;
 if(surviving>available){surviving=available;tmp.after.troops=(uint16_t)surviving;}
 tmp.gross_tax=(uint16_t)gross;
 tmp.payroll=(uint16_t)surviving;
 tmp.troops_lost=(uint16_t)(in->troops-surviving);
 unsigned balance=available-surviving;
 tmp.after.gold=(uint16_t)(balance>999u?999u:balance);
 *out=tmp;return GK75_OK;
}
Gk75Result gk75_template_projection(const Gk1CountryTemplate *c,Gk75SpringResult *out){
 if(!c||!out)return GK75_BAD_INPUT;
 Gk75SpringFields in={c->troops,c->town,c->masons,c->artisans,c->food_workers,
                     c->goods[10],c->gold,c->economy,c->morale,c->tax};
 return gk75_spring_food_tax_pay(&in,out);
}
