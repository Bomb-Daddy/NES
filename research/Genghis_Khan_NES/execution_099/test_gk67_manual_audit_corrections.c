#include "gk1_app_state.h"
#include "gk64_manual_rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {count++;if(!(x)){fprintf(stderr,"FAIL %s:%d %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
int main(int argc,char**argv){
 if(argc!=2)return 4;
 unsigned count=0;
 Gk1TemplateDataset *source=malloc(sizeof *source);
 Gk1App *app=malloc(sizeof *app);
 CHECK(source!=NULL&&app!=NULL);
 CHECK(gk1_template_load(argv[1],source)==GK1_LOAD_OK);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,source));
 Gk1LoadedScenario baseline[2];
 memcpy(baseline,app->worlds,sizeof baseline);
 Gk64SeasonGeography g={0},unchanged={191,192};
 unsigned cold_counts[2]={0},storm_counts[2]={0};
 for(unsigned w=0;w<2;w++){
  Gk1LoadedScenario *world=&app->worlds[w];
  CHECK(world->country_count==(w==0?14u:27u));
  for(unsigned i=0;i<world->country_count;i++){
   unsigned id=i+1;
   CHECK(gk64_season_geography(world,(uint8_t)i,&g)==GK64_OK);
   unsigned cold=!w||(id<=7||(id>=15&&id<=19));
   unsigned storm=w&&(id==2||id==18||id==19||(id>=23&&id<=27));
   CHECK(g.winter_severe_cold_eligible==cold);
   CHECK(g.summer_storm_eligible==storm);
   cold_counts[w]+=g.winter_severe_cold_eligible;
   storm_counts[w]+=g.summer_storm_eligible;
  }
  CHECK(gk64_season_geography(world,world->country_count,&unchanged)==GK64_INVALID);
  CHECK(unchanged.winter_severe_cold_eligible==191&&unchanged.summer_storm_eligible==192);
  uint8_t original_id=world->countries[0].country_id;
  world->countries[0].country_id=0;
  CHECK(gk64_season_geography(world,0,&unchanged)==GK64_INVALID);
  world->countries[0].country_id=original_id;
  CHECK(unchanged.winter_severe_cold_eligible==191&&unchanged.summer_storm_eligible==192);
 }
 CHECK(cold_counts[0]==14&&storm_counts[0]==0);
 CHECK(cold_counts[1]==12&&storm_counts[1]==8);
 CHECK(gk64_season_geography(NULL,0,&unchanged)==GK64_INVALID);
 CHECK(gk64_season_geography(&app->worlds[0],0,NULL)==GK64_INVALID);
 uint8_t old=249,ans=249;
 const uint8_t costs[]={3,4,5,6,6,4,5};
 for(unsigned i=0;i<7;i++){
  CHECK(gk64_terrain_mobility((Gk64Terrain)i,&ans)==GK64_OK);
  CHECK(ans==costs[i]);
 }
 ans=old;
 CHECK(gk64_terrain_mobility((Gk64Terrain)7,&ans)==GK64_INVALID);
 CHECK(ans==old);
 CHECK(gk64_terrain_mobility(GK64_WATER,NULL)==GK64_INVALID);
 for(unsigned unit=0;unit<3;unit++){
  unsigned threshold=unit==0?13u:11u;
  for(unsigned mobility=0;mobility<=15;mobility++){
   CHECK(gk64_divide_manual((uint8_t)unit,(uint8_t)mobility,&ans)==GK64_OK);
   CHECK(ans==(mobility>=threshold));
  }
 }
 for(unsigned profile=0;profile<2;profile++){
  for(unsigned mobility=0;mobility<=15;mobility++){
   CHECK(gk64_divide_cavalry((uint8_t)profile,(uint8_t)mobility,&ans)==GK64_OK);
   CHECK(ans==(mobility>=13));
  }
 }
 ans=old;
 CHECK(gk64_divide_manual(3,13,&ans)==GK64_INVALID&&ans==old);
 CHECK(gk64_divide_manual(0,16,&ans)==GK64_INVALID&&ans==old);
 CHECK(gk64_divide_manual(0,13,NULL)==GK64_INVALID);
 CHECK(gk64_divide_cavalry(2,13,&ans)==GK64_INVALID&&ans==old);
 CHECK(gk64_divide_cavalry(0,16,&ans)==GK64_INVALID&&ans==old);
 CHECK(memcmp(baseline,app->worlds,sizeof baseline)==0);
 /* Actual app route: controller-only source-backed roster, no phase change. */
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 CHECK(app->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 CHECK(gk64_season_geography(&app->worlds[app->active_world],app->country_cursor[0],&g)==GK64_OK);
 CHECK(g.winter_severe_cold_eligible==1&&g.summer_storm_eligible==0);
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL));
 CHECK(gk1_app_input(app,GK1_INPUT_DOWN));
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 CHECK(gk1_app_input(app,GK1_INPUT_DOWN)); /* World country 2 */
 CHECK(gk64_season_geography(&app->worlds[1],app->country_cursor[1],&g)==GK64_OK);
 CHECK(g.winter_severe_cold_eligible==1&&g.summer_storm_eligible==1);
 CHECK(memcmp(baseline,app->worlds,sizeof baseline)==0);
 printf("EXEC0067 TESTED_PORT_LINUX_C11 PASS %u assertions; cold Mongol14 World12 storm Mongol0 World8; ocean6 and Divide cavalry13 foot11; immutable original 41; NO RNG/SPRING/NES observed\n",count);
 free(app);free(source);return 0;
}
