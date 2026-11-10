#include "gk166_fresh_entry.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"EXEC0166 failed line %d: %s\n",__LINE__,#c);return 1;}checks++;}while(0)
static unsigned checks;
int main(int argc,char **argv){
 if(argc!=2&&argc!=3)return 3;
 Gk1App *app=malloc(sizeof *app),*baseline=malloc(sizeof *baseline);
 Gk1TemplateDataset *d=malloc(sizeof *d);
 if(!app||!baseline||!d)return 2;
 gk1_app_init(app);
 CHECK(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(app,d));
 *baseline=*app;
 CHECK(gk166_enter_original_fresh_json(NULL,argv[1],"x",0)==GK166_ARGUMENT);
 CHECK(gk166_enter_original_fresh_json(app,argv[1],"x",1)==GK166_ARGUMENT);
 CHECK(gk166_enter_original_fresh_json(app,"/bad", "x",0)==GK166_TEMPLATE);
 CHECK(gk166_enter_original_fresh_json(app,argv[1],"/missing/private-source",0)==GK166_SOURCE_UNAVAILABLE);
 CHECK(memcmp(app,baseline,sizeof *app)==0);
 CHECK(app->stock_checkpoint.valid==0);
 if(argc==3){
  CHECK(gk166_enter_original_fresh_json(app,argv[1],argv[2],0)==GK166_OK);
  CHECK(app->screen==GK1_SCREEN_STOCK_CONTINUE);
  CHECK(app->stock_checkpoint.valid&&app->stock_checkpoint.source_verified);
  CHECK(app->stock_checkpoint.profile==GK77_FRESH_MONGOL_WORKRAM);
  CHECK(app->stock_checkpoint.orders.remaining==3);
  CHECK(app->stock_checkpoint.world.country_count==14);
  CHECK(app->stock_checkpoint.world.countries[0].gold==430);
  CHECK(app->stock_checkpoint.world.countries[0].goods[10]==208);
  CHECK(gk85_raw_source_intact(&app->stock_checkpoint));
  CHECK(gk88_stock_projection_intact(&app->stock_checkpoint));
  CHECK(memcmp(app->worlds,baseline->worlds,sizeof app->worlds)==0);
  Gk1App *snapshot=malloc(sizeof *snapshot);if(!snapshot)return 2;
  *snapshot=*app;
  CHECK(gk166_enter_original_fresh_json(app,argv[1],"/missing/private-source",0)!=GK166_OK);
  CHECK(memcmp(app,snapshot,sizeof *app)==0);
  CHECK(gk166_enter_original_fresh_json(app,"/bad",argv[2],0)==GK166_TEMPLATE);
  CHECK(memcmp(app,snapshot,sizeof *app)==0);
  free(snapshot);
 }
 printf("EXEC0166 %u assertions PASS: private original fresh direct-memory %s, checkpoint only (VIEW/PASS)\n",checks,argc==3?"TESTED":"NOT RUN");
 free(d);free(baseline);free(app);return 0;
}
