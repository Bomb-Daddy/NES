#include "gk1_app_state.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
static unsigned checks=0;
#define CHECK(x) do{checks++;if(!(x)){fprintf(stderr,"GK176 CHECK %u line %d: %s\n",checks,__LINE__,#x);abort();}}while(0)
static void negatives(void){
 Gk176EnhancedCycle s={0},old={0};
 Gk161EnhancedYear2 y={0};Gk116EnhancedAssign sp={0};
 Gk158EnhancedSummer su={0};Gk159EnhancedFall fa={0};
 Gk160EnhancedWinter wi={0};Gk79Checkpoint o={0};
 CHECK(gk176_open(&y,&sp,&su,&fa,&wi,&o,&s)==GK176_SOURCE);
 CHECK(gk176_open(&y,&sp,&su,&fa,&wi,&o,NULL)==GK176_INPUT);
 CHECK(!gk176_attestation_intact(&s,&y,&sp,&su,&fa,&wi,&o));
 CHECK(gk176_transfer(&s,&y,&sp,&su,&fa,&wi,&o,0,4,1)==GK176_SOURCE);
 CHECK(gk176_pass(&s,&y,&sp,&su,&fa,&wi,&o)==GK176_SOURCE);
 CHECK(gk176_advance(&s,&y,&sp,&su,&fa,&wi,&o)==GK176_SOURCE);
 CHECK(!memcmp(&old,&s,sizeof s));
}
static void original_positive(const char *csv,const char *records){
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1App *a=calloc(1,sizeof *a);
 CHECK(dataset!=NULL&&a!=NULL);
 CHECK(gk1_template_load(csv,dataset)==GK1_LOAD_OK);
 gk1_app_init(a);
 CHECK(gk1_app_install_templates(a,dataset));
 CHECK(gk117_open_attested_original(csv,&a->worlds[0],records,
   GK77_FRESH_MONGOL_WORKRAM,0,&a->stock_checkpoint)==GK117_OK);
 Gk79Checkpoint original=a->stock_checkpoint;
 CHECK(gk116_open(&original,1,&a->enhanced_assign)==GK116_OK);
 CHECK(gk116_transfer(&a->enhanced_assign,&original,0,4,5)==GK116_OK);
 CHECK(gk116_pass_remaining(&a->enhanced_assign,&original)==GK116_OK);
 CHECK(gk158_open(&a->enhanced_assign,&original,&a->enhanced_summer)==GK158_OK);
 CHECK(gk158_transfer(&a->enhanced_summer,&a->enhanced_assign,&original,0,4,5)==GK158_OK);
 CHECK(gk158_pass(&a->enhanced_summer,&a->enhanced_assign,&original)==GK158_OK);
 CHECK(gk159_open(&a->enhanced_assign,&a->enhanced_summer,&original,&a->enhanced_fall)==GK159_OK);
 CHECK(gk159_transfer(&a->enhanced_fall,&a->enhanced_assign,&a->enhanced_summer,&original,0,4,5)==GK159_OK);
 CHECK(gk159_pass(&a->enhanced_fall,&a->enhanced_assign,&a->enhanced_summer,&original)==GK159_OK);
 CHECK(gk160_open(&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&original,&a->enhanced_winter)==GK160_OK);
 CHECK(gk160_transfer(&a->enhanced_winter,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&original,0,4,5)==GK160_OK);
 CHECK(gk160_pass(&a->enhanced_winter,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&original)==GK160_OK);
 CHECK(gk161_open(&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original,&a->enhanced_year2)==GK161_OK);
 CHECK(gk176_open(&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original,&a->enhanced_continuation)==GK176_SOURCE);
 CHECK(gk161_transfer(&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original,0,4,5)==GK161_OK);
 CHECK(gk161_pass(&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original)==GK161_OK);
 Gk176EnhancedCycle *e=&a->enhanced_continuation;
 CHECK(gk176_open(&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original,e)==GK176_OK);
 CHECK(e->year==1176u&&e->budget.season==GK71_SUMMER&&e->budget.remaining==3u);
 CHECK(gk176_attestation_intact(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original));
 /* The inherited controller path must open the actual mutable continuation,
  * not a sidecar component, and B must return without source mutation. */
 Gk1App *controller=malloc(sizeof *controller);CHECK(controller!=NULL);
 *controller=*a;controller->rules_profile=1u;
 controller->screen=GK1_SCREEN_STOCK_COMMANDS;controller->stock_command_cursor=1u;
 CHECK(gk1_app_input(controller,GK1_INPUT_CONFIRM));
 CHECK(controller->screen==GK1_SCREEN_ENHANCED_CONTINUATION);
 CHECK(gk1_app_input(controller,GK1_INPUT_SETUP));
 CHECK(controller->assign_quantity==5u);
 CHECK(gk1_app_input(controller,GK1_INPUT_CONFIRM));
 CHECK(controller->enhanced_continuation.receipts[0].action_count==1u);
 CHECK(controller->enhanced_continuation.budget.remaining==2u);
 CHECK(gk1_app_input(controller,GK1_INPUT_CANCEL));
 CHECK(controller->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(!memcmp(&controller->stock_checkpoint,&original,sizeof original));
 CHECK(!memcmp(e,&a->enhanced_continuation,sizeof *e));
 free(controller);
 Gk176EnhancedCycle before=*e;
 CHECK(gk176_transfer(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original,1,1,1)==GK176_INPUT);
 CHECK(!memcmp(e,&before,sizeof *e));
 CHECK(gk176_transfer(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original,2,4,65535)==GK176_NO_WORKER);
 CHECK(!memcmp(e,&before,sizeof *e));
 CHECK(gk176_advance(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original)==GK176_NO_ORDERS);
 CHECK(!memcmp(e,&before,sizeof *e));
 for(unsigned i=0;i<GK176_MAX_SEASONS;i++){
  CHECK(e->season_index==i);
  CHECK(e->year==1176u+(i+1u)/4u);
  CHECK(e->budget.season==(i+1u)%4u);
  CHECK(gk176_transfer(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original,0,4,1)==GK176_OK);
  CHECK(e->receipts[i].action_count==1u);
  CHECK(e->budget.remaining==2u);
  Gk176EnhancedCycle corrupt=*e;
  corrupt.receipts[i].actions[0].quantity++;
  CHECK(!gk176_attestation_intact(&corrupt,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original));
  corrupt=*e;corrupt.mutable_world.countries[0].food_workers++;
  CHECK(!gk176_attestation_intact(&corrupt,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original));
  CHECK(gk176_pass(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original)==GK176_OK);
  CHECK(e->budget.ended==1u&&e->budget.remaining==0u);
  before=*e;
  CHECK(gk176_pass(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original)==GK176_NO_ORDERS);
  CHECK(!memcmp(e,&before,sizeof *e));
  if(i+1u<GK176_MAX_SEASONS){
   CHECK(gk176_advance(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original)==GK176_OK);
   CHECK(e->budget.remaining==3u);
  }
 }
 before=*e;
 CHECK(gk176_advance(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original)==GK176_END);
 CHECK(!memcmp(e,&before,sizeof *e));
 CHECK(!memcmp(&a->stock_checkpoint,&original,sizeof original));
 CHECK(gk116_attestation_intact(&a->enhanced_assign,&original));
 CHECK(gk176_attestation_intact(e,&a->enhanced_year2,&a->enhanced_assign,&a->enhanced_summer,&a->enhanced_fall,&a->enhanced_winter,&original));
 free(a);free(dataset);
}
int main(int argc,char **argv){
 negatives();
 if(argc==3)original_positive(argv[1],argv[2]);
 else CHECK(argc==2);
 printf("GK176 %u assertions PASS, %s; Enhanced labor only; no original NES season parity\n",checks,argc==3?"private authenticated original stock fresh reset":"public negatives");
 return 0;
}
