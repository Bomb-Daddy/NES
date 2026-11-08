/* EXEC0062: actual Gk1App and 41-country source dataset; NO original setup
   or true strategic turn may be fabricated. Positive branch is a separately
   marked synthetic *adapter* test, not a gameplay/campaign acceptance. */
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CK(x) do{checks++;if(!(x)){fprintf(stderr,"EXEC0062 FAIL line %d: %s\n",__LINE__,#x);return 2;}}while(0)
static Gk61Market market_init(void){
 Gk61Market m={0};m.playable=1;m.profile=GK61_CLASSIC;m.rate=101;
 m.merchant_present[0]=1;m.buy_offer[0][2]=1;m.sell_offer[0][2]=1;
 return m;
}
int main(int argc,char **argv){
 if(argc!=2)return 3;
 Gk1TemplateDataset *ds=malloc(sizeof *ds);
 Gk1App *app=malloc(sizeof *app);
 Gk1App *prior=malloc(sizeof *prior);
 if(!ds||!app||!prior)return 4;
 CK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 gk1_app_init(app);
 CK(gk1_app_install_templates(app,ds));
 CK(app->world_ready[0]&&app->world_ready[1]);
 CK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 CK(app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 Gk61Market m=market_init();Gk61Actor actor={0};actor.orders=3;actor.ability[1]=100;
 *prior=*app;Gk61Actor actor_before=actor;
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_NOT_PLAYABLE);
 CK(memcmp(prior,app,sizeof *app)==0);
 CK(memcmp(&actor_before,&actor,sizeof actor)==0);
 app->active_world=1;*prior=*app;
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_NOT_PLAYABLE);
 CK(!memcmp(prior,app,sizeof *app));
 CK(!memcmp(&actor_before,&actor,sizeof actor));
 app->active_world=0;
 /* Positive integration is intentionally a synthetic future-phase fixture,
    NOT a statement that original NES setup or Spring has been implemented. */
 app->worlds[0].phase=(Gk1ScenarioPhase)2; /* synthetic state ONLY */
 m.playable=0;*prior=*app;
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_NOT_PLAYABLE);
 CK(!memcmp(prior,app,sizeof *app));
 m.playable=1;
 /* This country still has original PRE-SETUP amounts, so results below are
    ONLY for code integration, NOT authentic Spring1175 resource values. */
 const Gk1CountryTemplate c0=app->worlds[0].countries[0];
 CK(c0.gold==425&&c0.goods[2]==26&&c0.drafted_population==100);
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_OK);
 CK(app->worlds[0].countries[0].gold==418);
 CK(app->worlds[0].countries[0].goods[2]==27);
 CK(app->worlds[0].countries[1].gold==ds->countries[1].gold);
 CK(app->worlds[1].countries[0].gold==ds->countries[14].gold);
 CK(actor.orders==2&&actor.ability[1]==95);
 /* Invalid country and disabled offers are atomic even when context says
    future phase. Never mutate an unrelated lord or world when denied. */
 *prior=*app;actor_before=actor;
 CK(gk1_app_trade(app,&actor,&m,0,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_BAD_INPUT);
 CK(gk1_app_trade(app,&actor,&m,15,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_BAD_INPUT);
 CK(!memcmp(prior,app,sizeof *app));CK(!memcmp(&actor_before,&actor,sizeof actor));
 m.buy_offer[0][2]=0;
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_UNAVAILABLE);
 CK(!memcmp(prior,app,sizeof *app));CK(!memcmp(&actor_before,&actor,sizeof actor));
 app->worlds[0].phase=GK1_PRE_SETUP_TEMPLATE;*prior=*app;
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,2,1,-1)==GK61_NOT_PLAYABLE);
 CK(!memcmp(prior,app,sizeof *app));
 printf("EXEC0062 native app country integration/phase gate: %u checks PASS (synthetic positive fixture only)\n",checks);
 free(prior);free(app);free(ds);return 0;
}
