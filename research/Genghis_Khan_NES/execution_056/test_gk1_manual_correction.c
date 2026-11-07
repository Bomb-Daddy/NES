/* EXEC0056: native editor undo/redo inputs; NOT original NES stat algorithm. */
#include "gk1_manual_bonus_store.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define CHECK(x) do{++checks;if(!(x)){fprintf(stderr,"EXEC0056 FAILED %s:%d: %s\n",__FILE__,__LINE__,#x);return 3;}}while(0)
static int press(Gk1PadRouter *pad,Gk1App *a,uint16_t bit){
 Gk1PadSample off={0},on={0};on.buttons=bit;Gk1PadAction event[9];
 (void)gk1_pad_step(pad,&off,16,event,9);
 size_t n=gk1_pad_step(pad,&on,16,event,9);
 for(size_t i=0;i<n;i++){
  Gk1Input key=0;
  switch(event[i]){
   case GK1_PAD_ACT_UP:key=GK1_INPUT_UP;break;
   case GK1_PAD_ACT_DOWN:key=GK1_INPUT_DOWN;break;
   case GK1_PAD_ACT_LEFT:key=GK1_INPUT_LEFT;break;
   case GK1_PAD_ACT_RIGHT:key=GK1_INPUT_RIGHT;break;
   case GK1_PAD_ACT_A:key=GK1_INPUT_CONFIRM;break;
   case GK1_PAD_ACT_B:key=GK1_INPUT_CANCEL;break;
   case GK1_PAD_ACT_MENU:key=GK1_INPUT_SETUP;break;
   case GK1_PAD_ACT_X:key=GK1_INPUT_DRAFT_UNDO_ONE;break;
   case GK1_PAD_ACT_Y:key=GK1_INPUT_DRAFT_UNDO_TEN;break;
   default:break;
  }
  if(key&&!gk1_app_input(a,key))return 0;
 }
 (void)gk1_pad_step(pad,&off,16,event,9);
 return 1;
}
int main(int argc,char **argv){
 if(argc!=2)return 4;
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *s=malloc(sizeof *s),*round=malloc(sizeof *round),*prior=malloc(sizeof *prior);
 CHECK(dataset&&s&&round&&prior);
 CHECK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 gk1_app_init(s);CHECK(gk1_app_install_templates(s,dataset));
 Gk1PadRouter router;gk1_pad_init(&router);
 CHECK(press(&router,s,GK1_PAD_A));
 CHECK(press(&router,s,GK1_PAD_DOWN));CHECK(press(&router,s,GK1_PAD_A));
 CHECK(s->active_world==1 && s->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 CHECK(press(&router,s,GK1_PAD_A));CHECK(press(&router,s,GK1_PAD_MENU));
 CHECK(press(&router,s,GK1_PAD_A));CHECK(press(&router,s,GK1_PAD_A));CHECK(press(&router,s,GK1_PAD_A));
 CHECK(press(&router,s,GK1_PAD_DOWN));CHECK(press(&router,s,GK1_PAD_A));
 CHECK(s->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE && s->setup_ability_method[1]==2);
 CHECK(press(&router,s,GK1_PAD_MENU));CHECK(s->manual_bonus_active[1]);
 CHECK(press(&router,s,GK1_PAD_B));CHECK(press(&router,s,GK1_PAD_A));
 CHECK(s->manual_bonus[1][0][0]==11&&gk55_bonus_spent(s,1)==11);
 CHECK(press(&router,s,GK1_PAD_X));CHECK(s->manual_bonus[1][0][0]==10);
 CHECK(press(&router,s,GK1_PAD_Y));CHECK(s->manual_bonus[1][0][0]==0);
 CHECK(press(&router,s,GK1_PAD_X));CHECK(press(&router,s,GK1_PAD_Y));
 CHECK(gk55_bonus_spent(s,1)==0&&strstr(s->status,"NOTHING TO UNDO")!=NULL);
 CHECK(press(&router,s,GK1_PAD_RIGHT));CHECK(press(&router,s,GK1_PAD_DOWN));
 CHECK(press(&router,s,GK1_PAD_B));CHECK(press(&router,s,GK1_PAD_X));
 CHECK(s->manual_bonus[1][1][1]==9&&s->manual_bonus[1][0][0]==0);
 CHECK(press(&router,s,GK1_PAD_Y));CHECK(gk55_bonus_spent(s,1)==0); /* clamp 9 -> 0 */
 CHECK(press(&router,s,GK1_PAD_A));CHECK(s->manual_bonus[1][1][1]==1);
 CHECK(s->manual_bonus[0][1][1]==0&&s->worlds[0].country_count==14);
 CHECK(s->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 /* Outside the editor, the special action must not mutate any draft. */
 CHECK(press(&router,s,GK1_PAD_MENU));CHECK(!s->manual_bonus_active[1]);
 *prior=*s;
 CHECK(press(&router,s,GK1_PAD_X)&&press(&router,s,GK1_PAD_Y));
 CHECK(memcmp(prior->manual_bonus,s->manual_bonus,sizeof s->manual_bonus)==0);
 uint8_t blob[GK55_PRE_SETUP_BYTES];
 CHECK(gk55_setup_encode(s,dataset,blob,sizeof blob)==GK1_STORE_OK);
 gk1_app_init(round);CHECK(gk1_app_install_templates(round,dataset));
 CHECK(gk55_setup_decode(blob,sizeof blob,dataset,round)==GK1_STORE_OK);
 CHECK(round->manual_bonus[1][1][1]==1&&round->manual_bonus[0][1][1]==0);
 CHECK(round->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 /* Previous save identity remains byte-exact, no format change. */
 CHECK(blob[GK53_PRE_SETUP_BYTES]=='G'&&blob[GK53_PRE_SETUP_BYTES+1]=='K');
 printf("EXEC0056 TESTED_PORT_LINUX editor X -1 / Y -10 zero clamp, isolation, save cold-decode PASS checks=%u\n",checks);
 free(dataset);free(s);free(round);free(prior);return 0;
}
