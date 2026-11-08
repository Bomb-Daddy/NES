#include "gk1_app_state.h"
#include "gk65_battle_terminal.h"
#include "gk64_manual_rules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{ checks++;if(!(c)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv){
 if(argc!=2)return 4;
 unsigned checks=0;
 Gk65Terminal s={0},before={0};
 for(unsigned orders=1;orders<=3;orders++){
  CHECK(gk65_begin((uint8_t)orders,&s)==GK65_OK);
  CHECK(s.max_days==orders*30 && s.days_complete==0 && s.phase==GK65_ATTACKER_PHASE);
  for(unsigned day=0;day<30*orders;day++){
   CHECK(gk65_apply(&s,GK65_ACT_COMPLETE,GK65_NO_VICTIM)==GK65_OK);
   CHECK(s.phase==GK65_DEFENDER_PHASE && s.days_complete==day);
   CHECK(gk65_apply(&s,GK65_ACT_COMPLETE,GK65_NO_VICTIM)==GK65_OK);
   CHECK(s.days_complete==day+1u);
   if(day+1<30*orders)CHECK(s.phase==GK65_ATTACKER_PHASE);
  }
  CHECK(s.outcome==GK65_ATTACKER_RETREATS_TIME && s.phase==GK65_DONE);
  before=s;CHECK(gk65_apply(&s,GK65_ACT_COMPLETE,GK65_NO_VICTIM)==GK65_REJECTED);
  CHECK(memcmp(&before,&s,sizeof s)==0);
 }
 before=s;CHECK(gk65_begin(0,&s)==GK65_REJECTED);
 CHECK(memcmp(&before,&s,sizeof s)==0);
 CHECK(gk65_begin(4,&s)==GK65_REJECTED);
 CHECK(memcmp(&before,&s,sizeof s)==0);
 for(unsigned side=0;side<2;side++){
  for(unsigned event=GK65_COMMAND_DESTROYED;event<=GK65_COMMAND_CAPTURED_DUEL;event++){
   CHECK(gk65_begin(2,&s)==GK65_OK);
   if(side)CHECK(gk65_apply(&s,GK65_ACT_COMPLETE,GK65_NO_VICTIM)==GK65_OK);
   CHECK(gk65_apply(&s,(Gk65Event)event,side?GK65_ATTACKER_VICTIM:GK65_DEFENDER_VICTIM)==GK65_OK);
   CHECK(s.phase==GK65_DONE && s.terminal_event==event);
   CHECK(s.outcome==(side?GK65_DEFENDER_WON:GK65_ATTACKER_WON));
   CHECK(s.days_complete==0);
  }
 }
 /* Hostile: side of terminal event is explicit, never inferred from phase. */
 CHECK(gk65_begin(2,&s)==GK65_OK);before=s;
 CHECK(gk65_apply(&s,GK65_COMMAND_STARVED,GK65_NO_VICTIM)==GK65_REJECTED);
 CHECK(memcmp(&before,&s,sizeof s)==0);
 CHECK(gk65_apply(&s,GK65_ACT_COMPLETE,GK65_DEFENDER_VICTIM)==GK65_REJECTED);
 CHECK(memcmp(&before,&s,sizeof s)==0);
 CHECK(gk65_apply(&s,GK65_BOTH_COMMANDERS_DESTROYED,GK65_ATTACKER_VICTIM)==GK65_REJECTED);
 CHECK(memcmp(&before,&s,sizeof s)==0);
 CHECK(gk65_apply(&s,GK65_COMMAND_STARVED,GK65_ATTACKER_VICTIM)==GK65_OK);
 CHECK(s.outcome==GK65_DEFENDER_WON && s.days_complete==0);
 CHECK(gk65_begin(3,&s)==GK65_OK);
 CHECK(gk65_apply(&s,GK65_BOTH_COMMANDERS_DESTROYED,GK65_NO_VICTIM)==GK65_OK);
 CHECK(s.outcome==GK65_DEFENDER_RETAINS_DOUBLE_KO && s.attacker_captured);
 CHECK(gk65_begin(1,&s)==GK65_OK);before=s;
 CHECK(gk65_apply(&s,(Gk65Event)7,GK65_NO_VICTIM)==GK65_REJECTED);
 CHECK(memcmp(&before,&s,sizeof s)==0);
 s.days_complete=91;before=s;
 CHECK(gk65_apply(&s,GK65_ACT_COMPLETE,GK65_NO_VICTIM)==GK65_REJECTED);
 CHECK(memcmp(&before,&s,sizeof s)==0);
 Gk1TemplateDataset *d=malloc(sizeof *d);Gk1App *app=malloc(sizeof *app);
 CHECK(d && app);
 CHECK(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,d));
 Gk1LoadedScenario snap0=app->worlds[0],snap1=app->worlds[1];
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM)); /* title -> campaign */
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM)); /* Mongol */
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM)); /* candidate */
 CHECK(gk1_app_input(app,GK1_INPUT_SETUP));
 for(int i=0;i<4;i++)CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 CHECK(app->screen==GK1_SCREEN_SETUP_WIZARD && app->setup_stage[0]==GK53_SETUP_AWAIT_SOURCE);
 CHECK(gk1_app_input(app,GK1_INPUT_LABOR_PLAN));
 CHECK(app->screen==GK1_SCREEN_MANUAL_RULES);
 Gk64ManualPreview preview={0};
 CHECK(gk64_preview(&app->worlds[0],0,3,&preview)==GK64_OK);
 CHECK(preview.first_order_year==1175 && preview.days_for_remaining_orders==90);
 CHECK(gk1_app_input(app,GK1_INPUT_SETUP));
 CHECK(app->screen==GK1_SCREEN_BATTLE_TIMELINE);
 for(unsigned i=0;i<180;i++)CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 CHECK(app->battle_timeline.days_complete==90);
 CHECK(app->battle_timeline.outcome==GK65_ATTACKER_RETREATS_TIME);
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM)); /* terminal ignored */
 CHECK(app->battle_timeline.days_complete==90);
 CHECK(memcmp(&snap0,&app->worlds[0],sizeof snap0)==0);
 CHECK(memcmp(&snap1,&app->worlds[1],sizeof snap1)==0);
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL));
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL));
 CHECK(app->screen==GK1_SCREEN_SETUP_WIZARD);
 /* controller merchant preview with exact quote function; read-only source */
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL)); /* back to stage ABILITIES */
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL)); /* back to LORD */
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL)); /* back to DIFFICULTY */
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL)); /* back to PLAYERS */
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL)); /* back to countries */
 CHECK(gk1_app_input(app,GK1_INPUT_LABOR_PLAN));
 CHECK(gk1_app_input(app,GK1_INPUT_LABOR_PLAN));
 CHECK(app->screen==GK1_SCREEN_MARKET_EXPLORER);
 CHECK(gk1_app_input(app,GK1_INPUT_RIGHT));
 CHECK(app->market_merchant==1);
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));CHECK(app->market_sell==1);
 CHECK(gk1_app_input(app,GK1_INPUT_SETUP));CHECK(app->example_rate==110);
 CHECK(memcmp(&snap0,&app->worlds[0],sizeof snap0)==0);
 CHECK(memcmp(&snap1,&app->worlds[1],sizeof snap1)==0);
 Gk61Quote q={0};CHECK(gk61_quote(app->example_rate,GK61_ISLAM,0,&q)==GK61_OK);
 CHECK(q.buy_tenths>0);
 printf("GK1 EXEC0065 PASS %u direct assertions, 41 source countries unchanged, controller clock90/day and original quote mode; TESTED_PORT_LINUX_C11 only\n",checks);
 free(d);free(app);return 0;
}
