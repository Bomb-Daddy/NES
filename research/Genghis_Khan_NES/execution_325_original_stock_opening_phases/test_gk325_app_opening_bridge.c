#include "gk325_app_opening_bridge.h"
#include <stdio.h>
#include <string.h>
static int checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
static void init_app(Gk1App *app,unsigned w){
 memset(app,0,sizeof *app);app->active_world=(uint8_t)w;
 app->world_ready[w]=1;app->rules_profile=0;
 Gk1LoadedScenario *s=&app->worlds[w];
 s->phase=GK1_PRE_SETUP_TEMPLATE;s->scenario_id=(uint8_t)(w+1);
 s->country_count=(uint8_t)(w?27:14);
 for(unsigned i=0;i<s->country_count;i++){
  s->countries[i].index=(uint8_t)i;
  s->countries[i].country_id=(uint8_t)(i+1);
  s->countries[i].scenario_id=(uint8_t)(w+1);
 }
}
int main(void){
 Gk1App app;init_app(&app,0);Gk1App before=app;
 const Gk325Sample source={500,1175,439,208,61,101,3,0,1};
 Gk325Trace tr={GK325_MONGOL,GK325_ARCHIVED_RESTORED,GK325_SAVE_RAM,14,&source,1};
 Gk325Audit a={0};CHECK(gk325_app_audit(&app,&tr,&a)==GK325_OK);
 CHECK(a.observed_stock==1 && a.port_generated==0 && a.port_orders==0);
 CHECK(memcmp(&before,&app,sizeof app)==0);
 app.rules_profile=1;CHECK(gk325_app_audit(&app,&tr,&a)==GK325_WRONG_SOURCE);
 app.rules_profile=0;app.worlds[0].phase=0;
 CHECK(gk325_app_audit(&app,&tr,&a)==GK325_WRONG_SOURCE);
 app.worlds[0].phase=GK1_PRE_SETUP_TEMPLATE;
 app.worlds[0].countries[7].country_id=99;
 CHECK(gk325_app_audit(&app,&tr,&a)==GK325_WRONG_SOURCE);
 init_app(&app,1);
 tr.world=GK325_WORLD;tr.country_count=27;
 const Gk325Sample ww={700,1206,193,332,116,114,3,0,1};tr.samples=&ww;
 CHECK(gk325_app_audit(&app,&tr,&a)==GK325_OK);
 CHECK(a.source_year==1206 && a.port_orders==0);
 tr.world=GK325_MONGOL;
 CHECK(gk325_app_audit(&app,&tr,&a)==GK325_WRONG_SOURCE);
 CHECK(gk325_app_audit(0,&tr,&a)==GK325_BAD_INPUT);
 CHECK(gk325_app_audit(&app,0,&a)==GK325_BAD_INPUT);
 CHECK(gk325_app_audit(&app,&tr,0)==GK325_BAD_INPUT);
 printf("GK325 same inherited Gk1App bridge assertions PASS (%d); read-only PRE_SETUP\n",checks);
 return 0;
}
