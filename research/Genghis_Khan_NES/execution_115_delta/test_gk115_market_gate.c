#include "gk1_app_state.h"
#include "gk115_market_gate.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"GK115 failure %u: %s\n",(unsigned)__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 CK(argc==2);
 Gk1TemplateDataset *ds=calloc(1,sizeof *ds);
 Gk1App *app=calloc(1,sizeof *app),*old=malloc(sizeof *old);
 CK(ds&&app&&old);
 CK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 gk1_app_init(app);CK(gk1_app_install_templates(app,ds));
 Gk61Market m={0};m.playable=1;m.profile=GK61_CLASSIC;m.rate=121;
 m.merchant_present[0]=1;m.buy_offer[0][0]=1;
 Gk61Actor actor={0};actor.orders=3;actor.ability[1]=90;
 CK(gk115_stock_market_gate(&app->stock_checkpoint,&app->worlds[0],&m,0,1)==GK115_MARKET_WORLD_PRE_SETUP);
 *old=*app;Gk61Actor actor_old=actor;
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_NOT_PLAYABLE);
 CK(!memcmp(old,app,sizeof *app)&&!memcmp(&actor_old,&actor,sizeof actor));
 app->worlds[0].phase=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE;
 CK(gk115_stock_market_gate(&app->stock_checkpoint,&app->worlds[0],&m,0,1)==GK115_MARKET_STOCK_SOURCE_MISSING);
 *old=*app;
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_NOT_PLAYABLE);
 CK(!memcmp(old,app,sizeof *app)&&!memcmp(&actor_old,&actor,sizeof actor));
 /* Forged source flag + copied source-world data is NOT a SHA authenticated
  * original Mesen export; it must not authorize transaction or actor charges. */
 app->stock_checkpoint.valid=1;app->stock_checkpoint.source_verified=1;
 app->stock_checkpoint.world=app->worlds[0];
 app->stock_checkpoint.year=1175;app->stock_checkpoint.rate=m.rate;
 app->stock_checkpoint.orders.country_id=1;app->stock_checkpoint.orders.season=GK71_SPRING;
 *old=*app;
 CK(gk115_stock_market_gate(&app->stock_checkpoint,&app->worlds[0],&m,0,1)==GK115_MARKET_STOCK_SOURCE_MISSING);
 CK(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_NOT_PLAYABLE);
 CK(!memcmp(old,app,sizeof *app)&&!memcmp(&actor_old,&actor,sizeof actor));
 CK(gk115_stock_market_gate(NULL,&app->worlds[0],&m,0,1)==GK115_MARKET_BAD_INPUT);
 CK(gk115_stock_market_gate(&app->stock_checkpoint,&app->worlds[0],&m,0,0)==GK115_MARKET_BAD_INPUT);
 CK(gk115_stock_market_gate(&app->stock_checkpoint,&app->worlds[0],&m,2,1)==GK115_MARKET_BAD_INPUT);
 puts("GK115 TESTED_PORT_LINUX: fake post-setup + fake offers + fake checkpoint all fail closed; original numeric engine preserved");
 free(old);free(app);free(ds);return 0;
}
