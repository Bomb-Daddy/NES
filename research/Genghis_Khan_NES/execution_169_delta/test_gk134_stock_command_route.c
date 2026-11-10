#include "gk1_app_state.h"
#include "gk134_stock_command_route.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void cardinality_and_identity(void){
 static const char *const expected[14]={
  "TAX","ASSIGN","GIVE","TRAIN","TRADE","PEOPLE","VIEW",
  "MOVE","POLICY","TREATY","SPY","WAR","PASS","OTHER"
 };
 for(unsigned i=0;i<14u;i++){
  assert(gk134_command_name(i)!=NULL);
  assert(strcmp(gk134_command_name(i),expected[i])==0);
  assert(strcmp(gk87_stock_command_name(i),expected[i])==0);
  Gk134CommandRoute r=gk134_command_route(i);
  assert(r==(i==1?GK134_ROUTE_ENHANCED_ASSIGN:
           i==4?GK134_ROUTE_TRADE_REFERENCE:
           i==6?GK134_ROUTE_VIEW:
           i==12?GK134_ROUTE_PASS:i==13?GK134_ROUTE_PORT_STORAGE:GK134_ROUTE_LOCKED_STOCK_ACTION));
 }
 assert(gk134_command_name(14)==NULL);
 assert(gk134_command_name(255)==NULL);
 assert(gk134_command_route(14)==GK134_ROUTE_INVALID);
 assert(gk134_command_route(999)==GK134_ROUTE_INVALID);
 assert(gk134_command_route(0)==GK134_ROUTE_LOCKED_STOCK_ACTION);
 assert(gk134_command_route(1)==GK134_ROUTE_ENHANCED_ASSIGN);
}
static void source_guard_no_mutation(void){
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 gk1_app_init(app);app->screen=GK1_SCREEN_STOCK_COMMANDS;
 app->rules_profile=1;
 app->stock_command_cursor=0;
 Gk79Checkpoint before=app->stock_checkpoint;
 assert(!gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 assert(!memcmp(&before,&app->stock_checkpoint,sizeof before));
 app->stock_command_cursor=1;
 assert(!gk1_app_input(app,GK1_INPUT_CONFIRM));
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 assert(!memcmp(&before,&app->stock_checkpoint,sizeof before));
 free(app);
}
/* Full authentic profile input is optional, deliberately not simulated.
 * Only a private SHA-authenticated Spring can test the actual UI path. */
static void authentic_controller_route(const Gk1TemplateDataset *dataset,
 const char *csv,const char *private_path,Gk77Profile profile){
 Gk1App *app=calloc(1,sizeof *app);assert(app);
 gk1_app_init(app);
 assert(gk1_app_install_templates(app,dataset));
 unsigned w=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 assert(gk117_open_attested_original(csv,&app->worlds[w],private_path,
            profile,0,&app->stock_checkpoint)==GK117_OK);
 Gk79Checkpoint original=app->stock_checkpoint;
 app->screen=GK1_SCREEN_STOCK_COMMANDS;
 app->rules_profile=1;
 app->stock_command_cursor=0; /* TAX remains blocked in both profiles. */
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 assert(!memcmp(&original,&app->stock_checkpoint,sizeof original));
 assert(!app->enhanced_assign.active);
 app->stock_command_cursor=1;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->screen==GK1_SCREEN_ENHANCED_ASSIGN);
 assert(app->enhanced_assign.active);
 assert(!memcmp(&original,&app->stock_checkpoint,sizeof original));
 assert(gk1_app_input(app,GK1_INPUT_CANCEL)==1);
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 /* Classic mode must not run Enhanced Assign. */
 app->rules_profile=0;
 app->stock_command_cursor=1;
 Gk116EnhancedAssign kept=app->enhanced_assign;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 assert(!memcmp(&kept,&app->enhanced_assign,sizeof kept));
 assert(!memcmp(&original,&app->stock_checkpoint,sizeof original));
 app->stock_command_cursor=0;
 assert(gk1_app_input(app,GK1_INPUT_CONFIRM)==1);
 assert(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 assert(!memcmp(&original,&app->stock_checkpoint,sizeof original));
 free(app);
}
int main(int argc,char **argv){
 cardinality_and_identity();source_guard_no_mutation();
 if(argc==1){puts("GK134 PASS: canonical all-14 routing and denied fake-source UI. AUTHENTIC POSITIVE NOT RUN (requires private source)");return 0;}
 assert(argc==5);
 Gk1TemplateDataset *data=malloc(sizeof *data);assert(data);
 assert(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 assert(gk1_verified_original_csv(argv[1]));
 authentic_controller_route(data,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
 authentic_controller_route(data,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
 authentic_controller_route(data,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 free(data);
 puts("GK134 PASS: real fresh, restored Mongol and World source-attested controller TAX/ASSIGN mapping, Classic isolation; NO new NES Mesen run");
 return 0;
}
