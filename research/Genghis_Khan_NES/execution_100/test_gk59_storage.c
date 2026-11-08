#include "gk59_storage.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
int main(void){
 char b[512];memset(b,'Z',sizeof b);
 assert(gk59_user_file("C:\\Users\\tester\\AppData\\Local", "gk1_presetup_slot1.gkp",b,sizeof b));
 assert(!strcmp(b,"C:\\Users\\tester\\AppData\\Local\\GK1NativeResearch\\gk1_presetup_slot1.gkp"));
 assert(gk59_user_file("C:\\Users\\tester\\AppData\\Local\\", "gk1_presetup_slot1.gkp",b,sizeof b));
 assert(!strcmp(b,"C:\\Users\\tester\\AppData\\Local\\GK1NativeResearch\\gk1_presetup_slot1.gkp"));
 strcpy(b,"UNMODIFIED");
 assert(!gk59_user_file(NULL,"save.gkp",b,sizeof b));assert(!strcmp(b,"UNMODIFIED"));
 assert(!gk59_user_file("","save.gkp",b,sizeof b));assert(!strcmp(b,"UNMODIFIED"));
 assert(!gk59_user_file("C:\\Valid","../broken.gkp",b,sizeof b));assert(!strcmp(b,"UNMODIFIED"));
 assert(!gk59_user_file("C:\\Valid","bad\\name.gkp",b,sizeof b));assert(!strcmp(b,"UNMODIFIED"));
 assert(!gk59_user_file("C:\\Valid","save.gkp",b,5));assert(!strcmp(b,"UNMODIFIED"));
 assert(!gk59_user_file("C:\\Valid","s/?",b,sizeof b));assert(!strcmp(b,"UNMODIFIED"));
 assert(gk59_user_file("D:","gk1_research_slot1.gkr",b,sizeof b));
 assert(!strcmp(b,"D:\\GK1NativeResearch\\gk1_research_slot1.gkr"));
 puts("EXEC0059 portable Windows profile path policy path policy: PASS (LINUX C11 ONLY)");return 0;
}
