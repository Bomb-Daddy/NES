/* EXEC0066. Native controller interface, not authentic NES battle observations. */
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define T(c) do{checks++;if(!(c)){fprintf(stderr,"FAILED %s:%d %s\n",__FILE__,__LINE__,#c);return 1;}}while(0)
static int start(Gk1App *a,unsigned w){
 if(!gk1_app_input(a,GK1_INPUT_CONFIRM))return 0;
 if(w&&!gk1_app_input(a,GK1_INPUT_DOWN))return 0;
 if(!gk1_app_input(a,GK1_INPUT_CONFIRM))return 0;
 if(!gk1_app_input(a,GK1_INPUT_CONFIRM))return 0;
 if(!gk1_app_input(a,GK1_INPUT_SETUP))return 0;
 for(unsigned i=0;i<4;i++)if(!gk1_app_input(a,GK1_INPUT_CONFIRM))return 0;
 if(a->screen!=GK1_SCREEN_SETUP_WIZARD||a->setup_stage[w]!=GK53_SETUP_AWAIT_SOURCE)return 0;
 if(!gk1_app_input(a,GK1_INPUT_LABOR_PLAN))return 0;
 return gk1_app_input(a,GK1_INPUT_SETUP)&&a->screen==GK1_SCREEN_BATTLE_TIMELINE;
}
int main(int argc,char **argv){
 if(argc!=2)return 4;
 int checks=0;
 Gk1TemplateDataset *d=malloc(sizeof *d),*copy=malloc(sizeof *d);
 Gk1App *a=malloc(sizeof *a);T(d&&copy&&a);
 T(gk1_template_load(argv[1],d)==GK1_LOAD_OK);memcpy(copy,d,sizeof *d);
 for(unsigned w=0;w<2;w++){
  gk1_app_init(a);T(gk1_app_install_templates(a,d));
  /* World=1 begins with a valid default lord candidate 0. */
  Gk1LoadedScenario before0=a->worlds[0],before1=a->worlds[1];
  T(start(a,w)); T(a->battle_timeline.max_days==90);T(a->whatif_event==GK65_ACT_COMPLETE);
  T(a->whatif_victim==GK65_DEFENDER_VICTIM);
  T(gk1_app_input(a,GK1_INPUT_RIGHT));T(a->whatif_victim==GK65_DEFENDER_VICTIM);
  T(gk1_app_input(a,GK1_INPUT_UP));T(a->whatif_event==GK65_BOTH_COMMANDERS_DESTROYED);
  T(gk1_app_input(a,GK1_INPUT_CONFIRM));
  T(a->battle_timeline.outcome==GK65_DEFENDER_RETAINS_DOUBLE_KO && a->battle_timeline.attacker_captured);
  Gk65Terminal done=a->battle_timeline;uint8_t chosen=a->whatif_event;
  T(gk1_app_input(a,GK1_INPUT_CONFIRM));T(memcmp(&done,&a->battle_timeline,sizeof done)==0);
  T(gk1_app_input(a,GK1_INPUT_UP));T(a->whatif_event==chosen);
  T(gk1_app_input(a,GK1_INPUT_LEFT));T(a->whatif_victim==GK65_DEFENDER_VICTIM);
  T(gk1_app_input(a,GK1_INPUT_CANCEL));T(gk1_app_input(a,GK1_INPUT_SETUP));
  T(a->battle_timeline.outcome==GK65_UNDECIDED);T(a->whatif_event==GK65_ACT_COMPLETE);
  T(gk1_app_input(a,GK1_INPUT_DOWN));T(a->whatif_event==GK65_COMMAND_DESTROYED);
  T(gk1_app_input(a,GK1_INPUT_LEFT));T(a->whatif_victim==GK65_ATTACKER_VICTIM);
  T(gk1_app_input(a,GK1_INPUT_CONFIRM));T(a->battle_timeline.outcome==GK65_DEFENDER_WON);
  T(gk1_app_input(a,GK1_INPUT_CANCEL));T(gk1_app_input(a,GK1_INPUT_SETUP));
  T(gk1_app_input(a,GK1_INPUT_DOWN));T(gk1_app_input(a,GK1_INPUT_CONFIRM));
  T(a->battle_timeline.outcome==GK65_ATTACKER_WON);
  T(memcmp(&before0,&a->worlds[0],sizeof before0)==0);
  T(memcmp(&before1,&a->worlds[1],sizeof before1)==0);
  T(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 }
 T(memcmp(copy,d,sizeof *d)==0);
 printf("EXEC0066 controller WHAT-IF %d checks PASS; both originals immutable PRE_SETUP; no NES RNG claimed\n",checks);
 free(d);free(copy);free(a);return 0;
}
