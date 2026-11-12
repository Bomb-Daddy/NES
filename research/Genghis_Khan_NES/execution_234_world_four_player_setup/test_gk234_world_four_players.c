/* EXEC0234: real integrated four-player World campaign setup with source-gated
 * refusal. Uses same native C11 Gk1App and gamepad router, not a mock game.
 * Manual-derived controller choices only; NO original NES Spring generation. */
#include "gk1_manual_setup_store.h"
#include "gk1_pad_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned assertions;
#define REQUIRE(x) do { assertions++;if(!(x)){fprintf(stderr,"GK234 FAIL line %d: %s\n",__LINE__,#x);return 1;} } while(0)
static int tap(Gk1PadRouter *router,Gk1App *game,uint16_t button){
 Gk1PadSample off={0},on={0};Gk1PadAction actions[8];on.buttons=button;
 (void)gk1_pad_step(router,&off,16,actions,8);
 size_t n=gk1_pad_step(router,&on,16,actions,8);
 for(size_t i=0;i<n;i++){
  Gk1Input input=0;
  switch(actions[i]){
   case GK1_PAD_ACT_A: input=GK1_INPUT_CONFIRM;break;
   case GK1_PAD_ACT_B: input=GK1_INPUT_CANCEL;break;
   case GK1_PAD_ACT_UP:input=GK1_INPUT_UP;break;
   case GK1_PAD_ACT_DOWN:input=GK1_INPUT_DOWN;break;
   case GK1_PAD_ACT_LEFT:input=GK1_INPUT_LEFT;break;
   case GK1_PAD_ACT_RIGHT:input=GK1_INPUT_RIGHT;break;
   case GK1_PAD_ACT_MENU:input=GK1_INPUT_SETUP;break;
   default:break;
  }
  if(input&&!gk1_app_input(game,input))return 0;
 }
 (void)gk1_pad_step(router,&off,16,actions,8);return 1;
}
int main(int argc,char **argv){
 if(argc!=2)return 3;
 Gk1TemplateDataset *data=malloc(sizeof *data);
 Gk1App *game=malloc(sizeof *game),*reloaded=malloc(sizeof *reloaded);
 Gk1App *snapshot=malloc(sizeof *snapshot);
 REQUIRE(data&&game&&reloaded&&snapshot);
 REQUIRE(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 gk1_app_init(game);REQUIRE(gk1_app_install_templates(game,data));
 Gk1PadRouter router;gk1_pad_init(&router);
 REQUIRE(tap(&router,game,GK1_PAD_A)); /* title -> campaign */
 REQUIRE(tap(&router,game,GK1_PAD_LEFT)); /* real Classic, not Enhanced */
 REQUIRE(game->rules_profile==0);
 REQUIRE(tap(&router,game,GK1_PAD_DOWN));
 REQUIRE(tap(&router,game,GK1_PAD_A)); /* World scenario */
 REQUIRE(game->screen==GK1_SCREEN_SCENARIO_COUNTRIES&&game->active_world==1);
 REQUIRE(game->worlds[1].country_count==27&&game->worlds[0].country_count==14);
 REQUIRE(tap(&router,game,GK1_PAD_A)); /* choose eligible World country 1 */
 REQUIRE(tap(&router,game,GK1_PAD_MENU));
 REQUIRE(game->screen==GK1_SCREEN_SETUP_WIZARD);
 for(unsigned i=0;i<3;i++)REQUIRE(tap(&router,game,GK1_PAD_UP));
 REQUIRE(game->setup_player_count[1]==4);
 REQUIRE(tap(&router,game,GK1_PAD_A));
 REQUIRE(game->setup_stage[1]==GK53_SETUP_DIFFICULTY);
 for(unsigned i=0;i<4;i++)REQUIRE(tap(&router,game,GK1_PAD_RIGHT));
 REQUIRE(game->setup_difficulty[1]==5);
 REQUIRE(tap(&router,game,GK1_PAD_A));
 REQUIRE(game->setup_stage[1]==GK53_SETUP_LORD);
 const unsigned original_lords[4]={0,6,10,18};
 for(unsigned i=0;i<4;i++){
  REQUIRE(game->country_cursor[1]==original_lords[i]);
  REQUIRE(tap(&router,game,GK1_PAD_A));
  REQUIRE(game->setup_rulers[1][i]==original_lords[i]);
 }
 REQUIRE(game->setup_stage[1]==GK53_SETUP_ABILITIES);
 REQUIRE(tap(&router,game,GK1_PAD_B));
 REQUIRE(game->setup_stage[1]==GK53_SETUP_LORD&&game->setup_player_slot[1]==3);
 REQUIRE(game->setup_rulers[1][3]==255);
 REQUIRE(tap(&router,game,GK1_PAD_A));
 REQUIRE(game->setup_rulers[1][3]==18&&game->setup_stage[1]==GK53_SETUP_ABILITIES);
 REQUIRE(tap(&router,game,GK1_PAD_A)); /* automatic abilities */
 REQUIRE(game->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE);
 REQUIRE(game->setup_ability_method[1]==1);
 REQUIRE(tap(&router,game,GK1_PAD_A)); /* Classic must NOT enter false Spring */
 REQUIRE(game->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&game->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 REQUIRE(game->screen==GK1_SCREEN_SETUP_WIZARD);
 uint8_t encoded[GK53_PRE_SETUP_BYTES];
 REQUIRE(gk53_setup_encode(game,data,encoded,sizeof encoded)==GK1_STORE_OK);
 gk1_app_init(reloaded);REQUIRE(gk1_app_install_templates(reloaded,data));
 REQUIRE(gk53_setup_decode(encoded,sizeof encoded,data,reloaded)==GK1_STORE_OK);
 REQUIRE(reloaded->setup_player_count[1]==4&&reloaded->setup_difficulty[1]==5);
 for(unsigned i=0;i<4;i++)REQUIRE(reloaded->setup_rulers[1][i]==original_lords[i]);
 REQUIRE(reloaded->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE&&reloaded->setup_stage[1]==GK53_SETUP_AWAIT_SOURCE);
 /* Reject deliberate World setup corruption without modifying either world. */
 *snapshot=*reloaded;
 for(size_t k=0;k<sizeof encoded;k++){
  uint8_t old=encoded[k];encoded[k]^=1u;
  REQUIRE(gk53_setup_decode(encoded,sizeof encoded,data,reloaded)!=GK1_STORE_OK);
  REQUIRE(memcmp(snapshot,reloaded,sizeof *reloaded)==0);
  encoded[k]=old;
 }
 printf("EXEC0234 World four-player controller setup, no fake Spring, cold-load and hostile rollback %u assertions PASS\n",assertions);
 free(snapshot);free(reloaded);free(game);free(data);return 0;
}
