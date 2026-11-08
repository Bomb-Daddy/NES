#include "gk59_storage.h"
#include <string.h>
#include <stdint.h>
#include <ctype.h>
/* Directory is a trusted result of the Windows profile environment lookup.
 * Filename is a compile-time application name; reject path injections anyway.
 * Transactional output: on any invalid path, leave caller buffer unchanged. */
int gk59_user_file(const char *directory,const char *filename,
                   char *out,size_t capacity){
 if(!directory||!filename||!out||capacity==0)return 0;
 size_t dl=strlen(directory),fl=strlen(filename);
 if(dl==0||fl==0||dl>2048||fl>127)return 0;
 if(strstr(filename,"..")||strchr(filename,'/')||strchr(filename,'\\'))return 0;
 for(size_t i=0;i<fl;i++){
  unsigned char c=(unsigned char)filename[i];
  if(!isalnum(c)&&c!='.'&&c!='_'&&c!='-')return 0;
 }
 if(dl>1&&(directory[dl-1]=='/'||directory[dl-1]=='\\'))dl--;
 const char sep='\\';
 const char *folder="GK1NativeResearch";
 size_t dirlen=strlen(folder);
 if(dl>SIZE_MAX-dirlen-2 || dl+dirlen+2>SIZE_MAX-fl-1)return 0;
 size_t need=dl+1+dirlen+1+fl+1;
 if(need>capacity)return 0;
 /* Guard overlapping output vs input; tested paths use independent storage. */
 memcpy(out,directory,dl);out[dl]=sep;
 memcpy(out+dl+1,folder,dirlen);out[dl+1+dirlen]=sep;
 memcpy(out+dl+1+dirlen+1,filename,fl);
 out[need-1]='\0';return 1;
}
