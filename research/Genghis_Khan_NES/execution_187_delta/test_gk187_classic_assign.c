#include "gk1_app_state.h"
#include "gk117_stock_pristine_gate.h"
#include "gk183_checkpoint_isolation.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static int assertions=0;
#define CHECK(x) do {assert(x);assertions++;} while(0)
static Gk1App *make(const Gk1TemplateDataset *ds){
 Gk1App *s=calloc(1,sizeof *s);CHECK(s!=NULL);
 gk1_app_init(s);CHECK(gk1_app_install_templates(s,ds));return s;
}
static void synthetic_negative(const Gk1TemplateDataset *ds){
 Gk1App *s=make(ds);
 Gk187ClassicAssign out={0};out.active=91;
 Gk187ClassicAssign before=out;
 CHECK(gk187_open(&s->stock_checkpoint,&out)==GK187_SOURCE);
 CHECK(!memcmp(&before,&out,sizeof out));
 CHECK(gk187_transfer(&out,&s->stock_checkpoint,0,4,1)==GK187_SOURCE);
 CHECK(!memcmp(&before,&out,sizeof out));
 CHECK(gk187_pass(&out,&s->stock_checkpoint)==GK187_SOURCE);
 CHECK(!memcmp(&before,&out,sizeof out));
 free(s);
}
static void case_test(const Gk1TemplateDataset *ds,const char *csv,
 const char *private_record,Gk77Profile p,unsigned gold){
 Gk1App *s=make(ds);
 unsigned world=p==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 CHECK(gk117_open_attested_original(csv,&s->worlds[world],private_record,
   p,0u,&s->stock_checkpoint)==GK117_OK);
 const Gk79Checkpoint original=s->stock_checkpoint;
 Gk187ClassicAssign c={0};
 CHECK(s->stock_checkpoint.world.countries[0].gold==gold);
 CHECK(gk187_open(&s->stock_checkpoint,&c)==GK187_OK);
 CHECK(gk187_intact(&c,&s->stock_checkpoint));
 uint16_t original_troops=original.world.countries[0].troops;
 uint16_t original_farm=original.world.countries[0].food_workers;
 CHECK(gk187_transfer(&c,&s->stock_checkpoint,0u,4u,1u)==GK187_OK);
 CHECK(c.action_count==1u&&c.budget.remaining==2u);
 CHECK(c.mutable_world.countries[0].troops==original_troops-1u);
 CHECK(c.mutable_world.countries[0].food_workers==original_farm+1u);
 CHECK(gk187_intact(&c,&s->stock_checkpoint));
 Gk187ClassicAssign snapshot=c;
 CHECK(gk187_transfer(&c,&s->stock_checkpoint,0u,0u,1u)==GK187_INVALID);
 CHECK(!memcmp(&snapshot,&c,sizeof c));
 CHECK(gk187_transfer(&c,&s->stock_checkpoint,0u,4u,1000u)==GK187_NO_WORKERS);
 CHECK(!memcmp(&snapshot,&c,sizeof c));
 CHECK(gk187_transfer(&c,&s->stock_checkpoint,0u,4u,1u)==GK187_OK);
 CHECK(c.budget.remaining==1u);
 CHECK(gk187_pass(&c,&s->stock_checkpoint)==GK187_OK);
 CHECK(c.budget.remaining==0u&&c.budget.ended==1u&&c.passed==1u);
 CHECK(gk187_intact(&c,&s->stock_checkpoint));
 snapshot=c;
 CHECK(gk187_transfer(&c,&s->stock_checkpoint,0u,4u,1u)==GK187_EXHAUSTED);
 CHECK(gk187_pass(&c,&s->stock_checkpoint)==GK187_EXHAUSTED);
 CHECK(!memcmp(&snapshot,&c,sizeof c));
 Gk187ClassicAssign forged=c;forged.mutable_world.countries[0].gold++;
 CHECK(!gk187_intact(&forged,&s->stock_checkpoint));
 snapshot=forged;
 CHECK(gk187_transfer(&forged,&s->stock_checkpoint,0u,4u,1u)==GK187_SOURCE);
 CHECK(!memcmp(&snapshot,&forged,sizeof forged));
 Gk79Checkpoint bad=original;bad.stock_raw[0][0]^=1u;
 CHECK(!gk187_intact(&c,&bad));
 CHECK(!memcmp(&original,&s->stock_checkpoint,sizeof original));
 /* Native controller actually navigates/executes Classic manual ASSIGN. */
 s->screen=GK1_SCREEN_STOCK_COMMANDS;s->rules_profile=0u;
 s->stock_command_cursor=1u;
 CHECK(gk1_app_input(s,GK1_INPUT_CONFIRM)==1);
 CHECK(s->screen==GK1_SCREEN_CLASSIC_ASSIGN);
 CHECK(s->classic_assign.active==1u);
 CHECK(gk1_app_input(s,GK1_INPUT_CONFIRM)==1);
 CHECK(s->classic_assign.action_count==1u&&s->classic_assign.budget.remaining==2u);
 CHECK(gk1_app_input(s,GK1_INPUT_CANCEL)==1);
 CHECK(s->screen==GK1_SCREEN_STOCK_COMMANDS);
 s->stock_command_cursor=12u;
 CHECK(gk1_app_input(s,GK1_INPUT_CONFIRM)==1);
 CHECK(s->stock_checkpoint.pending_pass==1u);
 CHECK(gk1_app_input(s,GK1_INPUT_CANCEL)==1);
 CHECK(s->stock_checkpoint.pending_pass==0u&&s->classic_assign.budget.remaining==2u);
 CHECK(gk1_app_input(s,GK1_INPUT_CONFIRM)==1);
 CHECK(gk1_app_input(s,GK1_INPUT_CONFIRM)==1);
 CHECK(s->classic_assign.passed&&s->classic_assign.budget.remaining==0u);
 CHECK(gk183_classic_storage_isolated(s)==0);
 CHECK(!memcmp(&original,&s->stock_checkpoint,sizeof original));
 /* A new attached source MUST clear the derived Classic mutable ledger. */
 gk183_reset_checkpoint_derived(s);
 CHECK(s->classic_assign.active==0u);
 CHECK(gk183_classic_storage_isolated(s)==1);
 free(s);
}
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 Gk1TemplateDataset *ds=malloc(sizeof *ds);CHECK(ds!=NULL);
 CHECK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 synthetic_negative(ds);
 if(argc==5){
  case_test(ds,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM,430u);
  case_test(ds,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,439u);
  case_test(ds,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM,193u);
  printf("EXEC0187: %d authentic/private assertions PASS; Classic controller ASSIGN manually derived, NOT original NES runtime parity\n",assertions);
 }else printf("EXEC0187: %d negative public assertions PASS; authentic positive cases private only\n",assertions);
 free(ds);return 0;
}
