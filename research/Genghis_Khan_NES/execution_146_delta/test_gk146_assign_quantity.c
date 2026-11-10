#include "gk1_app_state.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void quantity_oracle(void){
 static const uint16_t values[6]={1,5,10,25,50,100};
 for(int i=0;i<6;i++){
  uint16_t x=0;
  assert(gk146_quantity_is_valid(values[i]));
  assert(gk146_quantity_step(values[i],1,&x)==GK146_QUANTITY_OK);
  assert(x==values[(i+1)%6]);
  assert(gk146_quantity_step(values[i],-1,&x)==GK146_QUANTITY_OK);
  assert(x==values[(i+5)%6]);
 }
 uint16_t before=991;
 assert(!gk146_quantity_is_valid(0));
 assert(!gk146_quantity_is_valid(2));
 assert(!gk146_quantity_is_valid(101));
 assert(gk146_quantity_step(0,1,&before)==GK146_QUANTITY_INVALID&&before==991);
 assert(gk146_quantity_step(1,0,&before)==GK146_QUANTITY_INVALID&&before==991);
 assert(gk146_quantity_step(1,1,NULL)==GK146_QUANTITY_INVALID);
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 gk1_app_init(app);app->screen=GK1_SCREEN_ENHANCED_ASSIGN;app->assign_quantity=1;
 Gk1App before_app=*app;
 assert(gk1_app_input(app,GK1_INPUT_SETUP));
 /* Synthetic/fake original cannot operate Enhanced order or quantity. */
 assert(!memcmp(&before_app.enhanced_assign,&app->enhanced_assign,sizeof app->enhanced_assign));
 assert(app->assign_quantity==1);
 free(app);
}
static void original_profile(const Gk1TemplateDataset *source,
 const char *csv,const char *record,Gk77Profile profile){
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 gk1_app_init(app);assert(gk1_app_install_templates(app,source));
 unsigned which=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 assert(gk117_open_attested_original(csv,&app->worlds[which],record,profile,0,
         &app->stock_checkpoint)==GK117_OK);
 Gk79Checkpoint stock=app->stock_checkpoint;
 app->rules_profile=1u;app->screen=GK1_SCREEN_STOCK_COMMANDS;
 app->stock_command_cursor=1u;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->screen==GK1_SCREEN_ENHANCED_ASSIGN);
 assert(app->assign_quantity==1u);
 assert(app->assign_from==0u&&app->assign_to==4u);
 for(int i=0;i<2;i++)assert(gk1_app_input(app,GK1_INPUT_SETUP));
 assert(app->assign_quantity==10u);
 assert(gk1_app_input(app,GK1_INPUT_LABOR_PLAN));
 assert(app->assign_quantity==5u);
 Gk1CountryTemplate prior=app->enhanced_assign.mutable_world.countries[0];
 assert(prior.troops>=5u && prior.food_workers<=994u);
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->enhanced_assign.action_count==1u);
 assert(app->enhanced_assign.budget.remaining==2u);
 Gk1CountryTemplate after=app->enhanced_assign.mutable_world.countries[0];
 assert(after.troops==prior.troops-5u);
 assert(after.food_workers==prior.food_workers+5u);
 assert(app->enhanced_assign.actions[0].quantity==5u);
 assert(!memcmp(&stock,&app->stock_checkpoint,sizeof stock));
 assert(gk116_attestation_intact(&app->enhanced_assign,&stock));
 const char *path="gk146_authenticated_private.ga16";
 (void)remove(path);
 assert(gk116_save_new(path,&stock,&app->enhanced_assign)==GK116_OK);
 Gk116EnhancedAssign restored={0};
 assert(gk116_cold_load(path,&stock,1u,&restored)==GK116_OK);
 assert(!memcmp(&restored,&app->enhanced_assign,sizeof restored));
 assert(remove(path)==0);
 /* Out-of-range transfer rejected without consuming an Enhanced order. */
 for(int i=0;i<5;i++)assert(gk1_app_input(app,GK1_INPUT_SETUP));
 assert(app->assign_quantity==1u);
 assert(gk1_app_input(app,GK1_INPUT_LABOR_PLAN));
 assert(app->assign_quantity==100u);
 app->assign_from=2;app->assign_to=4;
 Gk116EnhancedAssign before=app->enhanced_assign;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(!memcmp(&before,&app->enhanced_assign,sizeof before));
 assert(!memcmp(&stock,&app->stock_checkpoint,sizeof stock));
 assert(gk1_app_input(app,GK1_INPUT_CANCEL));
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 app->rules_profile=0;
 before=app->enhanced_assign;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(!memcmp(&before,&app->enhanced_assign,sizeof before));
 assert(!memcmp(&stock,&app->stock_checkpoint,sizeof stock));
 free(app);
}
int main(int argc,char **argv){
 quantity_oracle();
 if(argc==1){puts("GK146 public negative controls PASS; private ORIGINAL stock positive NOT RUN");return 0;}
 assert(argc==5);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);assert(dataset);
 assert(gk1_template_load(argv[1],dataset)==GK1_LOAD_OK);
 assert(gk1_verified_original_csv(argv[1]));
 original_profile(dataset,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
 original_profile(dataset,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
 original_profile(dataset,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 free(dataset);
 puts("GK146 private original-stock fresh + archived Mongol + archived World: variable quantity 5 workers/one action, rollback, save/coldload, immutable original raw/source PASS; NO new Mesen execution");
 return 0;
}
