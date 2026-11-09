#include "gk1_manual_bonus_store.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CK(x) do {checks++; if(!(x)){fprintf(stderr,"EXEC0055 FAIL %s:%d %s\n",__FILE__,__LINE__,#x);return 2;}}while(0)
static int press(Gk1PadRouter *pad,Gk1App *app,uint16_t bit){
 Gk1PadSample empty={0},held={0};held.buttons=bit;Gk1PadAction events[8];
 (void)gk1_pad_step(pad,&empty,16,events,8);
 size_t n=gk1_pad_step(pad,&held,16,events,8);
 for(size_t i=0;i<n;i++){
  Gk1Input key=0;
  switch(events[i]){
   case GK1_PAD_ACT_UP:key=GK1_INPUT_UP;break;
   case GK1_PAD_ACT_DOWN:key=GK1_INPUT_DOWN;break;
   case GK1_PAD_ACT_LEFT:key=GK1_INPUT_LEFT;break;
   case GK1_PAD_ACT_RIGHT:key=GK1_INPUT_RIGHT;break;
   case GK1_PAD_ACT_A:key=GK1_INPUT_CONFIRM;break;
   case GK1_PAD_ACT_B:key=GK1_INPUT_CANCEL;break;
   case GK1_PAD_ACT_MENU:key=GK1_INPUT_SETUP;break;
   default:break;
  }
  if(key&&!gk1_app_input(app,key))return 0;
 }
 (void)gk1_pad_step(pad,&empty,16,events,8);return 1;
}
static void re_crc(uint8_t *b,size_t n){
 uint32_t v=UINT32_C(0xffffffff);
 for(size_t i=0;i<n-4;i++){v^=b[i];for(unsigned j=0;j<8;j++)v=(v>>1)^((v&1u)?UINT32_C(0xedb88320):0u);}
 v=~v;for(unsigned i=0;i<4;i++)b[n-4+i]=(uint8_t)(v>>(8*i));
}
int main(int argc,char **argv){
 if(argc!=2)return 3;
 Gk1App *a=malloc(sizeof *a),*loaded=malloc(sizeof *loaded),*before=malloc(sizeof *before);
 Gk1TemplateDataset *source=malloc(sizeof *source);
 CK(a&&loaded&&before&&source);
 CK(gk1_template_load(argv[1],source)==GK1_LOAD_OK);
 gk1_app_init(a);CK(gk1_app_install_templates(a,source));
 Gk1PadRouter pad;gk1_pad_init(&pad);
 CK(press(&pad,a,GK1_PAD_A)); /* title->campaign */
 CK(press(&pad,a,GK1_PAD_DOWN)&&press(&pad,a,GK1_PAD_A)); /* World pre-setup */
 CK(a->active_world==1 && a->screen==GK1_SCREEN_SCENARIO_COUNTRIES);
 CK(press(&pad,a,GK1_PAD_A)&&press(&pad,a,GK1_PAD_MENU)); /* Genghis candidate, setup */
 CK(a->screen==GK1_SCREEN_SETUP_WIZARD);
 CK(press(&pad,a,GK1_PAD_A)); /* player count 1 */
 CK(press(&pad,a,GK1_PAD_A)); /* difficulty 1 */
 CK(press(&pad,a,GK1_PAD_A)); /* default Genghis */
 CK(a->setup_stage[1]==GK53_SETUP_ABILITIES);
 CK(press(&pad,a,GK1_PAD_DOWN)&&press(&pad,a,GK1_PAD_A)); /* manual */
 CK(a->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE && a->setup_ability_method[1]==2);
 CK(press(&pad,a,GK1_PAD_MENU)&&a->manual_bonus_active[1]==1);
 CK(press(&pad,a,GK1_PAD_B)&&a->manual_bonus[1][0][0]==10);
 CK(press(&pad,a,GK1_PAD_A)&&a->manual_bonus[1][0][0]==11);
 CK(press(&pad,a,GK1_PAD_RIGHT)&&press(&pad,a,GK1_PAD_DOWN));
 CK(a->manual_bonus_person[1]==1&&a->manual_bonus_ability[1]==1);
 for(unsigned n=0;n<8;n++)CK(press(&pad,a,GK1_PAD_B));
 CK(a->manual_bonus[1][1][1]==80 && gk55_bonus_spent(a,1)==91);
 CK(press(&pad,a,GK1_PAD_B)); /* 10 over the 100-point draft limit */
 CK(a->manual_bonus[1][1][1]==80 && gk55_bonus_spent(a,1)==91);
 for(unsigned n=0;n<9;n++)CK(press(&pad,a,GK1_PAD_A));
 CK(gk55_bonus_spent(a,1)==100 && a->manual_bonus[1][1][1]==89);
 CK(press(&pad,a,GK1_PAD_A));
 CK(gk55_bonus_spent(a,1)==100);
 CK(press(&pad,a,GK1_PAD_MENU)&&a->manual_bonus_active[1]==0);
 CK(a->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE && a->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 CK(a->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE);
 uint8_t b[GK55_PRE_SETUP_BYTES],old[GK53_PRE_SETUP_BYTES];
 CK(gk55_setup_encode(a,source,b,sizeof b)==GK1_STORE_OK);
 CK(gk53_setup_encode(a,source,old,sizeof old)==GK1_STORE_OK);
 gk1_app_init(loaded);CK(gk1_app_install_templates(loaded,source));
 CK(gk55_setup_decode(b,sizeof b,source,loaded)==GK1_STORE_OK);
 CK(loaded->manual_bonus[1][0][0]==11&&loaded->manual_bonus[1][1][1]==89);
 CK(gk55_bonus_spent(loaded,1)==100&&gk55_bonus_spent(loaded,0)==0);
 CK(loaded->rules_profile==a->rules_profile && loaded->setup_ability_method[1]==2);
 *before=*loaded;
 /* Duplicate/invalid source, per-stat overflow, forged totals, rejected atomically. */
 uint8_t corrupt[GK55_PRE_SETUP_BYTES];
 memcpy(corrupt,b,sizeof b);corrupt[GK53_PRE_SETUP_BYTES+15]=100;
 re_crc(corrupt,sizeof corrupt);
 CK(gk55_setup_decode(corrupt,sizeof corrupt,source,loaded)!=GK1_STORE_OK);
 CK(memcmp(before,loaded,sizeof *loaded)==0);
 memcpy(corrupt,b,sizeof b);corrupt[GK53_PRE_SETUP_BYTES+45+2]=99;
 re_crc(corrupt,sizeof corrupt);
 CK(gk55_setup_decode(corrupt,sizeof corrupt,source,loaded)!=GK1_STORE_OK);
 CK(memcmp(before,loaded,sizeof *loaded)==0);
 memcpy(corrupt,b,sizeof b);corrupt[GK53_PRE_SETUP_BYTES+75]=1;
 re_crc(corrupt,sizeof corrupt);
 CK(gk55_setup_decode(corrupt,sizeof corrupt,source,loaded)!=GK1_STORE_OK);
 CK(memcmp(before,loaded,sizeof *loaded)==0);
 CK(gk55_setup_decode(b,sizeof b-1,source,loaded)!=GK1_STORE_OK);
 CK(memcmp(before,loaded,sizeof *loaded)==0);
 memcpy(corrupt,b,sizeof b);corrupt[10]^=1;
 CK(gk55_setup_decode(corrupt,sizeof corrupt,source,loaded)!=GK1_STORE_OK);
 CK(memcmp(before,loaded,sizeof *loaded)==0);
 /* Hostile byte-for-byte bitflip: no single-bit corruption is accepted
  * or allowed to mutate previously loaded/verified source state. */
 for(size_t i=0;i<sizeof b;i++)for(unsigned bit=0;bit<8;bit++){
   memcpy(corrupt,b,sizeof b);corrupt[i]^=(uint8_t)(1u<<bit);
   CK(gk55_setup_decode(corrupt,sizeof corrupt,source,loaded)!=GK1_STORE_OK);
   CK(memcmp(before,loaded,sizeof *loaded)==0);
 }
 /* Legacy source save DOES NOT inherit stale bonus values. */
 CK(gk55_setup_decode(old,sizeof old,source,loaded)==GK1_STORE_OK);
 CK(gk55_bonus_spent(loaded,1)==0&&loaded->manual_bonus_active[1]==0);
 /* No split project; controller first selected both original worlds. */
 CK(a->worlds[0].country_count==14&&a->worlds[1].country_count==27);
 CK(a->setup_rulers[1][0]==0);
 /* Backing out of a manual allocation resets draft, so changing to AUTO
  * cannot leave a stale manual bonus that later blocks saving. */
 CK(press(&pad,a,GK1_PAD_B));
 CK(a->setup_stage[1]==GK53_SETUP_ABILITIES && gk55_bonus_spent(a,1)==0);
 CK(gk55_setup_encode(a,source,b,sizeof b)==GK1_STORE_OK);
 puts("EXEC0055 TESTED_PORT_LINUX six-ability / five-character native controller manual bonus draft, 100-point cap, backward-compatible source identity save and hostile negative rollback PASS");
 printf("assertions=%u\n",checks);
 free(a);free(loaded);free(before);free(source);return 0;
}
