#include "gk1_app_state.h"
#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do{++checks;if(!(x)){fprintf(stderr,"GK159 assertion %u at line %d: %s\n",checks,__LINE__,#x);abort();}}while(0)
static void blocked(void){
 Gk79Checkpoint original={0};
 Gk116EnhancedAssign spring={0};
 Gk158EnhancedSummer summer={0};
 Gk159EnhancedFall fall={0},before=fall;
 CHECK(gk159_open(&spring,&summer,&original,&fall)==GK159_SOURCE);
 CHECK(!memcmp(&fall,&before,sizeof fall));
 CHECK(gk159_transfer(&fall,&spring,&summer,&original,0,4,5)==GK159_SOURCE);
 CHECK(gk159_pass(&fall,&spring,&summer,&original)==GK159_SOURCE);
 CHECK(gk159_save_new("GK159_REFUSED_NEVER_CREATE.gk19",&original,&spring,&summer,&fall)==GK159_SOURCE);
 CHECK(gk159_cold_load("GK159_MISSING.gk19",&original,0,&spring,&summer,&fall)==GK159_SOURCE);
 CHECK(!memcmp(&fall,&before,sizeof fall));
}
static void run_original(const Gk1TemplateDataset *d,const char *csv,
 const char *path,Gk77Profile profile){
 Gk1App *app=calloc(1,sizeof *app);CHECK(app);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,d));
 unsigned world=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 CHECK(gk117_open_attested_original(csv,&app->worlds[world],path,profile,0,
     &app->stock_checkpoint)==GK117_OK);
 Gk79Checkpoint immutable=app->stock_checkpoint;
 CHECK(gk116_open(&immutable,1u,&app->enhanced_assign)==GK116_OK);
 CHECK(gk159_open(&app->enhanced_assign,&app->enhanced_summer,
     &immutable,&app->enhanced_fall)==GK159_SOURCE);
 CHECK(gk116_transfer(&app->enhanced_assign,&immutable,0,4,5)==GK116_OK);
 CHECK(gk116_pass_remaining(&app->enhanced_assign,&immutable)==GK116_OK);
 CHECK(gk158_open(&app->enhanced_assign,&immutable,&app->enhanced_summer)==GK158_OK);
 CHECK(gk159_open(&app->enhanced_assign,&app->enhanced_summer,
     &immutable,&app->enhanced_fall)==GK159_SOURCE);
 CHECK(gk158_transfer(&app->enhanced_summer,&app->enhanced_assign,
     &immutable,0,4,5)==GK158_OK);
 CHECK(gk158_pass(&app->enhanced_summer,&app->enhanced_assign,&immutable)==GK158_OK);
 CHECK(gk159_open(&app->enhanced_assign,&app->enhanced_summer,
     &immutable,&app->enhanced_fall)==GK159_OK);
 CHECK(gk159_attestation_intact(&app->enhanced_fall,&app->enhanced_assign,
     &app->enhanced_summer,&immutable));
 CHECK(app->enhanced_fall.budget.season==GK71_FALL&&
       app->enhanced_fall.budget.remaining==3u);
 Gk159EnhancedFall before=app->enhanced_fall;
 CHECK(gk159_transfer(&app->enhanced_fall,&app->enhanced_assign,&app->enhanced_summer,
       &immutable,0,0,5)==GK159_INPUT);
 CHECK(!memcmp(&before,&app->enhanced_fall,sizeof before));
 CHECK(gk159_transfer(&app->enhanced_fall,&app->enhanced_assign,&app->enhanced_summer,
       &immutable,2,4,65535)==GK159_NO_WORKER);
 CHECK(!memcmp(&before,&app->enhanced_fall,sizeof before));
 CHECK(gk159_transfer(&app->enhanced_fall,&app->enhanced_assign,&app->enhanced_summer,
       &immutable,0,4,5)==GK159_OK);
 CHECK(app->enhanced_fall.action_count==1u&&app->enhanced_fall.budget.remaining==2u);
 Gk159EnhancedFall tampered=app->enhanced_fall;
 tampered.mutable_world.countries[0].town++;
 CHECK(!gk159_attestation_intact(&tampered,&app->enhanced_assign,&app->enhanced_summer,&immutable));
 tampered=app->enhanced_fall;tampered.budget.remaining++;
 CHECK(!gk159_attestation_intact(&tampered,&app->enhanced_assign,&app->enhanced_summer,&immutable));
 Gk158EnhancedSummer bad_summer=app->enhanced_summer;
 bad_summer.mutable_world.countries[0].troops++;
 CHECK(!gk159_attestation_intact(&app->enhanced_fall,&app->enhanced_assign,&bad_summer,&immutable));
 Gk79Checkpoint bad_original=immutable;bad_original.raw_sha256[0]^=1u;
 CHECK(!gk159_attestation_intact(&app->enhanced_fall,&app->enhanced_assign,
     &app->enhanced_summer,&bad_original));
 CHECK(gk159_pass(&app->enhanced_fall,&app->enhanced_assign,&app->enhanced_summer,
     &immutable)==GK159_OK);
 CHECK(gk159_pass(&app->enhanced_fall,&app->enhanced_assign,&app->enhanced_summer,
     &immutable)==GK159_NO_ORDERS);
 CHECK(!memcmp(&immutable,&app->stock_checkpoint,sizeof immutable));
 const char *receipt="GK159_source_bound_test.gk19";(void)remove(receipt);
 CHECK(gk159_save_new(receipt,&immutable,&app->enhanced_assign,
     &app->enhanced_summer,&app->enhanced_fall)==GK159_OK);
 CHECK(gk159_save_new(receipt,&immutable,&app->enhanced_assign,
     &app->enhanced_summer,&app->enhanced_fall)==GK159_EXISTS);
 Gk116EnhancedAssign loaded_sp={0};Gk158EnhancedSummer loaded_su={0};Gk159EnhancedFall loaded_fa={0};
 CHECK(gk159_cold_load(receipt,&immutable,0,&loaded_sp,&loaded_su,&loaded_fa)==GK159_SOURCE);
 CHECK(gk159_cold_load(receipt,&immutable,1,&loaded_sp,&loaded_su,&loaded_fa)==GK159_OK);
 CHECK(!memcmp(&loaded_sp,&app->enhanced_assign,sizeof loaded_sp));
 CHECK(!memcmp(&loaded_su,&app->enhanced_summer,sizeof loaded_su));
 CHECK(!memcmp(&loaded_fa,&app->enhanced_fall,sizeof loaded_fa));
 FILE *fp=fopen(receipt,"r+b");CHECK(fp);CHECK(fseek(fp,81,SEEK_SET)==0);
 CHECK(fputc(0xff,fp)==0xff);CHECK(fclose(fp)==0);
 Gk159EnhancedFall saved=loaded_fa;
 CHECK(gk159_cold_load(receipt,&immutable,1,&loaded_sp,&loaded_su,&loaded_fa)==GK159_CORRUPT);
 CHECK(!memcmp(&loaded_fa,&saved,sizeof saved));
 CHECK(remove(receipt)==0);
 /* Native controller command menu cannot create a Fall without a Summer.
  * When Summer has a verified PASS receipt, controller ASSIGN routes to Fall. */
 app->rules_profile=1u;app->screen=GK1_SCREEN_STOCK_COMMANDS;
 app->stock_command_cursor=1u;
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 CHECK(app->screen==GK1_SCREEN_ENHANCED_FALL);
 CHECK(gk1_app_input(app,GK1_INPUT_SETUP));CHECK(app->assign_quantity==5u);
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL));
 CHECK(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(!memcmp(&immutable,&app->stock_checkpoint,sizeof immutable));
 free(app);
}
int main(int argc,char **argv){
 blocked();
 if(argc==2){printf("GK159 %u public negative controls PASS; private original snapshots NOT RUN\n",checks);return 0;}
 CHECK(argc==5);
 Gk1TemplateDataset *data=malloc(sizeof *data);CHECK(data);
 CHECK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 CHECK(gk1_verified_original_csv(argv[1]));
 run_original(data,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
 run_original(data,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
 run_original(data,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 free(data);
 printf("GK159 %u assertions: THREE authentic original source profiles, Enhanced Fall, replay, rollback, cold-load PASS (NOT original NES seasonal simulation)\n",checks);
 return 0;
}
