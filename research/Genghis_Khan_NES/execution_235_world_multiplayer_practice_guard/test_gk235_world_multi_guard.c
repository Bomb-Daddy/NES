/* EXEC0235: same native world and input dispatcher. Original template countries
 * remain PRE_SETUP. This tests a bounded Enhanced-only practice refusal, not
 * authentic original Spring, which remains unimplemented. */
#include "gk1_app_state.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(q) do{checks++;if(!(q)){fprintf(stderr,"GK235 line %d: %s\n",__LINE__,#q);return 1;}}while(0)
static int press(Gk1PadRouter *pad,Gk1App *a,uint16_t key){
 Gk1PadAction actions[8];Gk1PadSample zero={0},one={0};one.buttons=key;
 (void)gk1_pad_step(pad,&zero,16,actions,8);
 size_t count=gk1_pad_step(pad,&one,16,actions,8);
 for(size_t j=0;j<count;j++){
  Gk1Input input=0;
  switch(actions[j]){
   case GK1_PAD_ACT_A:input=GK1_INPUT_CONFIRM;break;
   case GK1_PAD_ACT_B:input=GK1_INPUT_CANCEL;break;
   case GK1_PAD_ACT_UP:input=GK1_INPUT_UP;break;
   case GK1_PAD_ACT_DOWN:input=GK1_INPUT_DOWN;break;
   case GK1_PAD_ACT_LEFT:input=GK1_INPUT_LEFT;break;
   case GK1_PAD_ACT_RIGHT:input=GK1_INPUT_RIGHT;break;
   case GK1_PAD_ACT_MENU:input=GK1_INPUT_SETUP;break;
   default:break;
  }
  if(input&&!gk1_app_input(a,input))return 0;
 }
 (void)gk1_pad_step(pad,&zero,16,actions,8);return 1;
}
static int world_setup(Gk1App *app,Gk1PadRouter *pad,unsigned humans){
 /* New game starts Enhanced. Select World, then country #1. */
 CHECK(press(pad,app,GK1_PAD_A));
 CHECK(press(pad,app,GK1_PAD_DOWN));
 CHECK(press(pad,app,GK1_PAD_A));
 CHECK(app->active_world==1u && app->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 CHECK(press(pad,app,GK1_PAD_A));
 CHECK(press(pad,app,GK1_PAD_MENU));
 for(unsigned i=1;i<humans;i++)CHECK(press(pad,app,GK1_PAD_UP));
 CHECK(press(pad,app,GK1_PAD_A));
 CHECK(press(pad,app,GK1_PAD_A));
 for(unsigned i=0;i<humans;i++)CHECK(press(pad,app,GK1_PAD_A));
 CHECK(press(pad,app,GK1_PAD_A));
 CHECK(app->screen==GK1_SCREEN_SETUP_WIZARD&&app->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE);
 CHECK(app->setup_player_count[1]==humans&&app->setup_ability_method[1]==1);
 return 0;
}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 Gk1TemplateDataset *data=malloc(sizeof *data);
 Gk1App *app=malloc(sizeof *app),*before=malloc(sizeof *before);
 CHECK(data&&app&&before);
 CHECK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 for(unsigned people=1;people<=4;people++){
  gk1_app_init(app);
  CHECK(gk1_app_install_templates(app,data));
  Gk1PadRouter pad;gk1_pad_init(&pad);
  CHECK(world_setup(app,&pad,people)==0);
  CHECK(app->worlds[0].country_count==14u&&app->worlds[1].country_count==27u);
  CHECK(app->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&app->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
  *before=*app;
  if(people>1){
   CHECK(gk1_app_enhanced_spring_lab_open(app)==GK222_BAD_SOURCE);
   CHECK(memcmp(before,app,sizeof *app)==0); /* pure rejected direct API */
   CHECK(press(&pad,app,GK1_PAD_A));
   CHECK(app->screen==GK1_SCREEN_SETUP_WIZARD && !app->port_spring.active);
   CHECK(strstr(app->status,"MULTI-HUMAN SPRING NOT IMPLEMENTED")!=NULL);
   CHECK(memcmp(before->worlds,app->worlds,sizeof app->worlds)==0);
   CHECK(memcmp(before->setup_rulers,app->setup_rulers,sizeof app->setup_rulers)==0);
   CHECK(memcmp(before->setup_player_count,app->setup_player_count,sizeof app->setup_player_count)==0);
   CHECK(app->manual_remaining_orders==before->manual_remaining_orders);
   CHECK(app->stock_checkpoint.orders.remaining==before->stock_checkpoint.orders.remaining);
   /* Repeated A cannot promote pre-setup or spend a fake order. */
   for(unsigned rep=0;rep<3;rep++)CHECK(press(&pad,app,GK1_PAD_A));
   CHECK(app->screen==GK1_SCREEN_SETUP_WIZARD&&!app->port_spring.active);
   CHECK(memcmp(before->worlds,app->worlds,sizeof app->worlds)==0);
  }else{
   CHECK(press(&pad,app,GK1_PAD_A));
   CHECK(app->screen==GK1_SCREEN_ENHANCED_SPRING_LAB&&app->port_spring.active);
   CHECK(app->port_spring.remaining==3u);
   CHECK(memcmp(before->worlds,app->worlds,sizeof app->worlds)==0);
   CHECK(app->rules_profile==1u);
  }
 }
 printf("EXEC0235 %u checks: 1-human opt-in Enhanced practice, 2-4 human fail-closed and all 41 PRE_SETUP countries preserved PASS\n",checks);
 free(before);free(app);free(data);return 0;
}
