#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk226_practice_checkpoint.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

Gk224Status gk226_practice_checkpoint(const char *path,const char *csv,
 const Gk1LoadedScenario *source,const Gk222Spring *state,uint8_t command){
 if(!path||!*path||!csv||!gk1_verified_original_csv(csv))return GK224_SOURCE;
 uint8_t bytes[GK224_PRACTICE_BYTES];
 Gk224Status r=gk224_encode(source,state,command,bytes);
 if(r!=GK224_OK)return r;
 /* The selected slot is either absent or an independently verified prior
  * Enhanced-only save. A corrupt/mismatched slot is never overwritten. */
#ifdef _WIN32
 DWORD attributes=GetFileAttributesA(path);
 if(attributes==INVALID_FILE_ATTRIBUTES){
  if(GetLastError()==ERROR_FILE_NOT_FOUND||GetLastError()==ERROR_PATH_NOT_FOUND)
   return gk224_save_new(path,csv,source,state,command);
  return GK224_UNAVAILABLE;
 }
 if(attributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))
  return GK224_UNAVAILABLE;
#else
 struct stat prior;
 if(lstat(path,&prior)!=0){
  if(errno==ENOENT)return gk224_save_new(path,csv,source,state,command);
  return GK224_UNAVAILABLE;
 }
 if(!S_ISREG(prior.st_mode))return GK224_UNAVAILABLE;
#endif
 Gk222Spring old={0};uint8_t old_command=0;
 r=gk224_load(path,csv,source,&old,&old_command);
 if(r!=GK224_OK)return r;
 if(old.scenario!=state->scenario||old.player_index!=state->player_index)
  return GK224_SOURCE;
 size_t path_len=strlen(path);
 if(path_len>3900u)return GK224_UNAVAILABLE;
 char *temp=malloc(path_len+64u);
 if(!temp)return GK224_UNAVAILABLE;
 int ok=0;
#ifdef _WIN32
 HANDLE file=INVALID_HANDLE_VALUE;
 for(unsigned attempt=0;attempt<32u;attempt++){
  snprintf(temp,path_len+64u,"%s.gk226.%08lX.%02u.tmp",path,
   (unsigned long)GetCurrentProcessId(),attempt);
  file=CreateFileA(temp,GENERIC_WRITE,0,NULL,CREATE_NEW,
                   FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_WRITE_THROUGH,NULL);
  if(file!=INVALID_HANDLE_VALUE)break;
  if(GetLastError()!=ERROR_FILE_EXISTS&&GetLastError()!=ERROR_ALREADY_EXISTS)break;
 }
 if(file!=INVALID_HANDLE_VALUE){
  DWORD written=0;
  ok=WriteFile(file,bytes,sizeof bytes,&written,NULL)&&
      written==sizeof bytes&&FlushFileBuffers(file);
  if(!CloseHandle(file))ok=0;
  if(ok){
   DWORD now=GetFileAttributesA(path);
   ok=now!=INVALID_FILE_ATTRIBUTES&&
      !(now&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))&&
      MoveFileExA(temp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
  }
  if(!ok)DeleteFileA(temp);
 }
#else
 snprintf(temp,path_len+64u,"%s.gk226.XXXXXX",path);
 int fd=mkstemp(temp);
 if(fd>=0){
  if(fchmod(fd,0600)==0){
   FILE *f=fdopen(fd,"wb");
   if(f){
    ok=fwrite(bytes,1,sizeof bytes,f)==sizeof bytes;
    if(fflush(f)!=0||fsync(fileno(f))!=0)ok=0;
    if(fclose(f)!=0)ok=0;
   }else close(fd);
  }else close(fd);
  if(ok){
   struct stat now;
   if(lstat(path,&now)!=0||!S_ISREG(now.st_mode)||
      now.st_dev!=prior.st_dev||now.st_ino!=prior.st_ino||
      rename(temp,path)!=0)ok=0;
  }
  if(!ok)unlink(temp);
 }
#endif
 free(temp);
 return ok?GK224_OK:GK224_UNAVAILABLE;
}
