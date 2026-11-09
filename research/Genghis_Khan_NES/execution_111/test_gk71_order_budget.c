/* MANUAL_DERIVED strategic order accounting only; synthetic phase-2 values
 * are named TEST_FIXTURE, NOT source-observed NES or player-ready campaigns. */
#include "gk71_order_budget.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int n=0;
#define CHECK(x) do{n++;if(!(x)){fprintf(stderr,"FAIL line %u %s\n",(unsigned)__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 4;
 Gk1TemplateDataset *raw=malloc(sizeof *raw);
 Gk1App *a=malloc(sizeof *a);
 if(!raw||!a)return 5;
 CHECK(gk1_template_load(argv[1],raw)==GK1_LOAD_OK);
 gk1_app_init(a);CHECK(gk1_app_install_templates(a,raw));
 Gk1LoadedScenario before[2];memcpy(before,a->worlds,sizeof before);
 Gk71Budget b={0};
 for(unsigned world=0;world<2;world++){
  for(unsigned ix=0;ix<a->worlds[world].country_count;ix++){
   for(unsigned governor=0;governor<3;governor++){
    for(unsigned season=0;season<4;season++){
     memset(&b,0xa5,sizeof b);
     CHECK(gk71_open(&a->worlds[world],(uint8_t)ix,(Gk71Season)season,
                           (Gk71Governor)governor,&b)==GK71_LOCKED);
     CHECK(b.scenario==world+1u&&b.country_id==ix+1u&&b.season==season);
     CHECK(b.playable==0&&b.governor==governor);
     CHECK(b.allowance==(governor==0?3u:governor==1?1u:0u));
     CHECK(b.remaining==b.allowance);
     Gk71Budget old=b;
     CHECK(gk71_commit_completed(&b,1)==GK71_LOCKED);
     CHECK(memcmp(&old,&b,sizeof b)==0);
     CHECK(gk71_pass(&b)==GK71_LOCKED);
     CHECK(memcmp(&old,&b,sizeof b)==0);
     CHECK(gk71_next_season(&a->worlds[world],&b,(Gk71Season)((season+1)%4))==GK71_LOCKED);
     CHECK(memcmp(&old,&b,sizeof b)==0);
    }
   }
  }
 }
 CHECK(memcmp(before,a->worlds,sizeof before)==0);
 /* Hostile tests: validation failures leave caller outputs byte-identical. */
 Gk71Budget poison;memset(&poison,0x5a,sizeof poison);Gk71Budget old=poison;
 CHECK(gk71_open(&a->worlds[0],14,GK71_SPRING,GK71_BASE,&poison)==GK71_INVALID);
 CHECK(memcmp(&poison,&old,sizeof old)==0);
 CHECK(gk71_open(&a->worlds[0],0,(Gk71Season)4,GK71_BASE,&poison)==GK71_INVALID);
 CHECK(memcmp(&poison,&old,sizeof old)==0);
 CHECK(gk71_open(&a->worlds[0],0,GK71_SPRING,(Gk71Governor)3,&poison)==GK71_INVALID);
 CHECK(memcmp(&poison,&old,sizeof old)==0);
 Gk1LoadedScenario malformed=a->worlds[0];malformed.countries[0].country_id=2;
 CHECK(gk71_open(&malformed,0,GK71_SPRING,GK71_BASE,&poison)==GK71_INVALID);
 CHECK(memcmp(&poison,&old,sizeof old)==0);
 CHECK(gk71_open(0,0,GK71_SPRING,GK71_BASE,&poison)==GK71_INVALID);
 CHECK(memcmp(&poison,&old,sizeof old)==0);
 /* TEST_FIXTURE only: explicitly constructed not-real source-verified phase2.
  * These checks test reducer logic, NOT actual original first Spring. */
 Gk1LoadedScenario fake=a->worlds[0];
 fake.phase=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE;
 for(unsigned kind=0;kind<3;kind++){
  CHECK(gk71_open(&fake,0,GK71_SPRING,(Gk71Governor)kind,&b)==GK71_OK);
  old=b;CHECK(gk71_commit_completed(&b,0)==GK71_LOCKED);
  CHECK(memcmp(&b,&old,sizeof b)==0);
  if(kind==GK71_PRINCE){CHECK(gk71_pass(&b)==GK71_PRINCE_AI);continue;}
  CHECK(gk71_commit_completed(&b,1)==GK71_OK);
  CHECK(b.remaining==b.allowance-1&&b.completed==1);
  if(kind==GK71_DIRECT){CHECK(gk71_pass(&b)==GK71_EXHAUSTED);}
  else {CHECK(gk71_pass(&b)==GK71_OK);CHECK(b.remaining==0&&b.completed==1);}
  old=b;CHECK(gk71_commit_completed(&b,1)==GK71_EXHAUSTED);
  CHECK(memcmp(&b,&old,sizeof b)==0);
  CHECK(gk71_next_season(&fake,&b,GK71_WINTER)==GK71_INVALID);
  CHECK(memcmp(&b,&old,sizeof b)==0);
  CHECK(gk71_next_season(&fake,&b,GK71_SUMMER)==GK71_OK);
  CHECK(b.season==GK71_SUMMER&&b.remaining==b.allowance&&b.completed==0);
 }
 CHECK(memcmp(before,a->worlds,sizeof before)==0);
 printf("EXEC0071 Gk71 order-budget %d direct checks PASS; 41 PRE_SETUP countries immutable; synthetic future-phase only\n",n);
 free(raw);free(a);return 0;
}
