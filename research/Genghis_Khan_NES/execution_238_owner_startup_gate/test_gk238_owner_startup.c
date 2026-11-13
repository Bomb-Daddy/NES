#include "gk238_owner_startup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"EXEC0238 failed line %d: %s\n",__LINE__,#x);return 1;}count++;}while(0)
int main(int argc,char **argv){
 if(argc!=2&&argc!=3)return 2;
 unsigned count=0;
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *app=malloc(sizeof *app),*before=malloc(sizeof *before);
 if(!dataset||!app||!before)return 3;
 gk1_app_init(app);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(app,dataset));
 *before=*app;
 CHECK(gk238_owner_startup(app,argv[1],NULL,1,0)==GK238_UNREQUESTED);
 CHECK(gk238_owner_startup(app,argv[1],"",1,0)==GK238_UNREQUESTED);
 CHECK(gk238_owner_startup(app,argv[1],"/nonexistent/private.json",1,1)==GK238_SMOKE_ISOLATED);
 CHECK(gk238_owner_startup(app,argv[1],"/nonexistent/private.json",0,0)==GK238_REFUSED);
 CHECK(gk238_owner_startup(app,argv[1],"/nonexistent/private.json",1,0)==GK238_REFUSED);
 CHECK(gk238_owner_startup(app,argv[1],argv[1],1,0)==GK238_REFUSED);
 CHECK(gk238_owner_startup(app,"/nonexistent/countries.csv","/nonexistent/private.json",1,0)==GK238_REFUSED);
 CHECK(gk238_owner_startup(app,NULL,"/nonexistent/private.json",1,0)==GK238_REFUSED);
 CHECK(gk238_owner_startup(NULL,argv[1],"/nonexistent/private.json",1,0)==GK238_REFUSED);
 CHECK(memcmp(app,before,sizeof *app)==0);
 CHECK(app->worlds[0].country_count==14&&app->worlds[1].country_count==27);
 CHECK(app->screen==GK1_SCREEN_TITLE);
 if(argc==3){
  CHECK(gk238_owner_startup(app,argv[1],argv[2],1,1)==GK238_SMOKE_ISOLATED);
  CHECK(memcmp(app,before,sizeof *app)==0);
  CHECK(gk238_owner_startup(app,argv[1],argv[2],1,0)==GK238_OWNER_IMPORTED);
  CHECK(app->screen==GK1_SCREEN_STOCK_CONTINUE);
  CHECK(app->rules_profile==0u);
  CHECK(app->stock_checkpoint.source_verified&&app->stock_checkpoint.valid);
  CHECK(app->stock_checkpoint.orders.remaining==3u);
  CHECK(app->stock_checkpoint.profile==GK77_FRESH_MONGOL_WORKRAM);
  CHECK(memcmp(app->worlds,before->worlds,sizeof app->worlds)==0);
  *before=*app;
  CHECK(gk238_owner_startup(app,argv[1],argv[2],1,0)==GK238_REFUSED);
  CHECK(memcmp(app,before,sizeof *app)==0);
 }
 printf("EXEC0238 %u assertions PASS; owner opt-in, fail closed, smoke isolation; no independent NES Spring or physical Xbox\n",count);
 free(dataset);free(app);free(before);return 0;
}
