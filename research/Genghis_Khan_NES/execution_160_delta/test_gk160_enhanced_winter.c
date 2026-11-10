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
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"GK160 assertion %u line %d: %s\n",checks,__LINE__,#x);abort();} }while(0)
static void refused(void){
 Gk79Checkpoint o={0};Gk116EnhancedAssign sp={0};
 Gk158EnhancedSummer su={0};Gk159EnhancedFall fa={0};
 Gk160EnhancedWinter wi={0},old=wi;
 CHECK(gk160_open(&sp,&su,&fa,&o,&wi)==GK160_SOURCE);
 CHECK(!memcmp(&wi,&old,sizeof wi));
 CHECK(gk160_transfer(&wi,&sp,&su,&fa,&o,0,4,5)==GK160_SOURCE);
 CHECK(gk160_pass(&wi,&sp,&su,&fa,&o)==GK160_SOURCE);
 CHECK(gk160_save_new("GK160_NEVER_CREATE.gk20",&o,&sp,&su,&fa,&wi)==GK160_SOURCE);
 CHECK(gk160_cold_load("GK160_MISSING.gk20",&o,0,&sp,&su,&fa,&wi)==GK160_SOURCE);
 CHECK(gk160_open(&sp,&su,&fa,&o,NULL)==GK160_INPUT);
 CHECK(!memcmp(&wi,&old,sizeof wi));
}
static void run_original(const Gk1TemplateDataset *d,const char *csv,
 const char *path,Gk77Profile profile){
 Gk1App *app=calloc(1,sizeof *app);CHECK(app);
 gk1_app_init(app);CHECK(gk1_app_install_templates(app,d));
 unsigned world=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 CHECK(gk117_open_attested_original(csv,&app->worlds[world],path,profile,0,
        &app->stock_checkpoint)==GK117_OK);
 Gk79Checkpoint original=app->stock_checkpoint;
 CHECK(gk116_open(&original,1u,&app->enhanced_assign)==GK116_OK);
 CHECK(gk160_open(&app->enhanced_assign,&app->enhanced_summer,
        &app->enhanced_fall,&original,&app->enhanced_winter)==GK160_SOURCE);
 CHECK(gk116_transfer(&app->enhanced_assign,&original,0,4,5)==GK116_OK);
 CHECK(gk116_pass_remaining(&app->enhanced_assign,&original)==GK116_OK);
 CHECK(gk158_open(&app->enhanced_assign,&original,&app->enhanced_summer)==GK158_OK);
 CHECK(gk160_open(&app->enhanced_assign,&app->enhanced_summer,
        &app->enhanced_fall,&original,&app->enhanced_winter)==GK160_SOURCE);
 CHECK(gk158_transfer(&app->enhanced_summer,&app->enhanced_assign,
        &original,0,4,5)==GK158_OK);
 CHECK(gk158_pass(&app->enhanced_summer,&app->enhanced_assign,&original)==GK158_OK);
 CHECK(gk159_open(&app->enhanced_assign,&app->enhanced_summer,
        &original,&app->enhanced_fall)==GK159_OK);
 CHECK(gk160_open(&app->enhanced_assign,&app->enhanced_summer,
        &app->enhanced_fall,&original,&app->enhanced_winter)==GK160_SOURCE);
 CHECK(gk159_transfer(&app->enhanced_fall,&app->enhanced_assign,
        &app->enhanced_summer,&original,0,4,5)==GK159_OK);
 CHECK(gk159_pass(&app->enhanced_fall,&app->enhanced_assign,
        &app->enhanced_summer,&original)==GK159_OK);
 CHECK(gk160_open(&app->enhanced_assign,&app->enhanced_summer,
        &app->enhanced_fall,&original,&app->enhanced_winter)==GK160_OK);
 CHECK(gk160_attestation_intact(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original));
 CHECK(app->enhanced_winter.budget.season==GK71_WINTER);
 CHECK(app->enhanced_winter.budget.remaining==3u);
 Gk160EnhancedWinter prev=app->enhanced_winter;
 CHECK(gk160_transfer(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original,0,0,5)==GK160_INPUT);
 CHECK(!memcmp(&prev,&app->enhanced_winter,sizeof prev));
 CHECK(gk160_transfer(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original,2,4,65535)==GK160_NO_WORKER);
 CHECK(!memcmp(&prev,&app->enhanced_winter,sizeof prev));
 CHECK(gk160_transfer(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original,0,4,5)==GK160_OK);
 CHECK(app->enhanced_winter.action_count==1u&&app->enhanced_winter.budget.remaining==2u);
 Gk160EnhancedWinter tampered=app->enhanced_winter;
 tampered.mutable_world.countries[0].town++;
 CHECK(!gk160_attestation_intact(&tampered,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original));
 tampered=app->enhanced_winter;tampered.budget.remaining++;
 CHECK(!gk160_attestation_intact(&tampered,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original));
 Gk159EnhancedFall false_fall=app->enhanced_fall;false_fall.mutable_world.countries[0].troops++;
 CHECK(!gk160_attestation_intact(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&false_fall,&original));
 Gk79Checkpoint bad=original;bad.raw_sha256[0]^=1;
 CHECK(!gk160_attestation_intact(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&bad));
 CHECK(gk160_pass(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original)==GK160_OK);
 CHECK(gk160_pass(&app->enhanced_winter,&app->enhanced_assign,
        &app->enhanced_summer,&app->enhanced_fall,&original)==GK160_NO_ORDERS);
 CHECK(!memcmp(&original,&app->stock_checkpoint,sizeof original));
 const char *receipt="GK160_test_source_bound.gk20"; (void)remove(receipt);
 CHECK(gk160_save_new(receipt,&original,&app->enhanced_assign,&app->enhanced_summer,
        &app->enhanced_fall,&app->enhanced_winter)==GK160_OK);
 CHECK(gk160_save_new(receipt,&original,&app->enhanced_assign,&app->enhanced_summer,
        &app->enhanced_fall,&app->enhanced_winter)==GK160_EXISTS);
 Gk116EnhancedAssign sp={0};Gk158EnhancedSummer su={0};
 Gk159EnhancedFall fa={0};Gk160EnhancedWinter wi={0};
 CHECK(gk160_cold_load(receipt,&original,0,&sp,&su,&fa,&wi)==GK160_SOURCE);
 CHECK(gk160_cold_load(receipt,&original,1,&sp,&su,&fa,&wi)==GK160_OK);
 CHECK(!memcmp(&sp,&app->enhanced_assign,sizeof sp));
 CHECK(!memcmp(&su,&app->enhanced_summer,sizeof su));
 CHECK(!memcmp(&fa,&app->enhanced_fall,sizeof fa));
 CHECK(!memcmp(&wi,&app->enhanced_winter,sizeof wi));
 FILE *fp=fopen(receipt,"r+b");CHECK(fp);CHECK(fseek(fp,91,SEEK_SET)==0);
 CHECK(fputc(0xff,fp)==0xff);CHECK(fclose(fp)==0);
 Gk160EnhancedWinter pre=wi;
 CHECK(gk160_cold_load(receipt,&original,1,&sp,&su,&fa,&wi)==GK160_CORRUPT);
 CHECK(!memcmp(&wi,&pre,sizeof wi));
 CHECK(remove(receipt)==0);
 /* Native controller ASSIGN + PASS must route to source-attested Winter. */
 app->rules_profile=1u;app->screen=GK1_SCREEN_STOCK_COMMANDS;
 app->stock_command_cursor=1u;
 CHECK(gk1_app_input(app,GK1_INPUT_CONFIRM));
 CHECK(app->screen==GK1_SCREEN_ENHANCED_WINTER);
 CHECK(gk1_app_input(app,GK1_INPUT_SETUP));CHECK(app->assign_quantity==5u);
 CHECK(gk1_app_input(app,GK1_INPUT_CANCEL));
 CHECK(app->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(!memcmp(&original,&app->stock_checkpoint,sizeof original));
 free(app);
}
int main(int argc,char **argv){
 refused();
 if(argc==2){printf("GK160 %u public negative controls PASS; authentic private profiles NOT RUN\n",checks);return 0;}
 CHECK(argc==5);
 Gk1TemplateDataset *data=malloc(sizeof *data);CHECK(data);
 CHECK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 CHECK(gk1_verified_original_csv(argv[1]));
 run_original(data,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
 run_original(data,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
 run_original(data,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 free(data);
 printf("GK160 %u assertions: 3 authentic checkpoint source profiles; Enhanced Winter, rollback/save/load PASSED; no original NES parity\n",checks);
 return 0;
}
