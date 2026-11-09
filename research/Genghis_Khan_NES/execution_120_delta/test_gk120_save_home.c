#define _POSIX_C_SOURCE 200809L
#include "gk120_save_home.h"
#include "gk58_slots.h"
#include "gk59_storage.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

int main(void){
 char buffer[1100];strcpy(buffer,"UNTOUCHED");
 /* XDG has precedence; explicit absolute path means CWD is irrelevant. */
 assert(gk120_linux_path("/tmp/xdg-data","/home/player","gk1_enhanced_assign_slot1.gk16",buffer,sizeof buffer));
 assert(!strcmp(buffer,"/tmp/xdg-data/gk1-native-research/gk1_enhanced_assign_slot1.gk16"));
 assert(gk120_linux_path("relative/xdg","/home/player","gk1_stock_checkpoint_full_slot1.gk8",buffer,sizeof buffer));
 assert(!strcmp(buffer,"/home/player/.local/share/gk1-native-research/gk1_stock_checkpoint_full_slot1.gk8"));
 assert(gk120_linux_path(NULL,"/home/player/","gk1_presetup_slot1.gkp",buffer,sizeof buffer));
 assert(!strcmp(buffer,"/home/player/.local/share/gk1-native-research/gk1_presetup_slot1.gkp"));
 assert(gk120_linux_path("/tmp/xdg-data///",NULL,"gk1_research_slot1.gkr",buffer,sizeof buffer));
 assert(!strcmp(buffer,"/tmp/xdg-data/gk1-native-research/gk1_research_slot1.gkr"));
 char tiny[8]; assert(!gk58_slot_path(buffer,3,tiny,sizeof tiny)); /* insufficient 3-slot capacity */
 char slots[1100];assert(gk58_slot_path(buffer,3,slots,sizeof slots));
 assert(!strcmp(slots,"/tmp/xdg-data/gk1-native-research/gk1_research_slot1.gkr.slot3"));
 assert(!gk120_linux_path("bad","relative","okay.gk",buffer,sizeof buffer));
 assert(!gk120_linux_path(NULL,NULL,"okay.gk",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe/../unsafe",NULL,"okay.gk",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe/./unsafe",NULL,"okay.gk",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe",NULL,"../escape",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe",NULL,"a/b",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe",NULL,"a\\b",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe",NULL,"a b",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe",NULL,"",buffer,sizeof buffer));
 assert(!gk120_linux_path("/safe",NULL,"okay.gk",buffer,3));
 assert(!gk120_linux_path("/safe",NULL,"okay.gk",NULL,sizeof buffer));
 assert(!strcmp(buffer,"/tmp/xdg-data/gk1-native-research/gk1_research_slot1.gkr"));
 /* Build real private user hierarchy, then create an actual non-CWD save file. */
 char tmp[]="/tmp/gk120_storage_XXXXXX";
 char *root=mkdtemp(tmp);assert(root);
 char data_home[1024];snprintf(data_home,sizeof data_home,"%s/xdg/custom",root);
 assert(gk120_linux_path(data_home,NULL,"gk1_enhanced_assign_slot1.gk16",buffer,sizeof buffer));
 assert(gk120_prepare_linux_dir(buffer));
 assert(gk120_prepare_linux_dir(buffer)); /* idempotent */
 FILE *f=fopen(buffer,"wbx");assert(f);
 assert(fwrite("GK120",1,5,f)==5);assert(fclose(f)==0);
 f=fopen(buffer,"rb");assert(f);
 char b[8]={0};assert(fread(b,1,5,f)==5);assert(fclose(f)==0);
 assert(!strcmp(b,"GK120"));
 assert(gk58_slot_path(buffer,2,slots,sizeof slots));
 assert(gk120_prepare_linux_dir(slots));
 assert(!access(buffer,F_OK));
 assert(unlink(buffer)==0);
 /* Existing file as parent => must fail, no save path written. */
 FILE *marker=fopen(buffer,"wbx");assert(marker);assert(fclose(marker)==0);
 char nested[1300];snprintf(nested,sizeof nested,"%s/child/state.gk",buffer);
 assert(!gk120_prepare_linux_dir(nested));
 assert(unlink(buffer)==0);
 /* Reuse tested Win32 LocalAppData policy without pretending Windows ran. */
 assert(gk59_user_file("C:\\Users\\p\\AppData\\Local", "gk1_enhanced_assign_slot1.gk16",slots,sizeof slots));
 assert(!strcmp(slots,"C:\\Users\\p\\AppData\\Local\\GK1NativeResearch\\gk1_enhanced_assign_slot1.gk16"));
 /* Precise removal only of directories created in this test. */
 char dir[1024];snprintf(dir,sizeof dir,"%s/xdg/custom/gk1-native-research",root);
 assert(rmdir(dir)==0);
 snprintf(dir,sizeof dir,"%s/xdg/custom",root);assert(rmdir(dir)==0);
 snprintf(dir,sizeof dir,"%s/xdg",root);assert(rmdir(dir)==0);
 assert(rmdir(root)==0);
 puts("EXEC0120 TESTED_PORT_LINUX: absolute XDG/HOME slots, directory creation, direct-executable CWD independence, rollback, Windows path policy static contract PASS");
 return 0;
}
