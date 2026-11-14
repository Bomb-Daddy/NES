/* EXEC0275: exact captured original fresh-reset setup compatibility.
 * This is a negative admission gate, NOT independent original NES setup/RNG. */
#include "gk166_fresh_entry.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks;
#define CK(c) do { checks++; if(!(c)){fprintf(stderr,"GK275 failed %d: %s\n",__LINE__,#c);return 2;} } while(0)
static Gk1App *app,*before;

static void exact_known_wizard(void){
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_player_count[0]=1u;
 app->setup_player_slot[0]=1u;
 app->setup_candidate[0]=0u;
 app->setup_rulers[0][0]=0u;
 app->setup_difficulty[0]=1u;
 app->setup_ability_method[0]=1u; /* automatic ability selection */
 app->manual_bonus_active[0]=0u;
}
static int denied_unchanged(const char *csv,const char *fresh){
 memcpy(before,app,sizeof *app);
 Gk166Result r=gk166_enter_original_fresh_json(app,csv,fresh,0u);
 CK(r==GK166_TEMPLATE);
 CK(memcmp(before,app,sizeof *app)==0);
 return 0;
}
int main(int argc,char **argv){
 CK(argc==2||argc==3);
 app=calloc(1,sizeof *app);before=malloc(sizeof *before);
 Gk1TemplateDataset *templates=malloc(sizeof *templates);
 CK(app&&before&&templates);
 gk1_app_init(app);
 CK(gk1_template_load(argv[1],templates)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(app,templates)==1);
 app->rules_profile=0u;
 const char *fresh=(argc==3?argv[2]:"/no-private-original-fresh-source-gk275");
 /* Control: a normal original checkpoint continuation remains legal without
  * first falsely claiming the source was generated from the port wizard. */
 memcpy(before,app,sizeof *app);
 if(argc==3){
 CK(gk166_enter_original_fresh_json(app,argv[1],fresh,0u)==GK166_OK);
 CK(app->stock_checkpoint.source_verified==1u);
 CK(app->stock_checkpoint.rate==121u && app->stock_checkpoint.orders.remaining==3u);
 CK(memcmp(app->worlds,before->worlds,sizeof app->worlds)==0);
 memcpy(app,before,sizeof *app);
 exact_known_wizard();
 CK(gk166_enter_original_fresh_json(app,argv[1],fresh,0u)==GK166_OK);
 CK(app->stock_checkpoint.source_verified==1u);
 memcpy(app,before,sizeof *app);
 }
 exact_known_wizard();app->setup_difficulty[0]=2u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->setup_difficulty[0]=5u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->setup_difficulty[0]=1u;app->setup_ability_method[0]=2u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->setup_ability_method[0]=1u;app->setup_candidate[0]=1u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->setup_candidate[0]=0u;app->setup_rulers[0][0]=1u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->setup_rulers[0][0]=0u;app->setup_player_count[0]=2u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->setup_player_count[0]=1u;app->setup_player_slot[0]=0u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->setup_player_slot[0]=1u;app->manual_bonus_active[0]=1u;
 CK(denied_unchanged(argv[1],fresh)==0);
 app->manual_bonus_active[0]=0u;app->manual_bonus[0][0][0]=1u;
 CK(denied_unchanged(argv[1],fresh)==0);
 /* A corrupted pre-wizard marker is NOT a valid observed setup provenance. */
 app->manual_bonus[0][0][0]=0u;
 app->setup_stage[0]=250u;
 CK(denied_unchanged(argv[1],fresh)==0);
 printf("EXEC0275 %u assertions PASS: fresh-workRAM source handling, known choice mismatch rejection, atomic rollback; private source mode=%s; no independent first-Spring RNG\n",checks,argc==3?"authenticated":"not run");
 free(templates);free(before);free(app);return 0;
}
