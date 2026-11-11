#include "gk196_classic_trade_status.h"
#include "gk134_stock_command_route.h"
#include "gk193_auto_source_entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static unsigned checks=0;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"EXEC0196 FAIL line %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static int authentic(const char *csv,const char *src,Gk77Profile expected){
 Gk1App *a=calloc(1,sizeof *a);CHECK(a);
 gk1_app_init(a);a->rules_profile=0;
 Gk1TemplateDataset *d=malloc(sizeof *d);CHECK(d);
 CHECK(gk1_template_load(csv,d)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(a,d));free(d);
 Gk77Profile profile=GK77_ARCHIVED_WORLD_SAVERAM;
 CHECK(gk193_enter_original_auto(a,csv,src,0,&profile)==GK193_OK);
 CHECK(profile==expected);
 Gk196TradeStatus v={0};
 CHECK(gk196_classic_trade_status(a,&v)==GK196_OK);
 CHECK(v.remaining==3u&&v.actions==0u&&v.derived==0u&&v.passed==0u);
 CHECK(v.rate==a->stock_checkpoint.rate&&v.year==a->stock_checkpoint.year);
 CHECK(gk1_app_input(a,GK1_INPUT_SETUP)==1);
 CHECK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->screen==GK1_SCREEN_CLASSIC_ASSIGN);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(gk196_classic_trade_status(a,&v)==GK196_OK);
 CHECK(v.remaining==2u&&v.actions==1u&&v.derived==1u&&v.passed==0u);
 CHECK(a->stock_checkpoint.orders.remaining==3u);
 CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 a->stock_command_cursor=7u; /* Trader menu: exact router mapping checked by result */
 /* Resolve the original 14-command Trade route without inferring stock offers. */
 unsigned trade_index=255u;
 for(unsigned i=0;i<14u;i++)if(gk134_command_route(i)==GK134_ROUTE_TRADE_REFERENCE){trade_index=i;break;}
 CHECK(trade_index<14u);a->stock_command_cursor=(uint8_t)trade_index;
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_TRADE_REFERENCE);
 CHECK(gk1_app_input(a,GK1_INPUT_RIGHT)==1);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(gk196_classic_trade_status(a,&v)==GK196_OK);
 CHECK(v.remaining==2u&&v.actions==1u&&a->stock_checkpoint.orders.remaining==3u);
 CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 /* Deliberate mutable-world corruption. UI entry and navigation fail closed. */
 Gk1App *bad=malloc(sizeof *bad);CHECK(bad);*bad=*a;
 bad->classic_assign.mutable_world.countries[bad->classic_assign.player_index].gold++;
 Gk196TradeStatus before=v;
 CHECK(gk196_classic_trade_status(bad,&v)==GK196_SOURCE);
 CHECK(!memcmp(&before,&v,sizeof v));
 bad->stock_command_cursor=(uint8_t)trade_index;
 CHECK(gk1_app_input(bad,GK1_INPUT_CONFIRM)==1);
 CHECK(bad->screen==GK1_SCREEN_STOCK_COMMANDS);
 bad->screen=GK1_SCREEN_STOCK_TRADE_REFERENCE;
 Gk1App snapshot=*bad;
 CHECK(gk1_app_input(bad,GK1_INPUT_RIGHT)==0);
 CHECK(!memcmp(&snapshot,bad,sizeof snapshot));
 *bad=*a;bad->stock_checkpoint.stock_raw[0][0]^=1u;
 CHECK(gk196_classic_trade_status(bad,&v)==GK196_SOURCE);
 *bad=*a;bad->rules_profile=1;
 CHECK(gk196_classic_trade_status(bad,&v)==GK196_SOURCE);
 free(bad);free(a);return 0;
}
int main(int argc,char **argv){
 CHECK(gk196_classic_trade_status(NULL,NULL)==GK196_INVALID);
 Gk1App *a=calloc(1,sizeof *a);CHECK(a);
 Gk196TradeStatus value={0};
 CHECK(gk196_classic_trade_status(a,&value)==GK196_SOURCE);
 CHECK(gk196_classic_trade_status(a,NULL)==GK196_INVALID);
 free(a);
 if(argc==5){
  CHECK(authentic(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM)==0);
  CHECK(authentic(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM)==0);
  CHECK(authentic(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM)==0);
 }
 printf("EXEC0196 %u assertions PASS; Classic live Trade quote budget source-attested, no merchant or RNG parity\n",checks);
 return 0;
}
