#include "gk331_app_offer_bridge.h"
#include "gk162_original_entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{checks++;if(!(c)){fprintf(stderr,"APP BRIDGE line %d: %s\n",__LINE__,#c);return 2;}}while(0)
static unsigned checks=0;
int main(int argc,char **argv){
 if(argc!=4)return 2;
 Gk1TemplateDataset *data=calloc(1,sizeof *data);
 Gk1App *app=calloc(1,sizeof *app);
 Gk1App *saved=malloc(sizeof *saved);
 CHECK(data&&app&&saved);
 CHECK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,data));
 uint8_t seed[6]={1,2,3,4,5,6};Gk331Offers untouched;memset(&untouched,0x5a,sizeof untouched);
 Gk331Offers old=untouched;
 CHECK(gk331_app_stock_trade_offers(app,argv[3],seed,&untouched)==GK331_STAGE);
 CHECK(memcmp(&untouched,&old,sizeof old)==0);
 app->rules_profile=0;app->screen=GK1_SCREEN_SPRING_ARCHIVE;
 CHECK(gk162_enter_original_checkpoint(app,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM,0)==GK162_ENTRY_OK);
 CHECK(gk85_raw_source_intact(&app->stock_checkpoint));
 *saved=*app;
 CHECK(gk331_app_stock_trade_offers(app,argv[3],seed,&untouched)==GK331_OK);
 CHECK(untouched.draws==84u&&!untouched.merchant_presence_known);
 CHECK(memcmp(app,saved,sizeof *saved)==0);
 Gk331Offers copy=untouched;
 app->stock_checkpoint.stock_raw[0][0]^=1;
 CHECK(gk331_app_stock_trade_offers(app,argv[3],seed,&untouched)==GK331_STAGE);
 CHECK(memcmp(&untouched,&copy,sizeof copy)==0);
 *app=*saved;app->stock_checkpoint.orders.playable=0;
 CHECK(gk331_app_stock_trade_offers(app,argv[3],seed,&untouched)==GK331_STAGE);
 CHECK(memcmp(&untouched,&copy,sizeof copy)==0);
 *app=*saved;app->rules_profile=4;
 CHECK(gk331_app_stock_trade_offers(app,argv[3],seed,&untouched)==GK331_STAGE);
 CHECK(memcmp(&untouched,&copy,sizeof copy)==0);
 printf("GK331_NATIVE_APP_AUTHENTIC_ORIGINAL_FRESH_CHECKPOINT_PASS checks=%u source_intact=1 no_orders_spent=1\n",checks);
 free(saved);free(app);free(data);return 0;
}
