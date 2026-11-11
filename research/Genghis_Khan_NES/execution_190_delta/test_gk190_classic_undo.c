/* An exact-source-bound, manual-derived PORT-ONLY undo/cold-load test.
 * Never distribute authentic original RAM record fixtures or claim NES parity. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk189_classic_cold_save.h"
#include "gk162_original_entry.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
static unsigned checks;
#define CHECK(v) do { assert(v);checks++; } while(0)
static void no_source(const Gk1TemplateDataset *ds){
 Gk1App *a=calloc(1,sizeof *a);CHECK(a!=NULL);
 gk1_app_init(a);CHECK(gk1_app_install_templates(a,ds));
 Gk187ClassicAssign s={0},previous={0};
 CHECK(gk187_undo_last(&s,&a->stock_checkpoint)==GK187_SOURCE);
 CHECK(!memcmp(&s,&previous,sizeof s));
 s.active=1u;previous=s;
 CHECK(gk187_undo_last(&s,&a->stock_checkpoint)==GK187_SOURCE);
 CHECK(!memcmp(&s,&previous,sizeof s));
 a->screen=GK1_SCREEN_CLASSIC_ASSIGN;
 CHECK(gk1_app_input(a,GK1_INPUT_DRAFT_UNDO_TEN)==1);
 CHECK(!memcmp(&s,&previous,sizeof s));
 CHECK(!a->classic_assign.active);
 free(a);
}
static void authentic(const Gk1TemplateDataset *ds,const char *csv,
 const char *raw,Gk77Profile p){
 Gk1App *a=calloc(1,sizeof *a),*load=calloc(1,sizeof *load);
 CHECK(a&&load);
 gk1_app_init(a);gk1_app_init(load);
 CHECK(gk1_app_install_templates(a,ds));
 CHECK(gk1_app_install_templates(load,ds));
 CHECK(gk162_enter_original_checkpoint(a,csv,raw,p,0u)==GK162_ENTRY_OK);
 a->rules_profile=0u;
 CHECK(gk187_open(&a->stock_checkpoint,&a->classic_assign)==GK187_OK);
 Gk1App *unchanged=malloc(sizeof *unchanged);CHECK(unchanged!=NULL);
 *unchanged=*a;
 CHECK(gk187_undo_last(&a->classic_assign,&a->stock_checkpoint)==GK187_NO_ACTION);
 CHECK(!memcmp(unchanged,a,sizeof *a));
 Gk1CountryTemplate original=a->stock_checkpoint.world.countries[0];
 CHECK(gk187_transfer(&a->classic_assign,&a->stock_checkpoint,0u,4u,1u)==GK187_OK);
 CHECK(gk187_transfer(&a->classic_assign,&a->stock_checkpoint,0u,4u,2u)==GK187_OK);
 Gk187ClassicAssign two=a->classic_assign;
 CHECK(gk187_undo_last(&a->classic_assign,&a->stock_checkpoint)==GK187_OK);
 CHECK(a->classic_assign.budget.remaining==2u);
 CHECK(a->classic_assign.action_count==1u);
 CHECK(a->classic_assign.mutable_world.countries[0].troops==original.troops-1u);
 CHECK(a->classic_assign.mutable_world.countries[0].food_workers==original.food_workers+1u);
 CHECK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 CHECK(gk187_transfer(&a->classic_assign,&a->stock_checkpoint,0u,4u,2u)==GK187_OK);
 CHECK(!memcmp(&a->classic_assign,&two,sizeof two));
 /* An invalid mutation must not modify any byte on attempted undo. */
 a->classic_assign.mutable_world.countries[0].gold++;
 Gk187ClassicAssign forged=a->classic_assign;
 CHECK(gk187_undo_last(&a->classic_assign,&a->stock_checkpoint)==GK187_SOURCE);
 CHECK(!memcmp(&forged,&a->classic_assign,sizeof forged));
 a->classic_assign=two;
 /* Controller undo request, then save, and load in another live C11 app. */
 a->screen=GK1_SCREEN_CLASSIC_ASSIGN;
 CHECK(gk1_app_input(a,GK1_INPUT_DRAFT_UNDO_TEN)==1);
 CHECK(a->classic_assign.action_count==1u);
 CHECK(a->classic_assign.budget.remaining==2u);
 CHECK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 char path[]="/tmp/gk190-classic-undo-XXXXXX";
#ifndef _WIN32
 int fd=mkstemp(path);CHECK(fd>=0);CHECK(close(fd)==0);CHECK(unlink(path)==0);
#endif
 CHECK(gk189_save_new(path,a)==GK189_OK);
 CHECK(gk189_cold_load(path,csv,load)==GK189_OK);
 CHECK(load->classic_assign.action_count==1u&&load->classic_assign.budget.remaining==2u);
 CHECK(gk187_intact(&load->classic_assign,&load->stock_checkpoint));
 CHECK(!memcmp(&a->classic_assign,&load->classic_assign,sizeof a->classic_assign));
 CHECK(gk187_pass(&load->classic_assign,&load->stock_checkpoint)==GK187_OK);
 Gk187ClassicAssign passed=load->classic_assign;
 CHECK(gk187_undo_last(&load->classic_assign,&load->stock_checkpoint)==GK187_EXHAUSTED);
 CHECK(!memcmp(&load->classic_assign,&passed,sizeof passed));
 CHECK(remove(path)==0);
 free(unchanged);free(load);free(a);
}
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 Gk1TemplateDataset *ds=calloc(1,sizeof *ds);CHECK(ds!=NULL);
 CHECK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 no_source(ds);
 if(argc==5){
  authentic(ds,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
  authentic(ds,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
  authentic(ds,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 }
 printf("EXEC0190 %u %s undo/refusal/cold-save assertions PASS; PORT-ONLY, NOT NES PARITY\n",checks,argc==5?"THREE AUTHENTIC PROFILES":"PUBLIC NEGATIVES");
 free(ds);return 0;
}
