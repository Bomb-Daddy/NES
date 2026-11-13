#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
/* EXEC0256: source-authenticated controller VIEW-A must NEVER call gk119_pass
 * on pristine NES original orders. Optional private three-profile QA; no ROM,
 * snapshots, raw 83-byte records or licensed assets bundled here. */
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
static unsigned checks=0;
#define CK(x) do { assert((x)); checks++; } while(0)
static Gk1App *new_app(const char *csv,const char *source,Gk77Profile profile){
 Gk1App *a=calloc(1,sizeof *a);CK(a!=NULL);
 Gk1TemplateDataset *d=malloc(sizeof *d);CK(d!=NULL);
 gk1_app_init(a);CK(gk1_template_load(csv,d)==GK1_LOAD_OK);
 CK(gk1_app_install_templates(a,d)==1);free(d);a->rules_profile=0u;
 Gk77Profile found=0;
 CK(gk193_enter_original_auto(a,csv,source,0u,&found)==GK193_OK);
 CK(found==profile);CK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
 CK(a->stock_checkpoint.orders.remaining==3u);
 CK(a->classic_assign.active==0u);return a;
}
static void original_unmodified(const Gk1App *a,const Gk79Checkpoint *before){
 CK(gk85_raw_source_intact(&a->stock_checkpoint));
 CK(gk88_stock_projection_intact(&a->stock_checkpoint));
 CK(memcmp(a->stock_checkpoint.stock_raw,before->stock_raw,
   sizeof before->stock_raw)==0);
 CK(memcmp(&a->stock_checkpoint.orders,&before->orders,sizeof before->orders)==0);
 CK(memcmp(a->stock_checkpoint.raw_sha256,before->raw_sha256,32u)==0);
 CK(a->stock_checkpoint.year==before->year);
 CK(a->stock_checkpoint.world.country_count==before->world.country_count);
 CK(a->stock_checkpoint.orders.remaining==3u);
}
static void cold_receipt(Gk1App *a){
 char path[1024]={0};
#ifdef _WIN32
 char dir[MAX_PATH]={0},tmp[MAX_PATH]={0};
 CK(GetTempPathA(MAX_PATH,dir)>0);CK(GetTempFileNameA(dir,"g56",0,tmp)!=0);
 CK(DeleteFileA(tmp)!=0);CK(strlen(tmp)<sizeof path);strcpy(path,tmp);
#else
 strcpy(path,"/tmp/gk256-view-pass-XXXXXX");
 int fd=mkstemp(path);CK(fd>=0);CK(close(fd)==0);CK(unlink(path)==0);
#endif
 CK(gk188_save_new(path,&a->stock_checkpoint,&a->classic_assign)==GK188_OK);
 Gk187ClassicAssign *r=malloc(sizeof *r);CK(r!=NULL);
 memset(r,0xa5,sizeof *r);
 CK(gk188_load(path,&a->stock_checkpoint,r)==GK188_OK);
 CK(memcmp(r,&a->classic_assign,sizeof *r)==0);
 CK(gk187_intact(r,&a->stock_checkpoint));
 CK(remove(path)==0);free(r);
}
static void check_view_first_pass(const char *csv,const char *source,Gk77Profile profile){
 Gk1App *a=new_app(csv,source,profile);
 Gk79Checkpoint *before=malloc(sizeof *before);CK(before!=NULL);
 *before=a->stock_checkpoint;
 /* Native source View screen, first A arms without activating ledger. */
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->stock_checkpoint.pending_pass==1u);
 CK(a->classic_assign.active==0u);
 original_unmodified(a,before);
 /* B cancels, no ledger, no source change. */
 CK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 CK(a->stock_checkpoint.pending_pass==0u);
 CK(a->classic_assign.active==0u);original_unmodified(a,before);
 /* A/A commits PASS in derived ledger, preserving stock orders = 3. */
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.active==0u&&a->stock_checkpoint.pending_pass==1u);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.active==1u&&a->classic_assign.passed==1u);
 CK(a->classic_assign.action_count==0u);
 CK(a->classic_assign.budget.ended==1u);
 CK(a->classic_assign.budget.remaining==0u);
 CK(a->stock_checkpoint.pending_pass==0u);
 CK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 original_unmodified(a,before);cold_receipt(a);
 Gk187ClassicAssign after=a->classic_assign;
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(memcmp(&after,&a->classic_assign,sizeof after)==0);
 original_unmodified(a,before);
 printf("EXEC0256 authentic %s: first VIEW A-B/A-A, immutable NES, source-bound cold receipt PASS\n",
  gk77_profile_name(profile));free(before);free(a);
}
static void check_tax_then_view_pass(const char *csv,const char *source){
 Gk1App *a=new_app(csv,source,GK77_FRESH_MONGOL_WORKRAM);
 Gk79Checkpoint *before=malloc(sizeof *before);CK(before!=NULL);
 *before=a->stock_checkpoint;
 CK(gk1_app_input(a,GK1_INPUT_SETUP)==1);
 CK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->screen==GK1_SCREEN_CLASSIC_TAX);
 CK(gk1_app_input(a,GK1_INPUT_UP)==1);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.action_count==1u);
 CK(a->classic_assign.budget.remaining==2u);
 CK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 /* Enter VIEW from menu, then use A/A to PASS the remaining 2 orders. */
 for(unsigned j=0;j<6;j++)CK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CK(a->stock_command_cursor==6u);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->stock_checkpoint.pending_pass==1u);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CK(a->classic_assign.passed==1u);
 CK(a->classic_assign.action_count==1u);
 CK(a->classic_assign.budget.remaining==0u);
 CK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 original_unmodified(a,before);cold_receipt(a);
 free(before);free(a);
}
static void public_fail_closed(void){
 Gk1App *a=calloc(1,sizeof *a);CK(a!=NULL);gk1_app_init(a);
 a->rules_profile=0u;a->screen=GK1_SCREEN_STOCK_CONTINUE;
 Gk1App *before=malloc(sizeof *before);CK(before!=NULL);
 memcpy(before,a,sizeof *a);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==0);
 CK(memcmp(before,a,sizeof *a)==0);
 a->stock_checkpoint.valid=1u;memcpy(before,a,sizeof *a);
 CK(gk1_app_input(a,GK1_INPUT_CONFIRM)==0);
 CK(memcmp(before,a,sizeof *a)==0);
 free(before);free(a);
}
int main(int argc,char **argv){
 if(argc!=1&&argc!=5)return 2;
 public_fail_closed();
 if(argc==5){
  check_view_first_pass(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
  check_view_first_pass(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
  check_view_first_pass(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
  check_tax_then_view_pass(argv[1],argv[2]);
 }
 printf("EXEC0256 %u linked C11 assertions PASS (%s). Original NES RNG, next season, Win32 and Xbox not certified.\n",checks,argc==5?"three authentic source profiles":"public fail-closed");
 return 0;
}
