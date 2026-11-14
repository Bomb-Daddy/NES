#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/* EXEC0270: moving among authenticated original-stock countries or changing
 * the read-only VIEW must DISARM a pending Pass confirmation. Otherwise a
 * delayed second A can unexpectedly end the port's three-order Spring.
 * Private positive test input: exact original-stock 14x83 / 27x83 country
 * records loaded through production SHA verification. No ROM or raw RAM
 * is embedded, printed, or shipped in this copyright-clean regression.
 */
#include "gk193_auto_source_entry.h"
#include "gk1_app_state.h"
#include "gk188_classic_receipt.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef NDEBUG
#undef NDEBUG
#endif
static unsigned checks;
#define CK(x) do { if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); exit(3);} ++checks; } while(0)
static void source_still_intact(const Gk1App *a,const Gk79Checkpoint *before){
 CK(gk85_raw_source_intact(&a->stock_checkpoint));
 CK(gk88_stock_projection_intact(&a->stock_checkpoint));
 CK(memcmp(a->stock_checkpoint.stock_raw,before->stock_raw,sizeof before->stock_raw)==0);
 CK(memcmp(a->stock_checkpoint.raw_sha256,before->raw_sha256,32u)==0);
 CK(memcmp(&a->stock_checkpoint.orders,&before->orders,sizeof before->orders)==0);
 CK(a->stock_checkpoint.orders.remaining==3u);
 CK(a->stock_checkpoint.year==before->year);
 CK(a->stock_checkpoint.rate==before->rate);
 CK(a->stock_checkpoint.world.country_count==before->world.country_count);
}
static void check_profile(const char *csv,const char *source,Gk77Profile expected){
 Gk1App *a=calloc(1,sizeof *a);CK(a!=NULL);
 Gk1TemplateDataset *d=malloc(sizeof *d);CK(d!=NULL);
 gk1_app_init(a);CK(gk1_template_load(csv,d)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(a,d)==1);free(d);
 a->rules_profile=0u;
 Gk77Profile found=0;
 CK(gk193_enter_original_auto(a,csv,source,0u,&found)==GK193_OK);
 CK(found==expected);
 CK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
 CK(a->stock_checkpoint.source_verified==1u);
 Gk79Checkpoint *before=malloc(sizeof *before);CK(before!=NULL);
 *before=a->stock_checkpoint;
 const Gk1Input moves[]={GK1_INPUT_UP,GK1_INPUT_DOWN,
  GK1_INPUT_LEFT,GK1_INPUT_RIGHT,GK1_INPUT_LABOR_PLAN};
 for(unsigned n=0;n<sizeof moves/sizeof moves[0];n++){
  CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1); /* First A arms Pass. */
  CK(a->stock_checkpoint.pending_pass==1u);
  CK(a->classic_assign.active==0u);
  CK(gk1_app_input(a,moves[n])==1);
  CK(a->stock_checkpoint.pending_pass==0u); /* No surprise second-A commit. */
  CK(a->classic_assign.active==0u);
  source_still_intact(a,before);
  CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
  CK(a->stock_checkpoint.pending_pass==1u);
  CK(a->classic_assign.active==0u);
  CK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
  CK(a->stock_checkpoint.pending_pass==0u);
  source_still_intact(a,before);
 }
 /* All navigation changes only VIEW state, even after one genuine port Tax
  * transaction, while the original checkpoint remains byte-identical. */
 CK(gk1_app_input(a,GK1_INPUT_SETUP)==1);
 CK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->screen==GK1_SCREEN_CLASSIC_TAX);
 CK(gk1_app_input(a,GK1_INPUT_UP)==1);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.action_count==1u);
 CK(a->classic_assign.budget.remaining==2u);
 CK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 for(int j=0;j<6;j++)CK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->stock_checkpoint.pending_pass==1u);
 CK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CK(a->stock_checkpoint.pending_pass==0u);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->stock_checkpoint.pending_pass==1u);
 CK(a->classic_assign.passed==0u);
 CK(a->classic_assign.budget.remaining==2u);
 source_still_intact(a,before);
 /* A/A is still the correct explicit Pass confirmation after re-arming. */
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.passed==1u);
 CK(a->classic_assign.budget.remaining==0u);
 CK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 source_still_intact(a,before);
 printf("EXEC0270 ORIGINAL %s: View A/move/A cancel; Tax/move; explicit Pass; immutable source PASS\n",gk77_profile_name(expected));
 free(before);free(a);
}
static void check_public_negative(void){
 Gk1App *a=calloc(1,sizeof *a);CK(a!=NULL);
 Gk1App *copy=malloc(sizeof *copy);CK(copy!=NULL);
 gk1_app_init(a);a->rules_profile=0u;
 a->screen=GK1_SCREEN_STOCK_CONTINUE;
 for(int state=0;state<2;state++){
  if(state)a->stock_checkpoint.valid=1u;
  for(int cmd=1;cmd<=10;cmd++){
   memcpy(copy,a,sizeof *copy);
   CK(gk1_app_input(a,(Gk1Input)cmd)==0);
   CK(memcmp(a,copy,sizeof *a)==0);
  }
 }
 free(copy);free(a);
}
int main(int argc,char **argv){
 if(argc!=1&&argc!=5)return 2;
 check_public_negative();
 if(argc==5){
  check_profile(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
  check_profile(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
  check_profile(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 }
 printf("EXEC0270 %u checks PASS (%s); source-backed Spring continuation only, no RNG startup parity\n",checks,argc==5?"private three profiles":"public refusal");
 return 0;
}
