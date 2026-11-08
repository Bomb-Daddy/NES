#include "gk61_market.h"
#include <limits.h>
#include <string.h>
/* NES-GX USA / accepted economic source v1.07: 17 item columns, 3 merchant
   bases, one global Rate. These are DATA not complete merchant stock/offers. */
static const uint16_t k_base[3][17]={
 {10,16,32,18,14,11,13,8,20,18,10,20,80,100,40,60,150},
 {4,37,28,22,8,14,15,5,15,29,11,15,75,110,40,70,200},
 {8,25,25,10,15,7,18,20,31,38,11,25,82,90,35,65,250}
};
static uint16_t min2(uint16_t a,uint16_t b){return a<b?a:b;}
static int valid_market(const Gk61Market *m){
 if(!m||m->rate<50u||m->rate>999u||(m->profile!=GK61_CLASSIC&&m->profile!=GK61_SHARED_BP))return 0;
 for(unsigned k=0;k<3;k++){
  if(m->merchant_present[k]>1)return 0;
  for(unsigned j=0;j<17;j++)if(m->buy_offer[k][j]>1)return 0;
  for(unsigned j=0;j<11;j++)if(m->sell_offer[k][j]>1)return 0;
 }
 return 1;
}
static int valid_country(const Gk1CountryTemplate *c){
 if(!c||c->gold>999||c->arms>999||c->skill>999||c->morale>999||c->troops>999||
    c->town>999||c->masons>999||c->artisans>999||c->food_workers>999||
    c->undrafted_reserve>999||c->eligibility_percent>100)return 0;
 uint32_t p=(uint32_t)c->troops+c->town+c->masons+c->artisans+c->food_workers;
 if(p>999u||p!=c->drafted_population||p+c->undrafted_reserve!=c->combined_population)return 0;
 for(unsigned i=0;i<11;i++)if(c->goods[i]>999)return 0;
 return 1;
}
Gk61Result gk61_quote(uint16_t rate,Gk61Merchant merchant,uint8_t item,Gk61Quote *out){
 if(!out||merchant>GK61_UIGHUR||item>=17||rate<50||rate>999)return GK61_BAD_INPUT;
 /* v1.07 uses a wide multiply to avoid native u16 overflow here; source
    quote truncates before sell 90%. Rate is globally shared by merchants. */
 uint32_t price=(uint32_t)k_base[merchant][item]*(rate+100u)/100u;
 if(price>UINT16_MAX)return GK61_BAD_INPUT;
 Gk61Quote q={(uint16_t)price,(uint16_t)(price*90u/100u),
              (uint16_t)(60u*(rate+100u)/100u)};
 if(q.sell_tenths==0)q.sell_tenths=1;
 *out=q;return GK61_OK;
}
static Gk61Result validated_request(const Gk61Market *m,const Gk1CountryTemplate *c,
        Gk61Merchant k,Gk61Action action,uint8_t item,Gk61Quote *q){
 if(!valid_market(m)||!valid_country(c)||k>GK61_UIGHUR||action>GK61_HIRE||item>=17)return GK61_BAD_INPUT;
 if(!m->playable)return GK61_NOT_PLAYABLE;
 if(!m->merchant_present[k])return GK61_UNAVAILABLE;
 if(action==GK61_BUY && !m->buy_offer[k][item])return GK61_UNAVAILABLE;
 if(action==GK61_SELL && (item>=GK61_SELLABLE_COUNT||!m->sell_offer[k][item]))return GK61_UNAVAILABLE;
 if(action==GK61_HIRE && item!=0u)return GK61_BAD_INPUT; /* canonical dummy item0 for Hire */
 return gk61_quote(m->rate,k,action==GK61_HIRE?0u:item,q);
}
static uint16_t headroom(uint16_t current){return (uint16_t)(999u-current);}
Gk61Result gk61_max_quantity(const Gk61Market *m,const Gk1CountryTemplate *c,
                             Gk61Merchant k,Gk61Action action,uint8_t item,uint16_t *out){
 if(!out)return GK61_BAD_INPUT;
 Gk61Quote q={0};Gk61Result r=validated_request(m,c,k,action,item,&q);
 if(r!=GK61_OK)return r;
 uint32_t p=(uint32_t)c->troops+c->town+c->masons+c->artisans+c->food_workers;
 uint32_t n=0;
 if(action==GK61_BUY){
  n=((uint32_t)10u*c->gold)/q.buy_tenths;
  if(item<11u)n=min2((uint16_t)n,headroom(c->goods[item]));
  else {
   if(p==0)return GK61_NO_CAPACITY;
   uint32_t a=p*headroom(c->arms)/k_base[GK61_CHINA][item];
   if(n>a)n=a;
   if(n>999)n=999;
  }
 }else if(action==GK61_SELL){
  n=(uint32_t)10u*headroom(c->gold)/q.sell_tenths;
  if(n>c->goods[item])n=c->goods[item];
 }else { /* Hire uses source buy quote from 60, not any commodity base. */
  n=(uint32_t)10u*c->gold/q.hire_tenths;
  uint32_t cap=999u-p;
  if(n>cap)n=cap;
 }
 if(n>999)n=999;
 *out=(uint16_t)n;return GK61_OK;
}
Gk61Result gk61_commit(const Gk61Market *m,Gk1CountryTemplate *c,Gk61Actor *actor,
                       Gk61Merchant k,Gk61Action action,uint8_t item,
                       uint16_t quantity,int morale_roll11){
 if(!actor||!quantity||!valid_country(c)||!valid_market(m)||actor->orders>3u)return GK61_BAD_INPUT;
 for(unsigned i=0;i<GK61_ABILITY_COUNT;i++)if(actor->ability[i]>999)return GK61_BAD_INPUT;
 if(action==GK61_HIRE && (morale_roll11<0||morale_roll11>10))return GK61_BAD_INPUT;
 Gk61Quote q={0};Gk61Result v=validated_request(m,c,k,action,item,&q);
 if(v!=GK61_OK)return v;
 if(!actor->orders)return GK61_NO_ORDERS;
 uint16_t *charge=m->profile==GK61_CLASSIC?&actor->ability[1]:&actor->ability[4];
 if(*charge<5)return GK61_NO_ABILITY;
 uint16_t max=0;v=gk61_max_quantity(m,c,k,action,item,&max);
 if(v!=GK61_OK)return v;
 if(quantity>max)return GK61_NO_RESOURCE;
 Gk1CountryTemplate next=*c;Gk61Actor next_actor=*actor;
 uint32_t n=(uint32_t)quantity;
 if(action==GK61_BUY){
  uint32_t debit=(n*q.buy_tenths+9u)/10u;
  if(debit>next.gold)return GK61_NO_RESOURCE;
  next.gold=(uint16_t)(next.gold-debit);
  if(item<11u)next.goods[item]=(uint16_t)(next.goods[item]+quantity);
  else {
   uint32_t p=next.drafted_population;
   if(!p)return GK61_NO_CAPACITY;
   uint32_t gain=9u*k_base[GK61_CHINA][item]*(n+1u)/p;
   uint32_t resulting=next.arms+gain;
   next.arms=(uint16_t)(resulting>999u?999u:resulting);
  }
 }else if(action==GK61_SELL){
  uint32_t credit=n*q.sell_tenths/10u;
  if(credit>headroom(next.gold))return GK61_NO_RESOURCE;
  next.gold=(uint16_t)(next.gold+credit);
  next.goods[item]=(uint16_t)(next.goods[item]-quantity);
 }else {
  uint32_t p=next.drafted_population,denom=p+n;
  if(denom>999u||!denom)return GK61_NO_CAPACITY;
  uint32_t debit=(n*q.hire_tenths+9u)/10u;
  if(debit>next.gold)return GK61_NO_RESOURCE;
  /* Quality dilution is based on whole drafted P, not merely old troops. */
  next.arms=(uint16_t)((p*next.arms+n*100u)/denom);
  next.skill=(uint16_t)((p*next.skill+n*100u)/denom);
  next.morale=(uint16_t)((p*next.morale+n*(50u+(unsigned)morale_roll11))/denom);
  next.troops=(uint16_t)(next.troops+quantity);
  next.drafted_population=(uint16_t)denom;
  next.combined_population=(uint16_t)(denom+next.undrafted_reserve); /* loaded resident accounting */
  next.gold=(uint16_t)(next.gold-debit);
 }
 next_actor.orders--;
 if(m->profile==GK61_CLASSIC)next_actor.ability[1]-=5;
 else next_actor.ability[4]-=5;
 *c=next;*actor=next_actor;return GK61_OK;
}
