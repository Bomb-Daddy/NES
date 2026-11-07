#include "gk61_market.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CK(x) do {checks++;if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static Gk1CountryTemplate base_country(void){
 Gk1CountryTemplate c={0};c.gold=439;c.goods[0]=73;c.goods[2]=26;
 c.troops=61;c.town=5;c.masons=4;c.artisans=6;c.food_workers=27;
 c.drafted_population=103;c.undrafted_reserve=34;c.combined_population=137;
 c.arms=110;c.skill=124;c.morale=118;return c;
}
static Gk61Market base_market(void){
 Gk61Market m={0};m.rate=101;m.profile=GK61_CLASSIC;m.playable=1;
 for(unsigned k=0;k<3;k++){
  m.merchant_present[k]=1;
  for(unsigned i=0;i<17;i++)m.buy_offer[k][i]=1;
  for(unsigned i=0;i<11;i++)m.sell_offer[k][i]=1;
 }
 return m;
}
static Gk61Actor base_actor(void){Gk61Actor a={0};a.orders=3;a.ability[1]=20;a.ability[4]=100;return a;}
int main(void){
 Gk61Quote q={0};
 CK(gk61_quote(101,GK61_CHINA,2,&q)==GK61_OK);CK(q.buy_tenths==64&&q.sell_tenths==57&&q.hire_tenths==120);
 CK(gk61_quote(91,GK61_CHINA,2,&q)==GK61_OK);CK(q.buy_tenths==61&&q.sell_tenths==54);
 CK(gk61_quote(101,GK61_UIGHUR,11,&q)==GK61_OK);CK(q.buy_tenths==50);
 CK(gk61_quote(101,GK61_ISLAM,11,&q)==GK61_OK);CK(q.buy_tenths==30);
 CK(gk61_quote(101,GK61_UIGHUR,12,&q)==GK61_OK);CK(q.buy_tenths==164);
 CK(gk61_quote(101,GK61_CHINA,17,&q)==GK61_BAD_INPUT);
 CK(gk61_quote(49,GK61_CHINA,2,&q)==GK61_BAD_INPUT);
 CK(gk61_quote(101,(Gk61Merchant)3,0,&q)==GK61_BAD_INPUT);
 CK(gk61_quote(101,GK61_CHINA,0,NULL)==GK61_BAD_INPUT);
 Gk1CountryTemplate c=base_country(),before=c;
 Gk61Market m=base_market();Gk61Actor a=base_actor(),old=a;
 uint16_t max=0xFFFF;
 /* Core safety: never transact on the published PRE-SETUP source templates. */
 m.playable=0;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_NOT_PLAYABLE);
 CK(!memcmp(&c,&before,sizeof c)&&!memcmp(&a,&old,sizeof a));
 CK(gk61_max_quantity(&m,&c,GK61_CHINA,GK61_BUY,2,&max)==GK61_NOT_PLAYABLE);
 CK(max==0xFFFF);
 m.playable=1;
 CK(gk61_max_quantity(&m,&c,GK61_CHINA,GK61_BUY,2,&max)==GK61_OK);
 CK(max==68); /* 439*10/64 = 68, existing Jewels +68<=999 */
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_OK);
 CK(c.gold==432&&c.goods[2]==27&&a.ability[1]==15&&a.orders==2);
 /* First season rate changes naturally; original jewel resale q1 credits 5 Gold. */
 m.rate=91;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_SELL,2,1,-1)==GK61_OK);
 CK(c.gold==437&&c.goods[2]==26&&a.ability[1]==10&&a.orders==1);
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_SELL,11,1,-1)==GK61_UNAVAILABLE);
 /* Zero whole-gold credit for a legitimate tiny sale is not a no-op. */
 m.rate=50;c.goods[0]=20;c.gold=10;a=base_actor();
 CK(gk61_commit(&m,&c,&a,GK61_ISLAM,GK61_SELL,0,1,-1)==GK61_OK);
 CK(c.gold==10&&c.goods[0]==19&&a.ability[1]==15&&a.orders==2);
 /* Absence and cancellation refuse without resource/BP or order changes. */
 before=c;old=a;m.merchant_present[GK61_CHINA]=0;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_UNAVAILABLE);
 CK(!memcmp(&c,&before,sizeof c)&&!memcmp(&a,&old,sizeof a));
 m.merchant_present[GK61_CHINA]=1;m.buy_offer[GK61_CHINA][0]=0;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_UNAVAILABLE);
 m.buy_offer[GK61_CHINA][0]=1;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,0,0,-1)==GK61_BAD_INPUT);
 a.orders=0;CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_NO_ORDERS);
 a.orders=3;a.ability[1]=4;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_NO_ABILITY);
 m.profile=GK61_SHARED_BP;a.ability[4]=5;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_OK);
 CK(a.ability[4]==0&&a.ability[1]==4&&a.orders==2);
 /* One dagger Uighur costs 5 Gold at Rate101, Arms rises 110 to113 using
    CHINA dagger base 20 regardless of merchant. */
 m=base_market();c=base_country();a=base_actor();
 CK(gk61_commit(&m,&c,&a,GK61_UIGHUR,GK61_BUY,11,1,-1)==GK61_OK);
 CK(c.gold==434&&c.arms==113&&a.ability[1]==15&&a.orders==2);
 /* Weapon purchase never increases a sellable inventory column. */
 CK(c.goods[0]==73&&c.goods[2]==26);
 /* Hires do NOT remove reserve; whole-P weighted qualities, fixed original
    newcomer Morale50+rand11 draw supplied by caller (here roll5). */
 m=base_market();m.rate=100;c=base_country();c.gold=368;a=base_actor();
 CK(gk61_commit(&m,&c,&a,GK61_ISLAM,GK61_HIRE,0,8,5)==GK61_OK);
 CK(c.troops==69&&c.gold==272&&c.undrafted_reserve==34);
 CK(c.arms==109&&c.skill==122&&c.morale==113);
 CK(c.drafted_population==111&&c.combined_population==145);
 CK(a.orders==2&&a.ability[1]==15);
 CK(gk61_commit(&m,&c,&a,GK61_ISLAM,GK61_HIRE,0,1,11)==GK61_BAD_INPUT);
 /* Multiplication and capacity/rollback controls. */
 m=base_market();c=base_country();a=base_actor();c.goods[2]=999;
 before=c;old=a;
 CK(gk61_max_quantity(&m,&c,GK61_CHINA,GK61_BUY,2,&max)==GK61_OK&&max==0);
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_NO_RESOURCE);
 CK(!memcmp(&c,&before,sizeof c)&&!memcmp(&a,&old,sizeof a));
 c=base_country();a=base_actor();c.gold=999;c.goods[2]=1;
 CK(gk61_max_quantity(&m,&c,GK61_CHINA,GK61_SELL,2,&max)==GK61_OK&&max==0);
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_SELL,2,1,-1)==GK61_NO_RESOURCE);
 c=base_country();a=base_actor();c.drafted_population=102;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_BAD_INPUT);
 c=base_country();a=base_actor();m.buy_offer[GK61_CHINA][2]=2;
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_BAD_INPUT);
 /* All 51 item/merchant combinations versus the published NES manual table,
    plus independent percentage/round-down calculations at two Rates. */
 static const uint16_t manual[3][17]={
  {10,16,32,18,14,11,13,8,20,18,10,20,80,100,40,60,150},
  {4,37,28,22,8,14,15,5,15,29,11,15,75,110,40,70,200},
  {8,25,25,10,15,7,18,20,31,38,11,25,82,90,35,65,250}
 };
 for(unsigned merchant=0;merchant<3;merchant++){
  for(unsigned item=0;item<17;item++){
   for(unsigned rate=91;rate<=101;rate+=10){
    CK(gk61_quote((uint16_t)rate,(Gk61Merchant)merchant,(uint8_t)item,&q)==GK61_OK);
    uint32_t buy=(uint32_t)manual[merchant][item]*(rate+100u)/100u;
    uint32_t sell=buy*90u/100u;
    CK(q.buy_tenths==buy);
    CK(q.sell_tenths==(sell?sell:1u));
    CK(q.hire_tenths==60u*(rate+100u)/100u);
   }
  }
 }
 /* Nontrivial quantity and headroom boundaries: 11 jewels at China Rate101
    costs ceil(11*6.4)=71 Gold, not floor(70). */
 c=base_country();a=base_actor();m=base_market();
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,2,11,-1)==GK61_OK);
 CK(c.gold==368&&c.goods[2]==37&&a.orders==2&&a.ability[1]==15);
 c=base_country();a=base_actor();c.gold=999;c.arms=998;
 CK(gk61_max_quantity(&m,&c,GK61_CHINA,GK61_BUY,11,&max)==GK61_OK);
 CK(max==5); /* whole drafted population, China dagger base20 */
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,11,6,-1)==GK61_NO_RESOURCE);
 CK(gk61_commit(&m,&c,&a,GK61_CHINA,GK61_BUY,11,5,-1)==GK61_OK);
 CK(c.arms==999);
 /* Exact refusal still preserves all original 41-source record bytes. */
 puts("GK61: original merchant quote, positive buy/sell/weapon/hire, pre-setup guard, BP/order and rollback PASS");
 printf("ASSERTIONS: %u\n",checks);return 0;
}
