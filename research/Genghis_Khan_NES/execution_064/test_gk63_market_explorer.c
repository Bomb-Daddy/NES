/* Integration recovery of EXEC0063 controller price navigation for the SAME
 * Gk1App. 063 ZIP inaccessible; not a byte-for-byte recovery claim. */
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define C(x) do{checks++;if(!(x)){fprintf(stderr,"EXEC0064/063 FAIL:%d %s\n",__LINE__,#x);return 2;}}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 4;
 Gk1TemplateDataset *d=malloc(sizeof *d);
 Gk1App *app=malloc(sizeof *app);
 if(!d||!app)return 5;
 C(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 gk1_app_init(app);C(gk1_app_install_templates(app,d));
 Gk1LoadedScenario world0=app->worlds[0],world1=app->worlds[1];
 C(app->screen==GK1_SCREEN_TITLE);
 C(gk1_app_input(app,GK1_INPUT_CONFIRM)==1); /* two campaigns */
 C(gk1_app_input(app,GK1_INPUT_DOWN)==1); /* World */
 C(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 C(app->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 C(gk1_app_input(app,GK1_INPUT_LABOR_PLAN)==1);
 C(gk1_app_input(app,GK1_INPUT_LABOR_PLAN)==1);
 C(app->screen==GK1_SCREEN_MARKET_EXPLORER);
 for(unsigned merchant=0;merchant<3;merchant++){
  C(app->market_merchant==merchant);
  for(unsigned item=0;item<17;item++){
   C(app->market_item==item);
   Gk61Quote q={0};
   C(gk61_quote(app->market_example_rate,(Gk61Merchant)merchant,(uint8_t)item,&q)==GK61_OK);
   C(q.buy_tenths>0);
   C(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
   C(app->market_sell==(item<11));
   if(item<11)C(gk1_app_input(app,GK1_INPUT_CONFIRM)==1&&app->market_sell==0);
   C(gk1_app_input(app,GK1_INPUT_DOWN)==1);
  }
  C(app->market_item==0);
  C(gk1_app_input(app,GK1_INPUT_RIGHT)==1);
 }
 C(app->market_merchant==0);
 C(gk1_app_input(app,GK1_INPUT_SETUP)==1&&app->market_example_rate==110);
 for(unsigned i=0;i<500;i++)C(gk1_app_input(app,GK1_INPUT_SETUP)==1);
 C(app->market_example_rate==999);
 for(unsigned i=0;i<500;i++)C(gk1_app_input(app,GK1_INPUT_LABOR_PLAN)==1);
 C(app->market_example_rate==50);
 C(!memcmp(&world0,&app->worlds[0],sizeof world0));
 C(!memcmp(&world1,&app->worlds[1],sizeof world1));
 C(app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 C(gk1_app_input(app,GK1_INPUT_CANCEL)==1);
 C(app->screen==GK1_SCREEN_LABOR_PLAN);
 Gk61Market m={0};m.playable=1;m.rate=100;m.merchant_present[0]=1;m.buy_offer[0][0]=1;
 Gk61Actor actor={0};actor.orders=3;actor.ability[1]=100;
 C(gk1_app_trade(app,&actor,&m,1,GK61_CHINA,GK61_BUY,0,1,-1)==GK61_NOT_PLAYABLE);
 C(!memcmp(&world0,&app->worlds[0],sizeof world0));
 C(!memcmp(&world1,&app->worlds[1],sizeof world1));
 printf("EXEC0064 reconstructed 063 source quote UI: %u checks PASS, no season/trade mutations\n",checks);
 free(d);free(app);return 0;
}
