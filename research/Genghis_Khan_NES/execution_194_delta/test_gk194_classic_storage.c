/* EXEC0194: actual controller OTHER -> private GC19 save/cold-load after a
 * source-attested Classic labor order. This does not model original NES RNG. */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk194_classic_storage_gate.h"
#include "gk193_auto_source_entry.h"
#include "gk189_classic_cold_save.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
static unsigned checks;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"EXEC0194 failure line %d (%s)\n",__LINE__,#x);return 1;}checks++;}while(0)
static Gk1TemplateDataset *ds;
static int install(Gk1App *a){gk1_app_init(a);a->rules_profile=0u;return gk1_app_install_templates(a,ds);}
#ifndef _WIN32
static int private_profile(const char *csv,const char *source,Gk77Profile expected){
 Gk1App *a=calloc(1,sizeof *a),*b=calloc(1,sizeof *b),*snapshot=calloc(1,sizeof *snapshot);
 CHECK(a&&b&&snapshot);CHECK(install(a));CHECK(install(b));
 Gk77Profile detected=GK77_ARCHIVED_WORLD_SAVERAM;
 CHECK(gk193_enter_original_auto(a,csv,source,0,&detected)==GK193_OK);
 CHECK(detected==expected);
 CHECK(a->screen==GK1_SCREEN_STOCK_CONTINUE);
 CHECK(gk194_classic_storage_allowed(a));
 const Gk79Checkpoint original=a->stock_checkpoint;
 /* Actual pad/command routing: enter ASSIGN and commit one source-derived
  * labor allocation. Immutable original RAM must remain untouched. */
 CHECK(gk1_app_input(a,GK1_INPUT_SETUP)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CHECK(a->stock_command_cursor==1u);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->screen==GK1_SCREEN_CLASSIC_ASSIGN);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->classic_assign.action_count==1u);
 CHECK(a->classic_assign.budget.remaining==2u);
 CHECK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 CHECK(!memcmp(&original.stock_raw,&a->stock_checkpoint.stock_raw,sizeof original.stock_raw));
 CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(gk194_classic_storage_allowed(a));
 CHECK(gk1_app_input(a,GK1_INPUT_RIGHT)==1);
 for(unsigned i=0;i<5u;i++)CHECK(gk1_app_input(a,GK1_INPUT_DOWN)==1);
 CHECK(a->stock_command_cursor==13u);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_STORAGE);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->stock_storage_confirm==1u && a->stock_storage_request==0u);
 CHECK(gk1_app_input(a,GK1_INPUT_CANCEL)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_COMMANDS && !a->stock_storage_request);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->screen==GK1_SCREEN_STOCK_STORAGE);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->stock_storage_confirm==1u);
 CHECK(gk1_app_input(a,GK1_INPUT_CONFIRM)==1);
 CHECK(a->stock_storage_request==1u);
 a->stock_storage_request=0u; /* exact native-shell one-shot consumption */
 char path[]="/tmp/gk194-storage-XXXXXX";
 int fd=mkstemp(path);CHECK(fd>=0);CHECK(close(fd)==0);CHECK(unlink(path)==0);
 CHECK(gk189_save_new(path,a)==GK189_OK);
 CHECK(gk189_save_new(path,a)==GK189_EXISTS);
 CHECK(gk189_cold_load(path,csv,b)==GK189_OK);
 CHECK(b->classic_assign.action_count==1u && b->classic_assign.budget.remaining==2u);
 CHECK(gk187_intact(&b->classic_assign,&b->stock_checkpoint));
 CHECK(b->stock_checkpoint.profile==expected);
 CHECK(!memcmp(b->stock_checkpoint.stock_raw,original.stock_raw,sizeof original.stock_raw));
 /* Verify OTHER is usable even AFTER a source-authentic cold restart. */
 CHECK(gk1_app_input(b,GK1_INPUT_CANCEL)==1);
 CHECK(b->screen==GK1_SCREEN_STOCK_COMMANDS);
 b->stock_command_cursor=13u;
 CHECK(gk1_app_input(b,GK1_INPUT_CONFIRM)==1);
 CHECK(b->screen==GK1_SCREEN_STOCK_STORAGE);
 /* Negative: forged source-linked Classic action cannot enter OTHER. */
 Gk1App *forged=calloc(1,sizeof *forged);CHECK(forged!=NULL);
 *forged=*b;
 forged->classic_assign.mutable_world.countries[0].gold++;
 CHECK(!gk194_classic_storage_allowed(forged));
 forged->screen=GK1_SCREEN_STOCK_COMMANDS;forged->stock_command_cursor=13u;
 CHECK(gk1_app_input(forged,GK1_INPUT_CONFIRM)==1);
 CHECK(forged->screen==GK1_SCREEN_STOCK_COMMANDS && !forged->stock_storage_request);
 /* Negative: the storage screen can no longer dispatch after corruption. */
 forged->screen=GK1_SCREEN_STOCK_STORAGE;forged->stock_storage_confirm=1;
 CHECK(gk1_app_input(forged,GK1_INPUT_CONFIRM)==1);
 CHECK(forged->screen==GK1_SCREEN_STOCK_COMMANDS && !forged->stock_storage_request);
 *forged=*b;forged->rules_profile=1u;
 CHECK(!gk194_classic_storage_allowed(forged));
 *snapshot=*b;
 FILE *f=fopen(path,"r+b");CHECK(f!=NULL);
 CHECK(fseek(f,70L,SEEK_SET)==0);
 int byte=fgetc(f);CHECK(byte>=0);
 CHECK(fseek(f,70L,SEEK_SET)==0);
 CHECK(fputc(byte^1,f)!=EOF);CHECK(fclose(f)==0);
 CHECK(gk189_cold_load(path,csv,b)==GK189_CORRUPT);
 CHECK(memcmp(snapshot,b,sizeof *b)==0);
 CHECK(remove(path)==0);
 free(forged);free(snapshot);free(b);free(a);
 return 0;
}
#endif
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 Gk1App *a=calloc(1,sizeof *a);CHECK(a!=NULL);
 CHECK(!gk194_classic_storage_allowed(NULL));
 CHECK(!gk194_classic_storage_allowed(a));
 ds=calloc(1,sizeof *ds);CHECK(ds!=NULL);
 CHECK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 CHECK(install(a));CHECK(!gk194_classic_storage_allowed(a));
 free(a);
#ifndef _WIN32
 if(argc==5){
  CHECK(private_profile(argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM)==0);
  CHECK(private_profile(argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM)==0);
  CHECK(private_profile(argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM)==0);
 }
#else
 if(argc==5){fprintf(stderr,"Original private-source positive test not run on Win32\n");return 3;}
#endif
 printf("TESTED_PORT_LINUX EXEC0194 %u assertions PASS; %s; no licensed source packaged\n",checks,argc==5?"3 AUTHENTIC ORIGINAL PROFILES":"PUBLIC NEGATIVE GATES");
 free(ds);return 0;
}
