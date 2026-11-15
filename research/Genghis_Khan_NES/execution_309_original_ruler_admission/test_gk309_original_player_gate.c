#include "gk162_original_entry.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks=0;
#define CK(x) do{checks++;if(!(x)){fprintf(stderr,"EXEC0309 FAIL %d: %s\n",__LINE__,#x);exit(2);}}while(0)
static int permitted(unsigned scenario,unsigned country){
 if(scenario==0u)return country==0u;
 return country==0u||country==6u||country==10u||country==18u;
}
int main(int argc,char**argv){
 CK(argc==2);
 Gk1TemplateDataset *data=malloc(sizeof *data);
 Gk1App *app=calloc(1,sizeof *app),*before=malloc(sizeof *before);
 CK(data&&app&&before);
 CK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 gk1_app_init(app);
 CK(gk1_app_install_templates(app,data));
 CK(app->world_ready[0]&&app->world_ready[1]);
 for(unsigned w=0;w<2u;w++){
  unsigned total=w?27u:14u;
  Gk77Profile profile=w?GK77_ARCHIVED_WORLD_SAVERAM:GK77_FRESH_MONGOL_WORKRAM;
  for(unsigned c=0;c<total;c++){
   CK(!!gk53_setup_allowed_lord((uint8_t)w,(uint8_t)c)==permitted(w,c));
   *before=*app;
   Gk162EntryStatus r=gk162_enter_original_checkpoint(app,argv[1],
     "/private-file-does-not-exist",profile,(uint8_t)c);
   CK(r==(permitted(w,c)?GK162_ENTRY_SOURCE:GK162_ENTRY_TEMPLATE));
   CK(memcmp(before,app,sizeof *app)==0);
  }
 }
 /* A hostile caller cannot bypass the source entry function by fabricating
  * a copied Gk79Checkpoint marked source_verified. Gate must precede all
  * source and checkpoint parsing; the original app is untouched. */
 for(unsigned w=0;w<2u;w++){
  Gk79Checkpoint *cp=calloc(1,sizeof *cp);
  CK(cp!=NULL);
  cp->valid=1u;cp->source_verified=1u;
  cp->profile=(uint8_t)(w?GK77_ARCHIVED_WORLD_SAVERAM:GK77_FRESH_MONGOL_WORKRAM);
  for(unsigned c=0;c<(w?27u:14u);c++)if(!permitted(w,c)){
   cp->player_index=(uint8_t)c;
   *before=*app;
   CK(gk162_commit_verified_checkpoint(app,argv[1],cp)==GK162_ENTRY_TEMPLATE);
   CK(!memcmp(before,app,sizeof *app));
  }
  free(cp);
 }
 /* Explicit invalid profile and out-of-range cannot create a player turn. */
 *before=*app;
 CK(gk162_enter_original_checkpoint(app,argv[1],"/missing",
    (Gk77Profile)0u,0u)==GK162_ENTRY_ARGUMENT);
 CK(memcmp(before,app,sizeof *app)==0);
 CK(!gk53_setup_allowed_lord(0u,14u));
 CK(!gk53_setup_allowed_lord(1u,27u));
 printf("EXEC0309 Original human ruler selector: %u/ %u exhaustive 14+27 tests PASS; no stock Spring generated\n",checks,checks);
 free(before);free(app);free(data);return 0;
}
