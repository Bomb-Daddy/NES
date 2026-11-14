/* EXEC0276: archived SaveRAM source-history admission must not impersonate
 * an unrelated modern wizard; fresh WorkRAM cross-entry bypass also barred.
 * Private source optional, no copyrighted RAM or ROM included. */
#include "gk164_mss_entry.h"
#include "gk162_original_entry.h"
#include "gk166_fresh_entry.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned assertions;
#define CK(x) do { assertions++; if(!(x)){fprintf(stderr,"EXEC0276 failed line %d: %s\n",__LINE__,#x);return 2;} }while(0)
static Gk1App *app,*before;
static int reject_archived(const char *csv,Gk77Profile p){
 memcpy(before,app,sizeof *app);
 CK(!gk162_original_setup_compatible(app,p));
 CK(gk164_enter_original_mss(app,csv,"/gk276-missing-original-Mesen.mss",p,0u)==GK164_TEMPLATE);
 CK(memcmp(before,app,sizeof *app)==0);
 CK(gk162_enter_original_checkpoint(app,csv,"/gk276-missing-original-records",p,0u)==GK162_ENTRY_TEMPLATE);
 CK(memcmp(before,app,sizeof *app)==0);
 return 0;
}
int main(int argc,char **argv){
 CK(argc==2);
 app=calloc(1,sizeof *app);before=malloc(sizeof *before);
 Gk1TemplateDataset *d=malloc(sizeof *d);CK(app&&before&&d);
 gk1_app_init(app);CK(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(app,d));app->rules_profile=0u;
 Gk77Profile profiles[]={GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
 for(unsigned i=0;i<2;i++){
  unsigned w=i;Gk77Profile p=profiles[i];
  CK(gk162_original_setup_compatible(app,p));
  memcpy(before,app,sizeof *app);
  CK(gk164_enter_original_mss(app,argv[1],"/gk276-missing-original-Mesen.mss",p,0u)==GK164_SOURCE);
  CK(memcmp(before,app,sizeof *app)==0);
  app->setup_stage[w]=GK53_SETUP_DIFFICULTY;
  CK(reject_archived(argv[1],p)==0);app->setup_stage[w]=GK53_SETUP_PLAYERS;
  app->setup_difficulty[w]=3u;
  CK(reject_archived(argv[1],p)==0);app->setup_difficulty[w]=1u;
  app->setup_candidate[w]=1u;
  CK(reject_archived(argv[1],p)==0);app->setup_candidate[w]=255u;
  app->manual_bonus[w][1][2]=2u;
  CK(reject_archived(argv[1],p)==0);app->manual_bonus[w][1][2]=0u;
  app->setup_rulers[w][0]=0u;
  CK(reject_archived(argv[1],p)==0);app->setup_rulers[w][0]=255u;
  app->setup_ability_method[w]=1u;
  CK(reject_archived(argv[1],p)==0);app->setup_ability_method[w]=0u;
  CK(gk162_original_setup_compatible(app,p));
 }
 /* Direct 83-byte fresh checkpoint entry had bypassed the prior EXEC0275
  * fresh JSON-only wizard gate. Shared source entry now refuses it early. */
 app->setup_stage[0]=GK53_SETUP_AWAIT_SOURCE;app->active_world=0u;
 app->setup_player_count[0]=1u;app->setup_player_slot[0]=1u;
 app->setup_candidate[0]=0u;app->setup_rulers[0][0]=0u;
 app->setup_ability_method[0]=1u;app->setup_difficulty[0]=2u;
 CK(!gk162_original_setup_compatible(app,GK77_FRESH_MONGOL_WORKRAM));
 memcpy(before,app,sizeof *app);
 CK(gk162_enter_original_checkpoint(app,argv[1],"/gk276-missing-original-records",GK77_FRESH_MONGOL_WORKRAM,0u)==GK162_ENTRY_TEMPLATE);
 CK(memcmp(before,app,sizeof *app)==0);
 CK(gk166_enter_original_fresh_json(app,argv[1],"/gk276-missing-original-fresh.json",0u)==GK166_TEMPLATE);
 CK(memcmp(before,app,sizeof *app)==0);
 app->setup_difficulty[0]=1u;
 CK(gk162_original_setup_compatible(app,GK77_FRESH_MONGOL_WORKRAM));
 CK(!gk162_original_setup_compatible(app,(Gk77Profile)99));
 CK(!gk162_original_setup_compatible(NULL,GK77_ARCHIVED_MONGOL_SAVERAM));
 printf("EXEC0276 %u source-free C11 assertions PASS: archived title-only setup, direct fresh mismatch, missing private input, rollback; no new first-Spring RNG\n",assertions);
 free(d);free(before);free(app);return 0;
}
