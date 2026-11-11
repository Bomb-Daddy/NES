#include "gk199_classic_tax_undo.h"
#include "gk193_auto_source_entry.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CK(x) do{if(!(x)){fprintf(stderr,"GK199 FAIL at line %d: %s\n",__LINE__,#x);return 1;}n++;}while(0)
static int private_case(const char *csv,const char *file,Gk77Profile profile,unsigned *count){
 Gk1TemplateDataset *ds=calloc(1,sizeof *ds);
 Gk1App *app=calloc(1,sizeof *app);
 if(!ds||!app)return 0;
 if(gk1_template_load(csv,ds)!=GK1_LOAD_OK)return 0;
 gk1_app_init(app);
 if(!gk1_app_install_templates(app,ds))return 0;
 app->rules_profile=0;
 Gk77Profile detected=0;
 if(gk193_enter_original_auto(app,csv,file,0,&detected)!=GK193_OK||detected!=profile)return 0;
 if(gk187_open(&app->stock_checkpoint,&app->classic_assign)!=GK187_OK)return 0;
 Gk79Checkpoint original=app->stock_checkpoint;
 uint8_t starting=(uint8_t)app->classic_assign.mutable_world.countries[0].tax;
 uint8_t changed=starting==100u?99u:100u,selection=changed;
 if(gk187_set_tax(&app->classic_assign,&app->stock_checkpoint,changed)!=GK187_OK)return 0;
 if(gk187_transfer(&app->classic_assign,&app->stock_checkpoint,0u,4u,1u)!=GK187_OK)return 0;
 Gk187ClassicAssign before=app->classic_assign;
 if(gk199_undo_last_tax(&app->classic_assign,&app->stock_checkpoint,&selection)!=GK187_INVALID||
    memcmp(&before,&app->classic_assign,sizeof before)||selection!=changed)return 0;
 (*count)++;
 if(gk187_undo_last(&app->classic_assign,&app->stock_checkpoint)!=GK187_OK)return 0;
 if(gk199_undo_last_tax(&app->classic_assign,&app->stock_checkpoint,&selection)!=GK187_OK||
    selection!=starting||app->classic_assign.action_count!=0u||
    app->classic_assign.budget.remaining!=3u||
    !gk187_intact(&app->classic_assign,&app->stock_checkpoint))return 0;
 (*count)++;
 if(memcmp(&original,&app->stock_checkpoint,sizeof original))return 0;
 (*count)++;
 if(gk199_undo_last_tax(&app->classic_assign,&app->stock_checkpoint,&selection)!=GK187_NO_ACTION)return 0;
 (*count)++;
 if(gk187_set_tax(&app->classic_assign,&app->stock_checkpoint,changed)!=GK187_OK)return 0;
 if(gk187_set_tax(&app->classic_assign,&app->stock_checkpoint,starting)!=GK187_OK)return 0;
 if(gk187_set_tax(&app->classic_assign,&app->stock_checkpoint,changed)!=GK187_OK)return 0;
 if(gk199_undo_last_tax(&app->classic_assign,&app->stock_checkpoint,&selection)!=GK187_OK||
    app->classic_assign.action_count!=2u||app->classic_assign.budget.remaining!=1u||
    selection!=starting||!gk187_intact(&app->classic_assign,&app->stock_checkpoint))return 0;
 (*count)++;
 /* Genuine PASS is irreversible; no in-game tax rollback after ending. */
 if(gk187_pass(&app->classic_assign,&app->stock_checkpoint)!=GK187_OK)return 0;
 Gk187ClassicAssign after_pass=app->classic_assign;
 uint8_t prior_selection=selection;
 if(gk199_undo_last_tax(&app->classic_assign,&app->stock_checkpoint,&selection)!=GK187_EXHAUSTED ||
    selection!=prior_selection || memcmp(&after_pass,&app->classic_assign,sizeof after_pass))return 0;
 (*count)++;
 /* Tamper with derived state: no action, selection or source is altered. */
 app->classic_assign=after_pass;
 app->classic_assign.mutable_world.countries[0].gold++;
 Gk187ClassicAssign tampered=app->classic_assign;
 if(gk199_undo_last_tax(&app->classic_assign,&app->stock_checkpoint,&selection)!=GK187_SOURCE ||
    selection!=prior_selection || memcmp(&tampered,&app->classic_assign,sizeof tampered))return 0;
 (*count)++;
 if(memcmp(&original,&app->stock_checkpoint,sizeof original))return 0;
 (*count)++;
 free(app);free(ds);return 1;
}
int main(int argc,char **argv){
 unsigned n=0;
 Gk187ClassicAssign state={0};Gk79Checkpoint source={0};
 uint8_t selection=45u;
 CK(gk199_undo_last_tax(NULL,&source,&selection)==GK187_INVALID);
 CK(gk199_undo_last_tax(&state,NULL,&selection)==GK187_INVALID);
 CK(gk199_undo_last_tax(&state,&source,NULL)==GK187_INVALID);
 CK(gk199_undo_last_tax(&state,&source,&selection)==GK187_SOURCE);
 CK(selection==45u);
 CK(argc==1||argc==5);
 if(argc==5){
  CK(private_case(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM,&n));
  CK(private_case(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,&n));
  CK(private_case(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM,&n));
 }
 printf("GK199 authenticated Classic TAX Undo: %u assertions PASS; PORT-ONLY, not NES parity\n",n);
 return 0;
}
