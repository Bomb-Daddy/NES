#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/* EXEC0254: actual inherited C11 controller passes first Classic order,
 * ORIGINAL stock checkpoint remains immutable; source-bound GC19 cold replay.
 * Only authenticated private inputs exercise positive port runtime.
 * No licensed Mesen records or exact source ROM distributed in this test.
 */
#include "gk193_auto_source_entry.h"
#include "gk188_classic_receipt.h"
#include "gk1_app_state.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif
#ifdef NDEBUG
#undef NDEBUG
#endif
static unsigned checks=0u;
#define CK(x) do{assert(x); ++checks;}while(0)
static Gk1App *new_source_app(const char *csv,const char *source,Gk77Profile profile){
 Gk1App *a=calloc(1,sizeof *a);CK(a!=NULL);
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);CK(dataset!=NULL);
 gk1_app_init(a);
 CK(gk1_template_load(csv,dataset)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(a,dataset)==1);
 free(dataset);
 a->rules_profile=0u;
 Gk77Profile found=0;
 CK(gk193_enter_original_auto(a,csv,source,0u,&found)==GK193_OK);
 CK(found==profile);
 CK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
 CK(a->stock_checkpoint.orders.remaining==3u);
 CK(a->classic_assign.active==0u);
 return a;
}
static void open_pass_menu(Gk1App *a){
 CK(gk1_app_input(a,GK1_INPUT_SETUP)==1);
 CK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 CK(a->stock_command_cursor==0u);
 CK(gk1_app_input(a,GK1_INPUT_RIGHT)==1);
 for(unsigned n=0;n<5;n++)CK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CK(a->stock_command_cursor==12u);
}
static void pristine(const Gk1App *a,const Gk79Checkpoint *before){
 CK(gk85_raw_source_intact(&a->stock_checkpoint));
 CK(gk88_stock_projection_intact(&a->stock_checkpoint));
 CK(memcmp(a->stock_checkpoint.stock_raw,before->stock_raw,
   sizeof before->stock_raw)==0);
 CK(memcmp(&a->stock_checkpoint.orders,&before->orders,sizeof before->orders)==0);
 CK(memcmp(a->stock_checkpoint.raw_sha256,before->raw_sha256,32u)==0);
 CK(a->stock_checkpoint.year==before->year);
 CK(a->stock_checkpoint.profile==before->profile);
 CK(a->stock_checkpoint.world.country_count==before->world.country_count);
}
static void save_and_cold_reload(Gk1App *a){
 char file[1024]="";
#ifdef _WIN32
 char dir[MAX_PATH]={0},path[MAX_PATH]={0};
 CK(GetTempPathA(MAX_PATH,dir)>0);
 CK(GetTempFileNameA(dir,"g54",0,path)!=0);
 CK(DeleteFileA(path)!=0);
 CK(strlen(path)<sizeof file);
 strcpy(file,path);
#else
 strcpy(file,"/tmp/gk254-classic-pass-XXXXXX");
 int fd=mkstemp(file);CK(fd>=0);CK(close(fd)==0);CK(unlink(file)==0);
#endif
 CK(gk188_save_new(file,&a->stock_checkpoint,&a->classic_assign)==GK188_OK);
 Gk187ClassicAssign *replay=malloc(sizeof *replay);CK(replay!=NULL);
 memset(replay,0xa5,sizeof *replay);
 CK(gk188_load(file,&a->stock_checkpoint,replay)==GK188_OK);
 CK(memcmp(replay,&a->classic_assign,sizeof *replay)==0);
 CK(gk187_intact(replay,&a->stock_checkpoint));
 CK(remove(file)==0);
 free(replay);
}
static void test_authentic(const char *csv,const char *source,Gk77Profile profile){
 Gk1App *a=new_source_app(csv,source,profile);
 Gk79Checkpoint *original=malloc(sizeof *original);CK(original!=NULL);
 *original=a->stock_checkpoint;
 open_pass_menu(a);
 /* First press only arms confirmation; importantly, it creates no ledger
  * and consumes no original order. B cancels transaction atomically. */
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->stock_checkpoint.pending_pass==1u);
 CK(a->classic_assign.active==0u);
 pristine(a,original);
 CK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 CK(a->stock_checkpoint.pending_pass==0u);
 CK(a->classic_assign.active==0u);
 pristine(a,original);
 /* A-A must END the derived Classic Spring with zero executed orders. */
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.active==0u&&a->stock_checkpoint.pending_pass==1u);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.active==1u&&a->classic_assign.passed==1u);
 CK(a->classic_assign.action_count==0u);
 CK(a->classic_assign.budget.remaining==0u);
 CK(a->classic_assign.budget.ended==1u);
 CK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 CK(a->stock_checkpoint.pending_pass==0u);
 pristine(a,original);
 save_and_cold_reload(a);
 /* After Pass, controller cannot confirm additional actions, and prior
  * unmodified source still has its full authentic three original orders. */
 Gk187ClassicAssign prior=a->classic_assign;
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(memcmp(&prior,&a->classic_assign,sizeof prior)==0);
 pristine(a,original);
 printf("EXEC0254 AUTHENTIC SOURCE %s: controller PASS before Tax/Assign, A-B cancellation, A-A commit, immutable stock 3/3, GC19 cold replay PASS\n",gk77_profile_name(profile));
 free(original);free(a);
}
static void test_partial_then_pass(const char *csv,const char *source){
 Gk1App *a=new_source_app(csv,source,GK77_FRESH_MONGOL_WORKRAM);
 Gk79Checkpoint *original=malloc(sizeof *original);CK(original!=NULL);
 *original=a->stock_checkpoint;
 /* Commit one Tax rate using the actual integrated controller. */
 CK(gk1_app_input(a,GK1_INPUT_SETUP)==1);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->screen==GK1_SCREEN_CLASSIC_TAX);
 CK(gk1_app_input(a,GK1_INPUT_UP)==1);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.action_count==1u);
 CK(a->classic_assign.budget.remaining==2u);
 CK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 CK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 CK(gk1_app_input(a,GK1_INPUT_RIGHT)==1);
 for(unsigned n=0;n<5;n++)CK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CK(a->stock_command_cursor==12u);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->stock_checkpoint.pending_pass==1u);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.passed==1u);
 CK(a->classic_assign.action_count==1u);
 CK(a->classic_assign.budget.remaining==0u);
 CK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 pristine(a,original);
 save_and_cold_reload(a);
 /* A corrupt mutable receipt must never be silently accepted. */
 a->classic_assign.action_count=4u;
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.action_count==4u);
 pristine(a,original);
 printf("EXEC0254 partial Tax->PASS keeps original source bank pristine; corrupted ledger rejects\n");
 free(original);free(a);
}
static void public_negative(void){
 Gk1App *a=calloc(1,sizeof *a);CK(a!=NULL);
 gk1_app_init(a);a->rules_profile=0u;
 Gk1App *unchanged=malloc(sizeof *unchanged);CK(unchanged!=NULL);
 memcpy(unchanged,a,sizeof *unchanged);
 Gk77Profile found=GK77_ARCHIVED_WORLD_SAVERAM;
 CK(gk193_enter_original_auto(a,"not-a-csv","not-a-Mesen-file",0u,&found)!=GK193_OK);
 CK(memcmp(a,unchanged,sizeof *unchanged)==0);
 CK(found==GK77_ARCHIVED_WORLD_SAVERAM);
 free(unchanged);free(a);
}
int main(int argc,char **argv){
 if(argc!=1&&argc!=5)return 2;
 public_negative();
 if(argc==5){
  test_authentic(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
  test_authentic(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
  test_authentic(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
  test_partial_then_pass(argv[1],argv[2]);
 }
 printf("EXEC0254 %u checks PASS (%s); no independent NES Spring RNG or next season implied\n",
        checks,argc==5?"authentic three-profile":"public negative only");
 return 0;
}
