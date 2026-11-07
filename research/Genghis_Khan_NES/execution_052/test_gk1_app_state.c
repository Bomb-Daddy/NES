#include "gk1_app_state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
static unsigned checks=0;
#define CK(x) do{++checks;if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
int main(void){
 Gk1App a;gk1_app_init(&a);
 CK(a.screen==GK1_SCREEN_TITLE);CK(a.menu_index==0);
 CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);CK(a.screen==GK1_SCREEN_CAMPAIGNS);
 CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);CK(a.screen==GK1_SCREEN_UNIMPLEMENTED);
 CK(strstr(a.status,"NOT IMPLEMENTED")!=NULL);
 CK(gk1_app_input(&a,GK1_INPUT_CANCEL)==1);CK(a.screen==GK1_SCREEN_CAMPAIGNS);
 CK(gk1_app_input(&a,GK1_INPUT_DOWN)==1);CK(a.menu_index==1);
 CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);CK(a.screen==GK1_SCREEN_UNIMPLEMENTED);
 CK(gk1_app_input(&a,GK1_INPUT_CANCEL)==1);CK(gk1_app_input(&a,GK1_INPUT_CANCEL)==1);
 CK(a.screen==GK1_SCREEN_TITLE);
 CK(gk1_app_input(&a,GK1_INPUT_DOWN)==1);CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);
 CK(a.screen==GK1_SCREEN_RESEARCH_BATTLE);CK(a.has_session==1);
 CK(a.session.battle.mover.soldiers==40);
 CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);CK(a.last_move==GK1_STEP_PENDING);
 CK(a.session.battle.mover.soldiers==34);CK(a.session.battle.mover.mobility==6);
 CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);CK(strstr(a.status,"UNSUPPORTED")!=NULL);
 CK(a.session.events==1);CK(gk1_app_input(&a,GK1_INPUT_CANCEL)==1);
 CK(a.last_move==GK1_FINISHED);CK(a.session.battle.completion_word==1);
 CK(gk1_app_input(&a,GK1_INPUT_CANCEL)==1);CK(a.screen==GK1_SCREEN_TITLE);
 CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);CK(a.screen==GK1_SCREEN_RESEARCH_BATTLE);
 for(unsigned i=1;i<6;i++){
  CK(gk1_app_input(&a,GK1_INPUT_RIGHT)==1);CK(a.case_index==i);
  CK(a.session.battle.mover.soldiers==40);
  CK(gk1_app_input(&a,GK1_INPUT_CONFIRM)==1);
  switch(i){
   case 1:CK(a.last_move==GK1_FINISHED);CK(a.session.battle.mover.mobility==10);CK(a.session.battle.completion_word==1);break;
   case 2:CK(a.last_move==GK1_REJECTED);CK(a.session.battle.mover.soldiers==40);break;
   case 3:CK(a.last_move==GK1_FINISHED);CK(a.session.battle.mover.soldiers==74);CK(a.session.battle.friendly_slot1.soldiers==0);CK(a.session.battle.completion_word==2);break;
   case 4:CK(a.last_move==GK1_REJECTED);CK(a.session.battle.enemy_slot10.soldiers==70);break;
   case 5:CK(a.last_move==GK1_FINISHED);CK(a.session.battle.mover.soldiers==32);break;
  }
 }
 CK(gk1_app_input(&a,GK1_INPUT_RIGHT)==1);CK(a.case_index==0);
 CK(gk1_app_case_label(99)[0]=='U');
 CK(gk1_app_input(NULL,GK1_INPUT_CONFIRM)==0);CK(gk1_app_input(&a,(Gk1Input)0)==0);
 Gk1App old=a;CK(gk1_app_input(&a,(Gk1Input)99)==0);CK(memcmp(&old,&a,sizeof a)==0);
 /* Ensure every state and menu can be escaped without a keyboard. */
 CK(gk1_app_input(&a,GK1_INPUT_CANCEL)==1);CK(a.screen==GK1_SCREEN_TITLE);
 printf("EXEC0041 app-state QA: %u/%u assertions PASS\n",checks,checks);return 0;
}
