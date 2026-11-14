/* EXEC0277: initial title-state entry must be genuinely untouched.
 * Original fresh WorkRAM is a historical Mesen observation, not a license
 * to relabel a modified setup as an NES-derived first Spring. */
#include "gk162_original_entry.h"
#include "gk166_fresh_entry.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned assertions;
#define CK(x) do { ++assertions; if(!(x)) {fprintf(stderr,"GK277 failed at %d: %s\n",__LINE__,#x);return 2;} } while(0)
static Gk1App *a,*before;
static int rejected(const char *csv){
 memcpy(before,a,sizeof *a);
 CK(!gk162_original_setup_compatible(a,GK77_FRESH_MONGOL_WORKRAM));
 CK(gk162_enter_original_checkpoint(a,csv,"/gk277-no-stock-records",GK77_FRESH_MONGOL_WORKRAM,0u)==GK162_ENTRY_TEMPLATE);
 CK(memcmp(before,a,sizeof *a)==0);
 CK(gk166_enter_original_fresh_json(a,csv,"/gk277-no-source.json",0u)==GK166_TEMPLATE);
 CK(memcmp(before,a,sizeof *a)==0);
 return 0;
}
int main(int argc,char **argv){
 CK(argc==2);
 a=calloc(1,sizeof *a);before=malloc(sizeof *before);
 Gk1TemplateDataset *d=malloc(sizeof *d);CK(a&&before&&d);
 gk1_app_init(a);CK(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(a,d));a->rules_profile=0;
 CK(gk162_original_setup_compatible(a,GK77_FRESH_MONGOL_WORKRAM));
 memcpy(before,a,sizeof *a);
 CK(gk162_enter_original_checkpoint(a,argv[1],"/gk277-no-stock-records",GK77_FRESH_MONGOL_WORKRAM,0u)==GK162_ENTRY_SOURCE);
 CK(memcmp(before,a,sizeof *a)==0);
 /* The historical code blindly accepted ANY state at PLAYERS, including
  * impossible setups with different selected lords, difficulty and bonuses. */
 a->setup_candidate[0]=1u;CK(!rejected(argv[1]));a->setup_candidate[0]=255u;
 a->setup_difficulty[0]=5u;CK(!rejected(argv[1]));a->setup_difficulty[0]=1u;
 a->setup_rulers[0][0]=3u;CK(!rejected(argv[1]));a->setup_rulers[0][0]=255u;
 a->setup_player_slot[0]=1u;CK(!rejected(argv[1]));a->setup_player_slot[0]=0u;
 a->setup_ability_method[0]=2u;CK(!rejected(argv[1]));a->setup_ability_method[0]=0u;
 a->setup_method_cursor[0]=1u;CK(!rejected(argv[1]));a->setup_method_cursor[0]=0u;
 a->manual_bonus[0][4][5]=1u;CK(!rejected(argv[1]));a->manual_bonus[0][4][5]=0u;
 a->manual_bonus_person[0]=3u;CK(!rejected(argv[1]));a->manual_bonus_person[0]=0u;
 a->manual_bonus_ability[0]=4u;CK(!rejected(argv[1]));a->manual_bonus_ability[0]=0u;
 a->active_world=1u;CK(!rejected(argv[1]));a->active_world=0u;
 CK(gk162_original_setup_compatible(a,GK77_FRESH_MONGOL_WORKRAM));
 /* Source-compatible setup wizard path is still allowed, but NOT claimed
  * as self-generated NES setup or actual RNG completion. */
 a->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;
 a->setup_player_slot[0]=1u;a->setup_candidate[0]=0u;
 a->setup_rulers[0][0]=0u;a->setup_ability_method[0]=1u;
 CK(gk162_original_setup_compatible(a,GK77_FRESH_MONGOL_WORKRAM));
 a->setup_method_cursor[0]=1u;CK(!rejected(argv[1]));a->setup_method_cursor[0]=0u;
 a->manual_bonus_person[0]=1u;CK(!rejected(argv[1]));a->manual_bonus_person[0]=0u;
 a->manual_bonus_ability[0]=1u;CK(!rejected(argv[1]));a->manual_bonus_ability[0]=0u;
 CK(gk162_original_setup_compatible(a,GK77_FRESH_MONGOL_WORKRAM));
 printf("GK1 EXEC0277 %u assertions PASS: untouched title and known-choice fresh provenance; no original stock generated\n",assertions);
 free(d);free(before);free(a);return 0;
}
