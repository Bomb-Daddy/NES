#include "gk1_manual_bonus_store.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
static unsigned checks;
#define CK(x) do {checks++;if(!(x)){fprintf(stderr,"EXEC0057 hostile test FAIL %s:%d %s\n",__FILE__,__LINE__,#x);return 2;}}while(0)
static int press(Gk1PadRouter *pad,Gk1App *a,uint16_t button){
 Gk1PadSample zero={0},one={0};one.buttons=button;Gk1PadAction events[16];
 (void)gk1_pad_step(pad,&zero,16,events,16);
 size_t n=gk1_pad_step(pad,&one,16,events,16);int result=1;
 for(size_t i=0;i<n;i++){
  Gk1Input k=0;
  switch(events[i]){
   case GK1_PAD_ACT_UP:k=GK1_INPUT_UP;break;
   case GK1_PAD_ACT_DOWN:k=GK1_INPUT_DOWN;break;
   case GK1_PAD_ACT_LEFT:k=GK1_INPUT_LEFT;break;
   case GK1_PAD_ACT_RIGHT:k=GK1_INPUT_RIGHT;break;
   case GK1_PAD_ACT_A:k=GK1_INPUT_CONFIRM;break;
   case GK1_PAD_ACT_B:k=GK1_INPUT_CANCEL;break;
   case GK1_PAD_ACT_MENU:k=GK1_INPUT_SETUP;break;
   case GK1_PAD_ACT_VIEW:k=GK1_INPUT_LABOR_PLAN;break;
   default:break;
  }
  if(k&&!gk1_app_input(a,k))result=0;
 }
 (void)gk1_pad_step(pad,&zero,16,events,16);return result;
}
static unsigned sum(const Gk1CountryTemplate *c){return (unsigned)c->troops+c->town+c->masons+c->artisans+c->food_workers;}
int main(int argc,char **argv){
 if(argc!=2)return 3;
 Gk1TemplateDataset *src=malloc(sizeof *src);Gk1App *a=malloc(sizeof *a),*restored=malloc(sizeof *restored),*old=malloc(sizeof *old);
 CK(src&&a&&restored&&old);
 CK(gk1_template_load(argv[1],src)==GK1_LOAD_OK);
 gk1_app_init(a);CK(gk1_app_install_templates(a,src));
 CK(a->worlds[0].country_count==14 && a->worlds[1].country_count==27);
 Gk1PadRouter router;gk1_pad_init(&router);
 CK(press(&router,a,GK1_PAD_A));CK(press(&router,a,GK1_PAD_A));
 CK(a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 Gk1CountryTemplate target=a->worlds[0].countries[0],neighbor=a->worlds[0].countries[1];
 Gk1LoadedScenario second=a->worlds[1];
 CK(sum(&target)==target.drafted_population);
 CK(press(&router,a,GK1_PAD_VIEW));CK(a->screen==GK1_SCREEN_LABOR_PLAN);
 CK(a->labor_from==0&&a->labor_to==1);
 for(unsigned i=0;i<4;i++)CK(press(&router,a,GK1_PAD_A));
 CK(a->worlds[0].countries[0].troops==target.troops-4);
 CK(a->worlds[0].countries[0].town==target.town+4);
 CK(sum(&a->worlds[0].countries[0])==target.drafted_population);
 CK(a->worlds[0].countries[0].undrafted_reserve==target.undrafted_reserve);
 CK(a->worlds[0].countries[0].combined_population==target.combined_population);
 CK(memcmp(&a->worlds[0].countries[1],&neighbor,sizeof neighbor)==0);
 CK(memcmp(&a->worlds[1],&second,sizeof second)==0);
 CK(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 /* A -> same pool must be a no-op; B back retains committed planning data. */
 CK(press(&router,a,GK1_PAD_UP)); /* dest town -> troops */
 Gk1CountryTemplate before=a->worlds[0].countries[0];
 CK(press(&router,a,GK1_PAD_A));
 CK(memcmp(&a->worlds[0].countries[0],&before,sizeof before)==0);
 CK(press(&router,a,GK1_PAD_B));CK(a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 CK(a->worlds[0].countries[0].troops==target.troops-4);
 CK(press(&router,a,GK1_PAD_DOWN));CK(press(&router,a,GK1_PAD_VIEW));
 CK(a->screen==GK1_SCREEN_LABOR_PLAN);
 /* Country2 zero source manually and verify atomic refusal; restore afterwards. */
 Gk1CountryTemplate *c=&a->worlds[0].countries[1];
 uint16_t troops=c->troops;c->troops=0;
 Gk1CountryTemplate empty_before=*c;
 CK(press(&router,a,GK1_PAD_A));
 CK(memcmp(c,&empty_before,sizeof *c)==0);
 c->troops=troops;
 CK(press(&router,a,GK1_PAD_B));
 CK(press(&router,a,GK1_PAD_UP));CK(a->country_cursor[0]==0);
 /* Already known 2734-byte source-gated state cold-save validates all fields. */
 uint8_t bytes[GK55_PRE_SETUP_BYTES],corrupt[GK55_PRE_SETUP_BYTES];
 CK(gk55_setup_encode(a,src,bytes,sizeof bytes)==GK1_STORE_OK);
 gk1_app_init(restored);CK(gk1_app_install_templates(restored,src));
 CK(gk55_setup_decode(bytes,sizeof bytes,src,restored)==GK1_STORE_OK);
 CK(restored->worlds[0].countries[0].troops==target.troops-4);
 CK(restored->worlds[0].countries[0].town==target.town+4);
 CK(memcmp(&restored->worlds[1],&second,sizeof second)==0);
 *old=*restored;memcpy(corrupt,bytes,sizeof bytes);corrupt[247]^=1;
 CK(gk55_setup_decode(corrupt,sizeof corrupt,src,restored)!=GK1_STORE_OK);
 CK(memcmp(old,restored,sizeof *old)==0);
 CK(gk55_setup_decode(bytes,sizeof bytes-1,src,restored)!=GK1_STORE_OK);
 CK(memcmp(old,restored,sizeof *old)==0);
 /* Preserve EXEC0056 subtract commands; no underflow or cross-world damage. */
 restored->manual_bonus_active[0]=1;
 restored->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 restored->setup_ability_method[0]=2;
 restored->screen=GK1_SCREEN_SETUP_WIZARD;
 CK(gk1_app_input(restored,GK1_INPUT_CANCEL)); /* +10, not a screen cancel */
 CK(restored->manual_bonus[0][0][0]==10);
 CK(gk1_app_input(restored,GK1_INPUT_DRAFT_UNDO_ONE));
 CK(restored->manual_bonus[0][0][0]==9);
 CK(gk1_app_input(restored,GK1_INPUT_DRAFT_UNDO_TEN));
 CK(restored->manual_bonus[0][0][0]==0);
 CK(gk1_app_input(restored,GK1_INPUT_DRAFT_UNDO_TEN));
 CK(restored->manual_bonus[0][0][0]==0);
 CK(gk55_bonus_spent(restored,1)==0);
 puts("EXEC0057 TESTED_PORT_LINUX: actual app controller View labor planner + conservation, zero/same refusal, scenario isolation, 2734-byte cold-save, corruption rollback, 0056 undo preservation PASS");
 printf("assertions=%u\n",checks);
 free(a);free(restored);free(old);free(src);return 0;
}
