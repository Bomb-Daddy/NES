#include "gk1_app_state.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(e) do{count++;if(!(e)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#e);return 1;}}while(0)
static int setup(Gk1App *a,unsigned w){
 if(!gk1_app_input(a,GK1_INPUT_CONFIRM))return 0;
 if(w&&!gk1_app_input(a,GK1_INPUT_DOWN))return 0;
 if(!gk1_app_input(a,GK1_INPUT_CONFIRM)||!gk1_app_input(a,GK1_INPUT_CONFIRM)||
    !gk1_app_input(a,GK1_INPUT_SETUP))return 0;
 for(unsigned i=0;i<4;i++)if(!gk1_app_input(a,GK1_INPUT_CONFIRM))return 0;
 return gk1_app_input(a,GK1_INPUT_LABOR_PLAN)&&
        gk1_app_input(a,GK1_INPUT_SETUP)&&a->screen==GK1_SCREEN_BATTLE_TIMELINE;
}
int main(int argc,char **argv){
 if(argc!=2)return 4;
 unsigned count=0;
 Gk72BattlePass state={0},old={0};
 CHECK(gk72_pass_begin(&state,15,13)==GK72_OK);
 CHECK(gk72_pass_apply(&state)==GK72_OK&&state.mobility[0]==15&&state.used[0]==1);
 old=state;CHECK(gk72_pass_apply(&state)==GK72_REJECTED&&!memcmp(&old,&state,sizeof old));
 CHECK(gk72_pass_select(&state,1)==GK72_OK);
 CHECK(gk72_pass_apply(&state)==GK72_OK&&state.mobility[1]==13);
 old=state;CHECK(gk72_pass_begin(&state,16,2)==GK72_REJECTED&&!memcmp(&old,&state,sizeof old));
 CHECK(gk72_pass_begin(&state,2,14)==GK72_REJECTED&&!memcmp(&old,&state,sizeof old));
 CHECK(gk72_pass_select(&state,2)==GK72_REJECTED&&!memcmp(&old,&state,sizeof old));
 CHECK(gk72_pass_new_activation(&state)==GK72_OK);
 CHECK(state.used[0]==0&&state.used[1]==0&&state.mobility[0]==15&&state.mobility[1]==13);
 for(unsigned w=0;w<2;w++){
  Gk1TemplateDataset *d=malloc(sizeof *d);
  Gk1App *a=malloc(sizeof *a);
  CHECK(d&&a&&gk1_template_load(argv[1],d)==GK1_LOAD_OK);
  gk1_app_init(a);CHECK(gk1_app_install_templates(a,d));
  Gk1LoadedScenario before[2]={a->worlds[0],a->worlds[1]};
  CHECK(setup(a,w));CHECK(a->battle_timeline.outcome==GK65_UNDECIDED);
  CHECK(gk1_app_input(a,GK1_INPUT_SETUP)&&a->screen==GK1_SCREEN_BATTLE_PASS_DRILL);
  CHECK(a->battle_pass.mobility[0]==12&&a->battle_pass.mobility[1]==10);
  CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM));CHECK(a->battle_pass.mobility[0]==13);
  Gk72BattlePass accepted=a->battle_pass;
  CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM));CHECK(!memcmp(&accepted,&a->battle_pass,sizeof accepted));
  CHECK(gk1_app_input(a,GK1_INPUT_RIGHT));CHECK(a->battle_pass.selected==1);
  CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM));CHECK(a->battle_pass.mobility[1]==11);
  CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)&&a->screen==GK1_SCREEN_BATTLE_TIMELINE);
  CHECK(gk1_app_input(a,GK1_INPUT_SETUP)&&a->screen==GK1_SCREEN_BATTLE_PASS_DRILL);
  CHECK(a->battle_pass.mobility[0]==13&&a->battle_pass.used[0]==1);
  CHECK(gk1_app_input(a,GK1_INPUT_SETUP));
  CHECK(gk1_app_input(a,GK1_INPUT_LEFT));CHECK(a->battle_pass.selected==0);
  CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)&&a->battle_pass.mobility[0]==14);
  CHECK(gk1_app_input(a,GK1_INPUT_SETUP));
  CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)&&a->battle_pass.mobility[0]==15);
  CHECK(gk1_app_input(a,GK1_INPUT_SETUP));
  CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)&&a->battle_pass.mobility[0]==15);
  CHECK(gk1_app_input(a,GK1_INPUT_CANCEL));
  CHECK(a->battle_timeline.outcome==GK65_UNDECIDED&&a->battle_timeline.days_complete==0);
  CHECK(!memcmp(before,&a->worlds,sizeof before));
  CHECK(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
  free(d);free(a);
 }
 /* Explicit invalid/corrupt state negative control must not write memory. */
 memset(&state,0,sizeof state);old=state;
 CHECK(gk72_pass_apply(&state)==GK72_REJECTED&&!memcmp(&state,&old,sizeof state));
 printf("EXEC0072 %u direct checks PASS, both 14/27 country worlds immutable; manual drill only\n",count);
 return 0;
}
