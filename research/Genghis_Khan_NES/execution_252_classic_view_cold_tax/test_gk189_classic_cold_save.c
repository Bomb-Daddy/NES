#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk189_classic_cold_save.h"
#include "gk162_original_entry.h"
#include "gk193_auto_source_entry.h"
#include "gk1_source_identity.h"
#include "gk195_classic_view.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
static unsigned checks;
#define CHECK(x) do{assert(x);++checks;}while(0)
static Gk1App *newapp(const Gk1TemplateDataset *ds){
 Gk1App *a=calloc(1,sizeof *a);CHECK(a!=NULL);
 gk1_app_init(a);CHECK(gk1_app_install_templates(a,ds));return a;
}
static void public_negative(const Gk1TemplateDataset *ds,const char *csv){
 Gk1App *a=newapp(ds),*snapshot=malloc(sizeof *snapshot);
 CHECK(snapshot!=NULL);*snapshot=*a;
 CHECK(gk189_save_new("/tmp/gk189-do-not-create.gc19",a)==GK189_SOURCE);
 CHECK(gk189_cold_load("/tmp/gk189-missing.gc19",csv,a)==GK189_MISSING);
 CHECK(!memcmp(a,snapshot,sizeof *a));
 CHECK(gk189_save_new(NULL,a)==GK189_INVALID);
 CHECK(gk189_cold_load(NULL,csv,a)==GK189_INVALID);
 CHECK(gk189_cold_load("",csv,a)==GK189_INVALID);
 CHECK(gk189_cold_load("/tmp/gk189-missing.gc19",NULL,a)==GK189_INVALID);
 CHECK(!memcmp(a,snapshot,sizeof *a));
 free(snapshot);free(a);
}
static void positive(const Gk1TemplateDataset *ds,const char *csv,
 const char *records,Gk77Profile profile){
 Gk1App *a=newapp(ds),*loaded=newapp(ds),*before=malloc(sizeof *before);
 CHECK(before!=NULL);
 /* Preserve legacy raw83 positive fixtures while also accepting the
  * Director's three REAL authenticated Mesen source file formats. */
 FILE *probe=fopen(records,"rb");CHECK(probe!=NULL);
 unsigned char sig[4]={0};CHECK(fread(sig,1,4u,probe)==4u);
 CHECK(fclose(probe)==0);
 if(!memcmp(sig,"MSS\1",4u)||sig[0]=='{'){
  Gk77Profile detected=GK77_FRESH_MONGOL_WORKRAM;
  a->rules_profile=0u;
  CHECK(gk193_enter_original_auto(a,csv,records,0u,&detected)==GK193_OK);
  CHECK(detected==profile);
 }else{
  CHECK(gk162_enter_original_checkpoint(a,csv,records,profile,0u)==GK162_ENTRY_OK);
 }
 a->rules_profile=0u;
 CHECK(gk187_open(&a->stock_checkpoint,&a->classic_assign)==GK187_OK);
 CHECK(gk187_transfer(&a->classic_assign,&a->stock_checkpoint,0,4,1)==GK187_OK);
 unsigned original_tax=a->stock_checkpoint.world.countries[0].tax;
 CHECK(original_tax>=1u&&original_tax<=100u);
 unsigned changed_tax=original_tax==100u?99u:original_tax+1u;
 CHECK(gk187_set_tax(&a->classic_assign,&a->stock_checkpoint,(uint8_t)changed_tax)==GK187_OK);
 a->stock_checkpoint.view_index=(uint8_t)(a->stock_checkpoint.world.country_count-1u);
 CHECK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 char path[1024];
#ifndef _WIN32
 strcpy(path,"/tmp/gk189-authentic-XXXXXX");
 int f=mkstemp(path);CHECK(f>=0);CHECK(close(f)==0);CHECK(unlink(path)==0);
#else
 char directory[MAX_PATH],reserved[MAX_PATH];
 DWORD bytes=GetTempPathA(MAX_PATH,directory);
 CHECK(bytes>0u&&bytes<MAX_PATH);
 CHECK(GetTempFileNameA(directory,"g19",0,reserved)!=0);
 CHECK(strlen(reserved)<sizeof path);
 strcpy(path,reserved);
 CHECK(DeleteFileA(path)!=0);
#endif
 /* Mid-season source-authenticated save must reopen the actual command
  * menu with a remaining order, not jump into a stale Assign screen. */
 CHECK(gk189_save_new(path,a)==GK189_OK);
 CHECK(gk189_cold_load(path,csv,loaded)==GK189_OK);
 CHECK(loaded->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(loaded->stock_command_cursor==0u);
 CHECK(loaded->stock_checkpoint.view_index==loaded->stock_checkpoint.world.country_count-1u);
 CHECK(loaded->classic_assign.budget.remaining==1u);
 CHECK(loaded->classic_assign.action_count==2u);
 CHECK(gk1_app_input(loaded,GK1_INPUT_CONFIRM)==1);
 CHECK(loaded->screen==GK1_SCREEN_CLASSIC_TAX);
 CHECK(gk1_app_input(loaded,GK1_INPUT_CANCEL)==1);
 CHECK(loaded->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(loaded->classic_assign.budget.remaining==1u);
 CHECK(gk187_intact(&loaded->classic_assign,&loaded->stock_checkpoint));
 /* Actual controller VIEW must honor the saved nonplayer-country cursor,
  * then wrap 14/27 countries without rewriting the authentic source. */
 loaded->stock_command_cursor=6u;
 CHECK(gk1_app_input(loaded,GK1_INPUT_CONFIRM)==1);
 CHECK(loaded->screen==GK1_SCREEN_STOCK_CONTINUE);
 CHECK(loaded->stock_checkpoint.view_index==loaded->stock_checkpoint.world.country_count-1u);
 CHECK(gk1_app_input(loaded,GK1_INPUT_RIGHT)==1);
 CHECK(loaded->stock_checkpoint.view_index==0u);
 Gk195CountryView derived={0};
 CHECK(gk195_classic_country_view(loaded,0u,&derived)==GK195_OK);
 CHECK(derived.is_player&&derived.derived&&derived.actions==2u);
 CHECK(derived.original.tax==original_tax&&derived.current.tax==changed_tax);
 CHECK(derived.current.gold==derived.original.gold);
 CHECK(gk1_app_input(loaded,GK1_INPUT_SETUP)==1);
 CHECK(loaded->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(remove(path)==0);
 CHECK(gk187_pass(&a->classic_assign,&a->stock_checkpoint)==GK187_OK);
 CHECK(gk187_intact(&a->classic_assign,&a->stock_checkpoint));
 CHECK(gk189_save_new(path,a)==GK189_OK);
 CHECK(gk189_save_new(path,a)==GK189_EXISTS);
 CHECK(gk189_cold_load(path,csv,loaded)==GK189_OK);
 CHECK(loaded->screen==GK1_SCREEN_STOCK_COMMANDS);
 CHECK(loaded->stock_command_cursor==6u);
 CHECK(loaded->stock_checkpoint.view_index==a->stock_checkpoint.view_index);
 CHECK(loaded->stock_checkpoint.pending_pass==0u);
 CHECK(loaded->classic_assign.passed==1u&&loaded->classic_assign.action_count==2u);
 CHECK(loaded->classic_assign.budget.remaining==0u);
 CHECK(gk187_intact(&loaded->classic_assign,&loaded->stock_checkpoint));
 CHECK(!memcmp(loaded->classic_assign.mutable_world.countries,
   a->classic_assign.mutable_world.countries,
   sizeof a->classic_assign.mutable_world.countries));
 CHECK(!memcmp(loaded->stock_checkpoint.stock_raw,a->stock_checkpoint.stock_raw,
    sizeof a->stock_checkpoint.stock_raw));
 CHECK(!memcmp(loaded->stock_checkpoint.raw_sha256,a->stock_checkpoint.raw_sha256,32));
 CHECK(loaded->worlds[0].phase==GK1_PRE_SETUP_TEMPLATE&&
       loaded->worlds[1].phase==GK1_PRE_SETUP_TEMPLATE);
 CHECK(loaded->rules_profile==0u);
 /* A real controller A now lands on the source-attested zero-order VIEW,
  * rather than a stale Assign screen.  It must never refill the budget or
  * modify original country records. */
 Gk187ClassicAssign before_view=loaded->classic_assign;
 CHECK(gk1_app_input(loaded,GK1_INPUT_CONFIRM)==1);
 CHECK(loaded->screen==GK1_SCREEN_STOCK_CONTINUE);
 CHECK(loaded->stock_checkpoint.view_index==a->stock_checkpoint.view_index);
 CHECK(!memcmp(&loaded->classic_assign,&before_view,sizeof before_view));
 CHECK(gk187_intact(&loaded->classic_assign,&loaded->stock_checkpoint));
 /* Atomicity on incorrect canonical pre-setup template, even with valid file. */
 *before=*loaded;
 loaded->worlds[profile==3u?1u:0u].countries[0].gold++;
 Gk1App *mutated=malloc(sizeof *mutated);CHECK(mutated!=NULL);*mutated=*loaded;
 CHECK(gk189_cold_load(path,csv,loaded)==GK189_SOURCE);
 CHECK(!memcmp(mutated,loaded,sizeof *loaded));
 *loaded=*before;
 Gk1App *check=malloc(sizeof *check);CHECK(check!=NULL);*check=*loaded;
#ifndef _WIN32
 CHECK(chmod(path,0644)==0);
 CHECK(gk189_cold_load(path,csv,loaded)==GK189_IO);
 CHECK(!memcmp(loaded,check,sizeof *loaded));
 CHECK(chmod(path,0600)==0);
 char other[]="/tmp/gk189-hardlink-XXXXXX";
 f=mkstemp(other);CHECK(f>=0);CHECK(close(f)==0);CHECK(unlink(other)==0);
 CHECK(link(path,other)==0);
 CHECK(gk189_cold_load(path,csv,loaded)==GK189_IO);
 CHECK(!memcmp(loaded,check,sizeof *loaded));
 CHECK(unlink(other)==0);
 char sym[]="/tmp/gk189-symlink-XXXXXX";
 f=mkstemp(sym);CHECK(f>=0);CHECK(close(f)==0);CHECK(unlink(sym)==0);
 CHECK(symlink(path,sym)==0);
 CHECK(gk189_cold_load(sym,csv,loaded)==GK189_IO);
 CHECK(!memcmp(loaded,check,sizeof *loaded));
 CHECK(unlink(sym)==0);
#else
 /* Native Win32 link-count hostile control: same full cold-load function,
  * not only the lightweight file-guard unit test. This demands an NTFS
  * filesystem supporting hard links; otherwise record a skipped control. */
 char hard[1100];snprintf(hard,sizeof hard,"%s.link",path);
 if(CreateHardLinkA(hard,path,NULL)){
  CHECK(gk189_cold_load(path,csv,loaded)==GK189_IO);
  CHECK(!memcmp(loaded,check,sizeof *loaded));
  CHECK(DeleteFileA(hard)!=0);
 }
#endif
 /* Attack 1: unrelated byte in real original record. File SHA recomputed;
  * original-stock exact-profile SHA still rejects the changed payload. */
 FILE *fp=fopen(path,"r+b");CHECK(fp!=NULL);
 unsigned n=(profile==GK77_ARCHIVED_WORLD_SAVERAM?27u:14u);
 size_t body=64u+n*GK77_RECORD_BYTES;
 uint8_t *buf=malloc(body+32u);CHECK(buf!=NULL);
 CHECK(fread(buf,1,body+32u,fp)==body+32u);
 buf[64u+20u]^=1u;
 gk1_sha256_bytes(buf,body,buf+body);
 CHECK(fseek(fp,0,SEEK_SET)==0);
 CHECK(fwrite(buf,1,body+32u,fp)==body+32u);
 CHECK(fclose(fp)==0);
 CHECK(gk189_cold_load(path,csv,loaded)==GK189_SOURCE);
 CHECK(!memcmp(loaded,check,sizeof *loaded));
 /* Attack 2: legal source restored, action direction forged and file SHA
  * recomputed. Even a valid non-cryptographic checksum cannot force an
  * impossible worker transfer or partially mutate the current campaign. */
 fp=fopen(path,"r+b");CHECK(fp!=NULL);buf[64u+20u]^=1u;
 buf[48]=9u;buf[49]=9u;
 gk1_sha256_bytes(buf,body,buf+body);
 CHECK(fwrite(buf,1,body+32u,fp)==body+32u);
 CHECK(fclose(fp)==0);
 CHECK(gk189_cold_load(path,csv,loaded)==GK189_CORRUPT);
 CHECK(!memcmp(loaded,check,sizeof *loaded));
 CHECK(remove(path)==0);
 free(buf);free(mutated);free(check);free(before);free(loaded);free(a);
}
int main(int argc,char **argv){
 CHECK(argc==2||argc==5);
 Gk1TemplateDataset *ds=malloc(sizeof *ds);CHECK(ds!=NULL);
 CHECK(gk1_template_load(argv[1],ds)==GK1_LOAD_OK);
 public_negative(ds,argv[1]);
 if(argc==5){
  positive(ds,argv[1],argv[2],GK77_FRESH_MONGOL_WORKRAM);
  positive(ds,argv[1],argv[3],GK77_ARCHIVED_MONGOL_SAVERAM);
  positive(ds,argv[1],argv[4],GK77_ARCHIVED_WORLD_SAVERAM);
 }
 printf("GK189 %u %s source-bound Classic snapshot/ASSIGN/Pass cold-load assertions PASS; NOT original NES parity\n",checks,argc==5?"PRIVATE ORIGINAL THREE PROFILES":"PUBLIC NEGATIVE");
 free(ds);return 0;
}
