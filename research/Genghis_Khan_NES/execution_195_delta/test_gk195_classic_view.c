#include "gk195_classic_view.h"
#include "gk193_auto_source_entry.h"
#include "gk189_classic_cold_save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"EXEC0195 failed %d: %s\n",__LINE__,#x);return 1;}checks++;}while(0)
static int case_private(const char *csv,const char *src,Gk77Profile expected){
 Gk1App *a=calloc(1,sizeof *a);CHECK(a!=NULL);
 gk1_app_init(a);a->rules_profile=0;
 Gk1TemplateDataset *ds=malloc(sizeof *ds);CHECK(ds!=NULL);
 CHECK(gk1_template_load(csv,ds)==GK1_LOAD_OK);
 CHECK(gk1_app_install_templates(a,ds));free(ds);
 Gk77Profile p=GK77_ARCHIVED_WORLD_SAVERAM;
 CHECK(gk193_enter_original_auto(a,csv,src,0,&p)==GK193_OK);
 CHECK(p==expected);
 Gk79Checkpoint source=a->stock_checkpoint;
 Gk195CountryView v={0};
 CHECK(gk195_classic_country_view(a,0,&v)==GK195_OK);
 CHECK(v.is_player==1u&&v.derived==0u&&v.remaining==3u);
 CHECK(!memcmp(&v.original,&v.current,sizeof v.original));
 CHECK(gk1_app_input(a,GK1_INPUT_SETUP)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->screen==GK1_SCREEN_CLASSIC_ASSIGN);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->classic_assign.action_count==1u);
 CHECK(gk195_classic_country_view(a,0,&v)==GK195_OK);
 CHECK(v.derived==1u&&v.actions==1u&&v.remaining==2u);
 CHECK(v.current.troops+1u==v.original.troops);
 CHECK(v.current.food_workers==v.original.food_workers+1u);
 CHECK(gk195_classic_country_view(a,1,&v)==GK195_OK);
 CHECK(v.derived==0u&&!memcmp(&v.original,&v.current,sizeof v.original));
 CHECK(!memcmp(&source.stock_raw,&a->stock_checkpoint.stock_raw,sizeof source.stock_raw));
 CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 a->stock_command_cursor=6u;
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
 CHECK(a->stock_checkpoint.view_index==a->stock_checkpoint.player_index);
 /* Classic VIEW A must consume the live Classic budget, not the original
  * checkpoint's independent 3 orders. Double confirmation and B rollback. */
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->stock_checkpoint.pending_pass==1u);
 CHECK(a->classic_assign.budget.remaining==2u);
 CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 CHECK(a->stock_checkpoint.pending_pass==0u);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->classic_assign.passed==1u&&a->classic_assign.budget.remaining==0u);
 CHECK(a->stock_checkpoint.orders.remaining==3u);
 CHECK(gk195_classic_country_view(a,0,&v)==GK195_OK);
 CHECK(v.passed==1u&&v.remaining==0u);
 CHECK(!memcmp(&source.stock_raw,&a->stock_checkpoint.stock_raw,sizeof source.stock_raw));
 Gk1App *bad=malloc(sizeof *bad);CHECK(bad!=NULL);*bad=*a;
 bad->classic_assign.mutable_world.countries[0].gold++;
 Gk195CountryView prior=v;
 CHECK(gk195_classic_country_view(bad,0,&v)==GK195_SOURCE);
 CHECK(!memcmp(&prior,&v,sizeof v));
 bad->screen=GK1_SCREEN_STOCK_CONTINUE;
 CHECK(gk1_app_input(bad,GK1_INPUT_CONFIRM)==0);
 CHECK(bad->stock_checkpoint.orders.remaining==3u);
 *bad=*a;bad->stock_checkpoint.stock_raw[0][0]^=1u;
 CHECK(gk195_classic_country_view(bad,0,&v)==GK195_SOURCE);
 *bad=*a;bad->rules_profile=1u;
 CHECK(gk195_classic_country_view(bad,0,&v)==GK195_SOURCE);
 CHECK(gk195_classic_country_view(a,255u,&v)==GK195_INVALID);
 free(bad);free(a);return 0;
}
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 CHECK(gk195_classic_country_view(NULL,0,NULL)==GK195_INVALID);
 Gk1App *s=calloc(1,sizeof *s);CHECK(s);
 Gk195CountryView v={0};
 CHECK(gk195_classic_country_view(s,0,&v)==GK195_SOURCE);
 Gk195CountryView before=v;
 CHECK(gk195_classic_country_view(s,0,&v)==GK195_SOURCE);
 CHECK(!memcmp(&before,&v,sizeof v));free(s);
 if(argc==5){
  CHECK(case_private(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM)==0);
  CHECK(case_private(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM)==0);
  CHECK(case_private(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM)==0);
 }
 printf("EXEC0195 %u assertions PASS; new source-attested Classic mutable country VIEW and proper Pass routing, NOT NES startup/RNG parity\n",checks);
 return 0;
}
