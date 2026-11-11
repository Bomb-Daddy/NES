#include "gk183_checkpoint_isolation.h"
#include "gk162_original_entry.h"
#include "gk167_private_cold_save.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned assertions;
#define CHECK(c) do {assertions++; if(!(c)){fprintf(stderr,"GK183 FAIL %u at %d: %s\n",assertions,__LINE__,#c);return 2;}}while(0)
int main(int argc,char **argv){
 if(argc!=2&&argc!=4)return 3;
 Gk1App *a=calloc(1,sizeof *a),*prior=malloc(sizeof *prior);
 CHECK(a&&prior);gk1_app_init(a);
 Gk1TemplateDataset *templates=malloc(sizeof *templates);
 CHECK(templates);
 CHECK(gk1_verified_original_csv(argv[1]));
 CHECK(gk1_template_load(argv[1],templates)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(a,templates));
 CHECK(!gk183_classic_storage_isolated(NULL));
 CHECK(!gk183_classic_storage_isolated(a));
 a->rules_profile=0u;
 CHECK(gk183_classic_storage_isolated(a));
 /* Isolate each derived stage: previous code forgot the Year2 continuation. */
 for(unsigned stage=0;stage<6u;stage++){
  switch(stage){
   case 0: a->enhanced_assign.active=1u;break;
   case 1: a->enhanced_summer.active=1u;break;
   case 2: a->enhanced_fall.active=1u;break;
   case 3: a->enhanced_winter.active=1u;break;
   case 4: a->enhanced_year2.active=1u;break;
   case 5: a->enhanced_continuation.active=1u;break;
  }
  CHECK(!gk183_classic_storage_isolated(a));
  CHECK(gk167_save_private_new("/tmp/gk183_must_not_write.gk17",a)==GK167_INVALID);
  gk183_reset_checkpoint_derived(a);
  CHECK(gk183_classic_storage_isolated(a));
 }
 /* The reset invalidates the whole post-Spring lineage even when its
  * active bit is 0 but its stale receipt/data is nonzero. */
 a->enhanced_continuation.receipts[4].actions[0].quantity=17u;
 a->enhanced_continuation.mutable_world.countries[0].gold=555u;
 a->enhanced_summer.action_count=2u;
 a->enhanced_year2.budget.remaining=2u;
 a->stock_storage_cursor=1u;
 a->stock_storage_confirm=1u;
 a->stock_storage_request=2u;
 a->stock_checkpoint.year=1175u; /* negative/non-source witness */
 *prior=*a;
 Gk79Checkpoint *invalid=calloc(1,sizeof *invalid);CHECK(invalid);
 invalid->valid=0u;
 CHECK(gk162_commit_verified_checkpoint(a,argv[1],invalid)!=GK162_ENTRY_OK);
 CHECK(memcmp(a,prior,sizeof *a)==0);
 gk183_reset_checkpoint_derived(a);
 CHECK(gk183_classic_storage_isolated(a));
 Gk176EnhancedCycle empty={0};
 CHECK(memcmp(&empty,&a->enhanced_continuation,sizeof empty)==0);
 CHECK(a->enhanced_summer.action_count==0u);
 CHECK(a->enhanced_year2.budget.remaining==0u);
 CHECK(!a->stock_storage_cursor&&!a->stock_storage_confirm&&!a->stock_storage_request);
 CHECK(memcmp(a->worlds,prior->worlds,sizeof a->worlds)==0);
 CHECK(memcmp(&a->stock_checkpoint,&prior->stock_checkpoint,sizeof a->stock_checkpoint)==0);
 CHECK(a->rules_profile==prior->rules_profile);
 /* Owner-private positive mode runs actual authentic original sources if
  * supplied externally; they MUST NOT be included in public artifacts. */
 if(argc==4){
  gk1_app_init(a);CHECK(gk1_app_install_templates(a,templates));
  a->rules_profile=0u;
  CHECK(gk162_enter_original_checkpoint(a,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM,0)==GK162_ENTRY_OK);
  a->enhanced_continuation.active=1u;
  a->enhanced_continuation.receipts[5].actions[0].quantity=29u;
  a->stock_storage_request=2u;a->stock_storage_confirm=1u;
  CHECK(gk167_save_private_new(argv[3],a)==GK167_INVALID);
  CHECK(gk162_enter_original_checkpoint(a,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM,0)==GK162_ENTRY_OK);
  CHECK(gk183_classic_storage_isolated(a));
  CHECK(!a->stock_storage_request&&!a->stock_storage_confirm);
  CHECK(gk85_raw_source_intact(&a->stock_checkpoint));
 }
 free(invalid);free(prior);free(a);free(templates);
 printf("GK183 %u assertions PASS; private original source %s; NO new first Spring/Classic commands\n",assertions,argc==4?"verified":"not supplied");
 return 0;
}
