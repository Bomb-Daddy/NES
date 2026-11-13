/* EXEC0259: Native Win32 LocalAppData path contract, hostile negatives. */
#include "gk59_storage.h"
#include <stdio.h>
#include <string.h>
#define CK(x) do {if(!(x)){fprintf(stderr,"GK259 FAIL %d: %s\n",__LINE__,#x);return 1;} ++checks;}while(0)
int main(void){
 unsigned checks=0;
 char output[1024];
 const char *root="C:\\Users\\player\\AppData\\Local";
 const char *expect="C:\\Users\\player\\AppData\\Local\\GK1NativeResearch\\gk1_enhanced_assign_slot1.gk16";
 CK(gk59_user_file(root,"gk1_enhanced_assign_slot1.gk16",output,sizeof output));
 CK(strcmp(output,expect)==0);
 CK(gk59_user_file("C:\\Users\\player\\AppData\\Local\\","gk1_enhanced_assign_slot1.gk16",output,sizeof output));
 CK(strcmp(output,expect)==0);
 strcpy(output,"UNCHANGED");
 CK(!gk59_user_file(root,"../escape.gk16",output,sizeof output));
 CK(!strcmp(output,"UNCHANGED"));
 CK(!gk59_user_file(root,"a/b.gk16",output,sizeof output));
 CK(!gk59_user_file(root,"a\\b.gk16",output,sizeof output));
 CK(!gk59_user_file(root,"a b.gk16",output,sizeof output));
 CK(!gk59_user_file(root,"",output,sizeof output));
 CK(!gk59_user_file(NULL,"valid.gk16",output,sizeof output));
 CK(!gk59_user_file(root,NULL,output,sizeof output));
 CK(!gk59_user_file(root,"valid.gk16",NULL,sizeof output));
 CK(!gk59_user_file(root,"valid.gk16",output,4));
 CK(!strcmp(output,"UNCHANGED"));
 puts("GK1 EXEC0259 Windows LocalAppData pure C11 path-policy PASS; native WinMain and physical Xbox not implied");
 printf("checks=%u\n",checks);return 0;
}
