#include "gk1_app_state.h"
#include "gk95_stock_trade_reference.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OK(x) do{if(!(x)){fprintf(stderr,"GK95 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int verified_case(const Gk1LoadedScenario *pre,const char *raw,
                         Gk77Profile profile,uint16_t authentic_rate){
 Gk1App *app=calloc(1,sizeof *app);OK(app);
 gk1_app_init(app);
 OK(gk79_open_original(pre,raw,profile,0,&app->stock_checkpoint)==GK79_OK);
 OK(app->stock_checkpoint.source_verified==1u);
 OK(app->stock_checkpoint.rate==authentic_rate);
 const Gk79Checkpoint initial=app->stock_checkpoint;
 Gk61Quote quote={0};
 for(unsigned m=0;m<3;m++)for(unsigned item=0;item<17;item++){
  Gk61Quote original={0};
  OK(gk61_quote(authentic_rate,(Gk61Merchant)m,(uint8_t)item,&original)==GK61_OK);
  OK(gk95_stock_trade_reference(&app->stock_checkpoint,(Gk61Merchant)m,
                   (uint8_t)item,&quote)==GK95_QUOTE_OK);
  OK(memcmp(&quote,&original,sizeof quote)==0);
 }
 OK(memcmp(&initial,&app->stock_checkpoint,sizeof initial)==0);
 app->screen=GK1_SCREEN_STOCK_COMMANDS;
 app->stock_command_cursor=4;
 OK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 OK(app->screen==GK1_SCREEN_STOCK_TRADE_REFERENCE);
 OK(app->stock_trade_merchant==0&&app->stock_trade_item==0);
 OK(gk1_app_input(app,GK1_INPUT_LEFT));OK(app->stock_trade_merchant==2);
 OK(gk1_app_input(app,GK1_INPUT_RIGHT));OK(app->stock_trade_merchant==0);
 OK(gk1_app_input(app,GK1_INPUT_UP));OK(app->stock_trade_item==16);
 OK(gk1_app_input(app,GK1_INPUT_DOWN));OK(app->stock_trade_item==0);
 OK(gk1_app_input(app,GK1_INPUT_CONFIRM));OK(app->stock_trade_sell==1);
 OK(gk1_app_input(app,GK1_INPUT_CANCEL));
 OK(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 OK(memcmp(&initial,&app->stock_checkpoint,sizeof initial)==0);
 OK(app->stock_checkpoint.orders.remaining==3);
 Gk79Checkpoint *bad=malloc(sizeof *bad);OK(bad);
 *bad=initial;bad->stock_raw[0][0]^=1;
 const Gk61Quote sentinel={444u,555u,666u};quote=sentinel;
 OK(gk95_stock_trade_reference(bad,GK61_CHINA,0,&quote)==GK95_QUOTE_UNVERIFIED);
 OK(memcmp(&sentinel,&quote,sizeof quote)==0);
 *bad=initial;bad->world.countries[0].gold++;
 OK(gk95_stock_trade_reference(bad,GK61_CHINA,0,&quote)==GK95_QUOTE_UNVERIFIED);
 OK(memcmp(&sentinel,&quote,sizeof quote)==0);
 *bad=initial;bad->source_verified=0;
 OK(gk95_stock_trade_reference(bad,GK61_CHINA,0,&quote)==GK95_QUOTE_UNVERIFIED);
 *bad=initial;bad->rate=1000u;
 OK(gk95_stock_trade_reference(bad,GK61_CHINA,0,&quote)==GK95_QUOTE_INVALID);
 OK(memcmp(&sentinel,&quote,sizeof quote)==0);
 OK(gk95_stock_trade_reference(&initial,(Gk61Merchant)3,0,&quote)==GK95_QUOTE_INVALID);
 OK(gk95_stock_trade_reference(&initial,GK61_CHINA,17,&quote)==GK95_QUOTE_INVALID);
 OK(gk95_stock_trade_reference(NULL,GK61_CHINA,0,&quote)==GK95_QUOTE_INVALID);
 OK(gk95_stock_trade_reference(&initial,GK61_CHINA,0,NULL)==GK95_QUOTE_INVALID);
 free(bad);free(app);
 return 0;
}
int main(int argc,char **argv){
 OK(argc==2||argc==5);
 Gk61Quote out={11u,22u,33u};
 Gk79Checkpoint empty={0};
 OK(gk95_stock_trade_reference(&empty,GK61_CHINA,0,&out)==GK95_QUOTE_UNVERIFIED);
 OK(out.buy_tenths==11u&&out.sell_tenths==22u&&out.hire_tenths==33u);
 if(argc==5){
  Gk1TemplateDataset *all=malloc(sizeof *all);OK(all);
  Gk1LoadedScenario *world=malloc(sizeof *world);OK(world);
  OK(gk1_template_load(argv[1],all)==GK1_LOAD_OK);
  OK(gk1_open_loaded_scenario(all,GK1_MONGOL_CONQUEST,world)==GK1_LOAD_OK);
  OK(verified_case(world,argv[2],GK77_FRESH_MONGOL_WORKRAM,121)==0);
  OK(verified_case(world,argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,101)==0);
  OK(gk1_open_loaded_scenario(all,GK1_WORLD_CONQUEST,world)==GK1_LOAD_OK);
  OK(verified_case(world,argv[4],GK77_ARCHIVED_WORLD_SAVERAM,114)==0);
  free(world);free(all);
  puts("GK95 PRIVATE AUTHENTIC STOCK: 3 profiles x 51 quotes, controller trade viewer and negative rollback PASS");
 }else puts("GK95 PUBLIC: null/unverified source refuses without quote modification; authentic positive needs private original stock records");
 return 0;
}
