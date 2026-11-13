#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk189_classic_cold_save.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif
#define CHECK(x) do{assert(x);checks++;}while(0)
static unsigned checks;
static void size_file(int fd,unsigned size){
#ifdef _WIN32
 CHECK(_chsize_s(fd,size)==0);
#else
 CHECK(ftruncate(fd,(off_t)size)==0);
#endif
}
int main(void){
 char path[1024];int fd=-1;
#ifdef _WIN32
 char dir[MAX_PATH],temp[MAX_PATH];
 DWORD n=GetTempPathA(MAX_PATH,dir);CHECK(n>0u&&n<MAX_PATH);
 CHECK(GetTempFileNameA(dir,"g48",0,temp)!=0);
 CHECK(strlen(temp)<sizeof path);strcpy(path,temp);
 CHECK(_sopen_s(&fd,path,_O_RDWR|_O_BINARY,_SH_DENYNO,0)==0);
#else
 strcpy(path,"/tmp/gk248-gc19-private-XXXXXX");
 fd=mkstemp(path);CHECK(fd>=0);
#endif
 /* All assertions are data-free. These are only accepted file envelopes,
  * never false claims that synthetic bytes are authenticated NES saves. */
 CHECK(!gk249_classic_cold_file_guard(fd,path));
 size_file(fd,64u+14u*83u+32u);
 CHECK(gk249_classic_cold_file_guard(fd,path));
 size_file(fd,64u+27u*83u+32u);
 CHECK(gk249_classic_cold_file_guard(fd,path));
 size_file(fd,64u+20u*83u+32u);
 CHECK(!gk249_classic_cold_file_guard(fd,path));
 size_file(fd,64u+27u*83u+33u);
 CHECK(!gk249_classic_cold_file_guard(fd,path));
 CHECK(!gk249_classic_cold_file_guard(-1,path));
 CHECK(!gk249_classic_cold_file_guard(fd,NULL));
#ifndef _WIN32
 size_file(fd,64u+14u*83u+32u);
 CHECK(fchmod(fd,0644)==0);
 CHECK(!gk249_classic_cold_file_guard(fd,path));
 CHECK(fchmod(fd,0600)==0);
 CHECK(gk249_classic_cold_file_guard(fd,path));
 char hard[1100];snprintf(hard,sizeof hard,"%s.link",path);
 CHECK(link(path,hard)==0);
 CHECK(!gk249_classic_cold_file_guard(fd,path));
 CHECK(unlink(hard)==0);
 CHECK(gk249_classic_cold_file_guard(fd,path));
 if(geteuid()==0){
  CHECK(fchown(fd,65534u,(gid_t)-1)==0);
  CHECK(!gk249_classic_cold_file_guard(fd,path));
  CHECK(fchown(fd,0u,(gid_t)-1)==0);
 }
 CHECK(close(fd)==0);CHECK(unlink(path)==0);
#else
 CHECK(_close(fd)==0);CHECK(DeleteFileA(path)!=0);
#endif
 printf("GK249 full Classic cold-save 14/27 private HANDLE guard: %u public checks pass; no original NES data\n",checks);
 return 0;
}
