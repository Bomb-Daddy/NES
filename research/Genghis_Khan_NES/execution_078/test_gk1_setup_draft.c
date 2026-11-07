#include "gk1_setup_draft_store.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CK(x) do{checks++;if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}}while(0)
static void command(Gk1App *a,Gk1Input c){if(!gk1_app_input(a,c))abort();}
static void pad(Gk1PadRouter *r,Gk1App *a,uint16_t mask){
 Gk1PadSample idle={0},press={0};press.buttons=mask;
 Gk1PadAction q[8];(void)gk1_pad_step(r,&idle,16,q,8);
 size_t n=gk1_pad_step(r,&press,16,q,8);
 for(size_t i=0;i<n;i++)switch(q[i]){
  case GK1_PAD_ACT_UP:command(a,GK1_INPUT_UP);break;
  case GK1_PAD_ACT_DOWN:command(a,GK1_INPUT_DOWN);break;
  case GK1_PAD_ACT_LEFT:command(a,GK1_INPUT_LEFT);break;
  case GK1_PAD_ACT_RIGHT:command(a,GK1_INPUT_RIGHT);break;
  case GK1_PAD_ACT_A:command(a,GK1_INPUT_CONFIRM);break;
  case GK1_PAD_ACT_B:command(a,GK1_INPUT_CANCEL);break;
  default:break;
 }
 (void)gk1_pad_step(r,&idle,16,q,8);
}
int main(int argc,char **argv){
 if(argc!=2)return 4;
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *a=malloc(sizeof *a),*before=malloc(sizeof *before);
 if(!dataset||!a||!before)return 5;
 CK(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 gk1_app_init(a);CK(gk1_app_install_templates(a,dataset));
 CK(a->setup_candidate[0]==255&&a->setup_candidate[1]==255&&a->rules_profile==1);
 Gk1PadRouter router;gk1_pad_init(&router);
 pad(&router,a,GK1_PAD_A); /* campaign menu */
 CK(a->screen==GK1_SCREEN_CAMPAIGNS);
 pad(&router,a,GK1_PAD_LEFT);CK(a->rules_profile==0);
 pad(&router,a,GK1_PAD_A);CK(a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 pad(&router,a,GK1_PAD_DOWN);pad(&router,a,GK1_PAD_DOWN);
 pad(&router,a,GK1_PAD_A);CK(a->setup_candidate[0]==2);
 CK(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE);
 CK(a->worlds[0].countries[2].country_id==3);
 pad(&router,a,GK1_PAD_B);CK(a->screen==GK1_SCREEN_CAMPAIGNS);
 pad(&router,a,GK1_PAD_DOWN);pad(&router,a,GK1_PAD_A);
 CK(a->active_world==1&&a->worlds[1].country_count==27);
 CK(a->setup_candidate[1]==255);
 pad(&router,a,GK1_PAD_A);CK(a->setup_candidate[1]==0);
 pad(&router,a,GK1_PAD_A);CK(a->setup_candidate[1]==255);
 pad(&router,a,GK1_PAD_RIGHT);pad(&router,a,GK1_PAD_A);
 CK(a->setup_candidate[1]==1);
 uint8_t b[GK52_PRE_SETUP_BYTES],legacy[GK51_PRE_SETUP_BYTES];
 CK(gk52_setup_encode(a,dataset,b,sizeof b)==GK1_STORE_OK);
 CK(gk51_pre_setup_encode(a,dataset,legacy,sizeof legacy)==GK1_STORE_OK);
 CK(!memcmp(b,legacy,sizeof legacy));
 *before=*a;
 Gk1App *restored=malloc(sizeof *restored);CK(restored!=NULL);
 gk1_app_init(restored);CK(gk1_app_install_templates(restored,dataset));
 CK(gk52_setup_decode(b,sizeof b,dataset,restored)==GK1_STORE_OK);
 CK(restored->rules_profile==0&&restored->setup_candidate[0]==2&&restored->setup_candidate[1]==1);
 CK(restored->active_world==1&&restored->worlds[0].countries[2].country_id==3);
 CK(gk52_setup_decode(legacy,sizeof legacy,dataset,restored)==GK1_STORE_OK);
 CK(restored->rules_profile==0&&restored->setup_candidate[0]==255&&restored->setup_candidate[1]==255);
 for(unsigned i=0;i<sizeof b;i++){
  uint8_t old=b[i];b[i]^=1;
  CK(gk52_setup_decode(b,sizeof b,dataset,a)!=GK1_STORE_OK);
  CK(!memcmp(a,before,sizeof *a));
  b[i]=old;
 }
 CK(gk52_setup_decode(b,sizeof b-1,dataset,a)!=GK1_STORE_OK);
 CK(!memcmp(a,before,sizeof *a));
 CK(gk52_setup_decode(b,sizeof b+1,dataset,a)!=GK1_STORE_OK);
 CK(!memcmp(a,before,sizeof *a));
 /* Rechecksummed attempts to smuggle invalid unused bytes / indices rejected. */
 b[GK51_PRE_SETUP_BYTES+12]=1;
 CK(gk52_setup_decode(b,sizeof b,dataset,a)!=GK1_STORE_OK);
 CK(!memcmp(a,before,sizeof *a));
 b[GK51_PRE_SETUP_BYTES+12]=0;
 b[GK51_PRE_SETUP_BYTES+10]=14;
 CK(gk52_setup_decode(b,sizeof b,dataset,a)!=GK1_STORE_OK);
 CK(!memcmp(a,before,sizeof *a));
 b[GK51_PRE_SETUP_BYTES+10]=2;
 CK(gk52_setup_decode(b,sizeof b,dataset,a)==GK1_STORE_OK);
 CK(a->setup_candidate[0]==2);
 /* Classic must refuse APPROVED MODIFIED battle fixtures without side effects. */
 command(a,GK1_INPUT_CANCEL);CK(a->screen==GK1_SCREEN_CAMPAIGNS);
 command(a,GK1_INPUT_CANCEL);CK(a->screen==GK1_SCREEN_TITLE);
 command(a,GK1_INPUT_DOWN);CK(a->menu_index==1);
 command(a,GK1_INPUT_CONFIRM);CK(a->screen==GK1_SCREEN_TITLE);
 CK(a->rules_profile==0&&a->setup_candidate[0]==2);
 CK(a->worlds[0].countries[2].country_id==3);
 /* Actual binary disk save/new and load in a second process are also CLI-tested. */
 printf("EXEC0052 controller/profile candidate v2 + legacy decode QA %u assertions PASS (synthetic controller, C11 Linux)\n",checks);
 free(restored);free(before);free(a);free(dataset);return 0;
}
