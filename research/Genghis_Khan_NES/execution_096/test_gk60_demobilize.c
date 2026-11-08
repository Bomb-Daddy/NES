#include "gk60_demobilize.h"
#include "gk1_manual_bonus_store.h"
#include "gk1_pad_router.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static unsigned checks;
#define CK(test) do { checks++; if(!(test)){fprintf(stderr,"EXEC0060 FAIL %s:%d: %s\n",__FILE__,__LINE__,#test);return 2;} }while(0)
static int press(Gk1PadRouter *pad,Gk1App *a,uint16_t bit){
 Gk1PadSample zero={0},one={0};one.buttons=bit;Gk1PadAction events[16];
 (void)gk1_pad_step(pad,&zero,16,events,16);
 size_t n=gk1_pad_step(pad,&one,16,events,16);
 int ok=1;
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
  if(k&&!gk1_app_input(a,k))ok=0;
 }
 (void)gk1_pad_step(pad,&zero,16,events,16);
 return ok;
}
int main(int argc,char **argv){
 if(argc!=2)return 3;
 Gk1TemplateDataset *src=malloc(sizeof *src);
 Gk1App *a=malloc(sizeof *a),*after=malloc(sizeof *after);
 CK(src&&a&&after);
 CK(gk1_template_load(argv[1],src)==GK1_LOAD_OK);
 gk1_app_init(a);CK(gk1_app_install_templates(a,src));
 Gk1LoadedScenario untouched_world=a->worlds[1];
 Gk1CountryTemplate untouched_neighbor=a->worlds[0].countries[1];
 Gk1CountryTemplate before=a->worlds[0].countries[0];
 CK(before.troops>20 && before.undrafted_reserve<970);
 CK(gk60_demob_max(&before)==before.troops);
 Gk1CountryTemplate nochange=before;
 CK(gk60_demob_apply(&nochange,0)==GK60_DEMOB_REFUSED);
 CK(memcmp(&nochange,&before,sizeof before)==0);
 CK(gk60_demob_apply(&nochange,(uint16_t)(before.troops+1))==GK60_DEMOB_REFUSED);
 CK(memcmp(&nochange,&before,sizeof before)==0);
 CK(gk60_demob_apply(NULL,1)==GK60_DEMOB_REFUSED);
 CK(gk60_demob_max(NULL)==0);
 Gk1CountryTemplate bad=before;
 bad.combined_population++;
 Gk1CountryTemplate saved_bad=bad;
 CK(gk60_demob_max(&bad)==0);
 CK(gk60_demob_apply(&bad,1)==GK60_DEMOB_REFUSED);
 CK(memcmp(&bad,&saved_bad,sizeof bad)==0);
 Gk1CountryTemplate cap=before;
 cap.undrafted_reserve=998;
 cap.combined_population=(uint16_t)(cap.drafted_population+998u);
 cap.eligible_draft_count=(uint16_t)((uint32_t)cap.undrafted_reserve*cap.eligibility_percent/100u);
 CK(gk60_demob_max(&cap)==1);
 Gk1CountryTemplate capbefore=cap;
 CK(gk60_demob_apply(&cap,2)==GK60_DEMOB_REFUSED);
 CK(memcmp(&cap,&capbefore,sizeof cap)==0);
 CK(gk60_demob_apply(&cap,1)==GK60_DEMOB_OK);
 CK(cap.undrafted_reserve==999&&cap.troops==before.troops-1);
 CK(gk60_demob_max(&cap)==0);
 CK(gk60_demob_apply(&cap,1)==GK60_DEMOB_REFUSED);

 Gk1PadRouter pad;gk1_pad_init(&pad);
 CK(press(&pad,a,GK1_PAD_A));CK(a->screen==GK1_SCREEN_CAMPAIGNS);
 CK(press(&pad,a,GK1_PAD_LEFT));CK(a->rules_profile==0);
 CK(press(&pad,a,GK1_PAD_A));CK(a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 CK(press(&pad,a,GK1_PAD_VIEW));CK(a->screen==GK1_SCREEN_LABOR_PLAN);
 CK(press(&pad,a,GK1_PAD_MENU));CK(a->screen==GK1_SCREEN_LABOR_PLAN);
 CK(memcmp(&a->worlds[0].countries[0],&before,sizeof before)==0);
 CK(press(&pad,a,GK1_PAD_B));CK(press(&pad,a,GK1_PAD_B));
 CK(a->screen==GK1_SCREEN_CAMPAIGNS);
 CK(press(&pad,a,GK1_PAD_RIGHT));CK(a->rules_profile==1);
 CK(press(&pad,a,GK1_PAD_A));CK(press(&pad,a,GK1_PAD_VIEW));
 CK(press(&pad,a,GK1_PAD_MENU));CK(a->screen==GK1_SCREEN_DEMOBILIZE);
 CK(a->demob_quantity==1);
 CK(press(&pad,a,GK1_PAD_RIGHT));CK(a->demob_quantity==11);
 CK(press(&pad,a,GK1_PAD_DOWN));CK(a->demob_quantity==10);
 CK(press(&pad,a,GK1_PAD_A));
 Gk1CountryTemplate *current=&a->worlds[0].countries[0];
 CK(current->troops==before.troops-10);
 CK(current->undrafted_reserve==before.undrafted_reserve+10);
 CK(current->drafted_population==before.drafted_population-10);
 CK(current->combined_population==before.combined_population);
 CK(current->eligible_draft_count==((uint32_t)current->undrafted_reserve*current->eligibility_percent/100u));
 CK(current->town==before.town&&current->artisans==before.artisans);
 CK(current->gold==before.gold&&current->goods[0]==before.goods[0]);
 CK(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 CK(memcmp(&a->worlds[0].countries[1],&untouched_neighbor,sizeof untouched_neighbor)==0);
 CK(memcmp(&a->worlds[1],&untouched_world,sizeof untouched_world)==0);
 CK(press(&pad,a,GK1_PAD_B));CK(a->screen==GK1_SCREEN_LABOR_PLAN);
 CK(press(&pad,a,GK1_PAD_B));CK(a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 uint8_t save[GK55_PRE_SETUP_BYTES];
 CK(gk55_setup_encode(a,src,save,sizeof save)==GK1_STORE_OK);
 gk1_app_init(after);CK(gk1_app_install_templates(after,src));
 CK(gk55_setup_decode(save,sizeof save,src,after)==GK1_STORE_OK);
 CK(after->worlds[0].countries[0].troops==before.troops-10);
 CK(after->worlds[0].countries[0].undrafted_reserve==before.undrafted_reserve+10);
 CK(memcmp(&after->worlds[1],&untouched_world,sizeof untouched_world)==0);
 Gk1App prev=*after;
 save[321]^=0x80;
 CK(gk55_setup_decode(save,sizeof save,src,after)!=GK1_STORE_OK);
 CK(memcmp(after,&prev,sizeof prev)==0);
 printf("EXEC0060 TESTED_PORT_LINUX Enhanced-only controller Demobilize +10, population conservation, source caps, Classic gate, cross-world isolation, cold-save, corrupted rollback PASS. Assertions=%u\n",checks);
 free(after);free(a);free(src);return 0;
}
