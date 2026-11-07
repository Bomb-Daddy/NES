#include "gk1_manual_setup_store.h"
#include "gk1_pad_router.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static unsigned tests;
#define CK(q) do{tests++;if(!(q)){fprintf(stderr,"EXEC0053 FAIL %s:%d: %s\n",__FILE__,__LINE__,#q);return 2;}}while(0)
static int press(Gk1PadRouter *p,Gk1App *a,uint16_t b){
 Gk1PadSample zero={0},down={0};down.buttons=b;
 Gk1PadAction events[8];(void)gk1_pad_step(p,&zero,16,events,8);
 size_t count=gk1_pad_step(p,&down,16,events,8);
 for(size_t i=0;i<count;i++){
  Gk1Input cmd=0;
  switch(events[i]){
   case GK1_PAD_ACT_UP:cmd=GK1_INPUT_UP;break;
   case GK1_PAD_ACT_DOWN:cmd=GK1_INPUT_DOWN;break;
   case GK1_PAD_ACT_LEFT:cmd=GK1_INPUT_LEFT;break;
   case GK1_PAD_ACT_RIGHT:cmd=GK1_INPUT_RIGHT;break;
   case GK1_PAD_ACT_A:cmd=GK1_INPUT_CONFIRM;break;
   case GK1_PAD_ACT_B:cmd=GK1_INPUT_CANCEL;break;
   case GK1_PAD_ACT_MENU:cmd=GK1_INPUT_SETUP;break;
   default:break;
  }
  if(cmd&&!gk1_app_input(a,cmd))return 0;
 }
 (void)gk1_pad_step(p,&zero,16,events,8);return 1;
}
static void re_crc(uint8_t *b,size_t n){
 uint32_t c=UINT32_C(0xffffffff);
 for(size_t i=0;i<n-4;i++){c^=b[i];for(int j=0;j<8;j++)
  c=(c>>1)^((c&1)?UINT32_C(0xedb88320):0);}
 c=~c;for(int j=0;j<4;j++)b[n-4+j]=(uint8_t)(c>>(j*8));
}
int main(int argc,char **argv){
 if(argc!=2)return 3;
 Gk1TemplateDataset *data=malloc(sizeof *data);
 Gk1App *a=malloc(sizeof *a),*before=malloc(sizeof *before),*loaded=malloc(sizeof *loaded);
 CK(data&&a&&before&&loaded);
 CK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 gk1_app_init(a);CK(gk1_app_install_templates(a,data));
 CK(gk53_setup_allowed_lord(0,0)&&!gk53_setup_allowed_lord(0,1));
 CK(gk53_setup_allowed_lord(1,0)&&gk53_setup_allowed_lord(1,6)&&
    gk53_setup_allowed_lord(1,10)&&gk53_setup_allowed_lord(1,18));
 for(int id=0;id<27;id++)if(id!=0&&id!=6&&id!=10&&id!=18)CK(!gk53_setup_allowed_lord(1,(uint8_t)id));
 CK(gk53_setup_country_for_slot(1,3)==18&&gk53_setup_country_for_slot(1,4)==255);
 Gk1PadRouter pad;gk1_pad_init(&pad);
 CK(press(&pad,a,GK1_PAD_A)); /* title -> campaign */
 CK(press(&pad,a,GK1_PAD_DOWN)&&press(&pad,a,GK1_PAD_A)); /* World */
 CK(a->active_world==1&&a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 for(int i=0;i<1;i++)CK(press(&pad,a,GK1_PAD_DOWN));
 CK(press(&pad,a,GK1_PAD_A)); /* country 2 NOT allowed */
 CK(press(&pad,a,GK1_PAD_MENU));
 CK(a->screen==GK1_SCREEN_SCENARIO_COUNTRIES &&
    a->setup_candidate[1]==1&&a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 CK(press(&pad,a,GK1_PAD_A)); /* clear */
 for(int i=1;i<6;i++)CK(press(&pad,a,GK1_PAD_DOWN)); /* now Richard index6 */
 CK(a->country_cursor[1]==6&&press(&pad,a,GK1_PAD_A));
 CK(a->setup_candidate[1]==6&&press(&pad,a,GK1_PAD_MENU));
 CK(a->screen==GK1_SCREEN_SETUP_WIZARD&&a->setup_stage[1]==GK53_SETUP_PLAYERS);
 CK(press(&pad,a,GK1_PAD_UP)&&a->setup_player_count[1]==2);
 CK(press(&pad,a,GK1_PAD_A)&&a->setup_stage[1]==GK53_SETUP_DIFFICULTY);
 for(int i=0;i<4;i++)CK(press(&pad,a,GK1_PAD_UP));
 CK(a->setup_difficulty[1]==5);
 CK(press(&pad,a,GK1_PAD_A)&&a->setup_stage[1]==GK53_SETUP_LORD);
 CK(press(&pad,a,GK1_PAD_A)&&a->setup_player_slot[1]==1&&a->setup_rulers[1][0]==6);
 CK(a->country_cursor[1]==0);
 CK(press(&pad,a,GK1_PAD_A));
 CK(a->setup_rulers[1][1]==0&&a->setup_stage[1]==GK53_SETUP_ABILITIES);
 CK(press(&pad,a,GK1_PAD_DOWN));
 CK(press(&pad,a,GK1_PAD_A)&&a->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE);
 CK(a->setup_ability_method[1]==2 && a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 CK(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&a->setup_candidate[0]==255);
 /* The command boundary is deliberately NOT playable Spring. */
 CK(press(&pad,a,GK1_PAD_A)&&a->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE);
 uint8_t bytes[GK53_PRE_SETUP_BYTES],legacy[GK52_PRE_SETUP_BYTES];
 CK(gk53_setup_encode(a,data,bytes,sizeof bytes)==GK1_STORE_OK);
 CK(gk52_setup_encode(a,data,legacy,sizeof legacy)==GK1_STORE_OK);
 CK(memcmp(bytes,legacy,sizeof legacy)==0);
 gk1_app_init(loaded);CK(gk1_app_install_templates(loaded,data));
 CK(gk53_setup_decode(bytes,sizeof bytes,data,loaded)==GK1_STORE_OK);
 CK(loaded->setup_player_count[1]==2&&loaded->setup_difficulty[1]==5);
 CK(loaded->setup_rulers[1][0]==6&&loaded->setup_rulers[1][1]==0);
 CK(loaded->setup_ability_method[1]==2&&loaded->screen==GK1_SCREEN_SETUP_WIZARD);
 *before=*a;
 /* Every single-byte mutation must be caught; existing app remains untouched. */
 for(size_t i=0;i<sizeof bytes;i++){
  uint8_t saved=bytes[i];bytes[i]^=1;
  CK(gk53_setup_decode(bytes,sizeof bytes,data,a)!=GK1_STORE_OK);
  CK(memcmp(a,before,sizeof *a)==0);
  bytes[i]=saved;
 }
 /* Attackers can re-checksum, so validate semantics independently. */
 size_t o=GK52_PRE_SETUP_BYTES;
 bytes[o+10]=5;re_crc(bytes,sizeof bytes);
 CK(gk53_setup_decode(bytes,sizeof bytes,data,a)!=GK1_STORE_OK);
 CK(memcmp(a,before,sizeof *a)==0);
 bytes[o+10]=2;bytes[o+26]=6;re_crc(bytes,sizeof bytes); /* duplicate ruler */
 CK(gk53_setup_decode(bytes,sizeof bytes,data,a)!=GK1_STORE_OK);
 CK(memcmp(a,before,sizeof *a)==0);
 bytes[o+26]=0;bytes[o+29]=1;re_crc(bytes,sizeof bytes);
 CK(gk53_setup_decode(bytes,sizeof bytes,data,a)!=GK1_STORE_OK);
 CK(memcmp(a,before,sizeof *a)==0);
 bytes[o+29]=0;re_crc(bytes,sizeof bytes);
 CK(gk53_setup_decode(bytes,sizeof bytes,data,a)==GK1_STORE_OK);
 CK(gk53_setup_decode(bytes,sizeof bytes-1,data,a)!=GK1_STORE_OK);
 CK(gk53_setup_decode(bytes,sizeof bytes+1,data,a)!=GK1_STORE_OK);
 /* Previously shipped v52 accepted with safe defaults: no manufactured Spring. */
 CK(gk53_setup_decode(legacy,sizeof legacy,data,loaded)==GK1_STORE_OK);
 CK(loaded->setup_player_count[0]==1&&loaded->setup_player_count[1]==1);
 CK(loaded->setup_stage[1]==GK53_SETUP_PLAYERS&&loaded->setup_rulers[1][0]==255);
 /* Rollback and no cross-scenario leakage after another original campaign. */
 CK(press(&pad,a,GK1_PAD_B)&&a->setup_stage[1]==GK53_SETUP_ABILITIES);
 CK(press(&pad,a,GK1_PAD_B)&&a->setup_stage[1]==GK53_SETUP_LORD);
 CK(a->setup_player_slot[1]==1&&a->setup_rulers[1][1]==255);
 CK(a->worlds[0].countries[0].gold==425);
 printf("EXEC0053 original NES setup gating/2-player manual abilities/save %u checks PASS (Linux synthetic pad, no NES ability roll)\n",tests);
 free(loaded);free(before);free(a);free(data);return 0;
}
