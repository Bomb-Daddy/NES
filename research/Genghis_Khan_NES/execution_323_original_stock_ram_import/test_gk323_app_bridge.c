#include "gk323_app_stock_bridge.h"
#include <stdio.h>
#include <string.h>
static int n;
#define CHECK(x) do{++n;if(!(x)){fprintf(stderr,"FAILED line %d %s\n",__LINE__,#x);return 1;}}while(0)
static void setup(Gk1App *a,unsigned w){
 memset(a,0,sizeof *a);a->rules_profile=0;a->active_world=(uint8_t)w;
 a->world_ready[w]=1;
 Gk1LoadedScenario *s=&a->worlds[w];
 unsigned count=w?27:14;
 s->phase=GK1_PRE_SETUP_TEMPLATE;
 s->scenario_id=w?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST;
 s->country_count=(uint8_t)count;
 for(unsigned i=0;i<count;i++){
  s->countries[i].scenario_id=(uint8_t)(w+1);
  s->countries[i].country_id=(uint8_t)(i+1);
  s->countries[i].index=(uint8_t)i;
 }
}
int main(void){
 Gk1App app;Gk323Spring result={0};uint8_t raw[27*83]={0};
 setup(&app,0);
 CHECK(gk323_app_observe(&app,GK323_ROM_SHA256,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&result)==GK323_OK);
 CHECK(result.country_count==14&&!result.port_orders_granted);
 CHECK(app.worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 Gk323Spring previous=result;
 #define REFUSE(expr) do{CHECK((expr)!=GK323_OK);CHECK(memcmp(&result,&previous,sizeof result)==0);}while(0)
 app.rules_profile=1;
 REFUSE(gk323_app_observe(&app,GK323_ROM_SHA256,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&result));
 app.rules_profile=0;app.worlds[0].countries[1].country_id=44;
 REFUSE(gk323_app_observe(&app,GK323_ROM_SHA256,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&result));
 setup(&app,1);
 REFUSE(gk323_app_observe(&app,GK323_ROM_SHA256,GK323_FRESH_RESET,GK323_WORK_RAM,1206,0,3,raw,27*83,&result));
 CHECK(gk323_app_observe(&app,GK323_ROM_SHA256,GK323_ARCHIVED_RESTORED,GK323_SAVE_RAM,1206,0,3,raw,27*83,&result)==GK323_OK);
 CHECK(result.country_count==27&&!result.generated_by_port);
 previous=result;app.worlds[1].phase=0;
 REFUSE(gk323_app_observe(&app,GK323_ROM_SHA256,GK323_ARCHIVED_RESTORED,GK323_SAVE_RAM,1206,0,3,raw,27*83,&result));
 setup(&app,1);app.world_ready[1]=0;
 REFUSE(gk323_app_observe(&app,GK323_ROM_SHA256,GK323_ARCHIVED_RESTORED,GK323_SAVE_RAM,1206,0,3,raw,27*83,&result));
 printf("GK323 APP BRIDGE PASS %d assertions; Classic-only Gk1App active-world source gate, no mutation\n",n);
 return 0;
}
