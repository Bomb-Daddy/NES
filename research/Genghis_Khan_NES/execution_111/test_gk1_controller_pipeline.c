/* A controller-only integration walk from title to completed research move.
 * Test samples are software-injected; physical Xbox acceptance NOT CLAIMED. */
#include "gk1_pad_router.h"
#include "gk1_app_state.h"
#include <stdlib.h>
#include <stdio.h>
#define T(x) do{checks++;if(!(x)){fprintf(stderr,"PIPELINE FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static int checks=0;
static Gk1PadRouter pad;
static Gk1App app;
static void pump(uint16_t bits,uint32_t dt){
 Gk1PadSample sample={0};sample.buttons=bits;
 Gk1PadAction out[8];size_t n=gk1_pad_step(&pad,&sample,dt,out,8);
 for(size_t i=0;i<n;i++){
  Gk1Input action;
  switch(out[i]){
   case GK1_PAD_ACT_UP:action=GK1_INPUT_UP;break;
   case GK1_PAD_ACT_DOWN:action=GK1_INPUT_DOWN;break;
   case GK1_PAD_ACT_LEFT:action=GK1_INPUT_LEFT;break;
   case GK1_PAD_ACT_RIGHT:action=GK1_INPUT_RIGHT;break;
   case GK1_PAD_ACT_A:action=GK1_INPUT_CONFIRM;break;
   case GK1_PAD_ACT_B:action=GK1_INPUT_CANCEL;break;
   default:continue;
  }
  T(gk1_app_input(&app,action));
 }
}
static void tap(uint16_t bits){pump(bits,16);pump(0,16);}
int main(void){
 gk1_app_init(&app);gk1_pad_init(&pad);
 pump(0,16);T(app.screen==GK1_SCREEN_TITLE);
 /* DPad down -> research; press A once, held A must not act again */
 tap(GK1_PAD_DOWN);T(app.menu_index==1);
 pump(GK1_PAD_A,16);T(app.screen==GK1_SCREEN_RESEARCH_BATTLE);T(app.case_index==0);
 for(int i=0;i<100;i++)pump(GK1_PAD_A,16);
 T(app.session.events==0); /* hold A from menu must not fire battle */
 pump(0,16);tap(GK1_PAD_A);
 T(app.session.events==1);T(app.last_move==GK1_STEP_PENDING);
 T(app.session.battle.mover.soldiers==34);
 tap(GK1_PAD_B);
 T(app.session.events==2);T(app.last_move==GK1_FINISHED);
 T(app.session.battle.completion_word==1);
 /* Navigate into low-Mobility fixture, reject move without changing units. */
 tap(GK1_PAD_RIGHT);tap(GK1_PAD_RIGHT);T(app.case_index==2);
 tap(GK1_PAD_A);T(app.last_move==GK1_REJECTED);T(app.session.events==1);
 T(app.session.battle.mover.soldiers==40);
 tap(GK1_PAD_B);T(app.screen==GK1_SCREEN_TITLE);
 /* A held during disconnect/reconnect MUST NOT activate title menu. */
 gk1_pad_disconnect(&pad);pump(GK1_PAD_A,16);T(app.screen==GK1_SCREEN_TITLE);
 pump(GK1_PAD_A,16);T(app.screen==GK1_SCREEN_TITLE);
 pump(0,16);tap(GK1_PAD_A);T(app.screen==GK1_SCREEN_RESEARCH_BATTLE);
 printf("EXEC0043 controller-to-native-app pipeline PASS: %d assertions\n",checks);
 return 0;
}
