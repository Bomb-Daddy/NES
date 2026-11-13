#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk247_receipt_file_guard.h"
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
#define CHECK(expr) do {assert(expr);checks++;} while(0)
static unsigned checks;
int main(void){
 char path[1024];
#ifdef _WIN32
 char dir[MAX_PATH],temp[MAX_PATH];
 DWORD n=GetTempPathA(MAX_PATH,dir);
 CHECK(n>0u&&n<MAX_PATH);
 CHECK(GetTempFileNameA(dir,"gc2",0,temp)!=0);
 CHECK(strlen(temp)<sizeof path);
 strcpy(path,temp);
 int fd=-1;
 CHECK(_sopen_s(&fd,path,_O_RDWR|_O_BINARY,_SH_DENYNO,0)==0);
#else
 strcpy(path,"/tmp/gk247-private-receipt-XXXXXX");
 int fd=mkstemp(path);
 CHECK(fd>=0);
#endif
 unsigned char b[96]={0};
#ifdef _WIN32
 CHECK(_write(fd,b,sizeof b)==sizeof b);
#else
 CHECK(write(fd,b,sizeof b)==(ssize_t)sizeof b);
#endif
 CHECK(gk247_receipt_file_guard(fd,path,96u));
 CHECK(!gk247_receipt_file_guard(fd,path,95u));
 CHECK(!gk247_receipt_file_guard(fd,path,97u));
 CHECK(!gk247_receipt_file_guard(-1,path,96u));
 CHECK(!gk247_receipt_file_guard(fd,NULL,96u));
 CHECK(!gk247_receipt_file_guard(fd,path,0u));
#ifdef _WIN32
 CHECK(_write(fd,b,1)==1);
 CHECK(!gk247_receipt_file_guard(fd,path,96u));
 CHECK(_close(fd)==0);
 CHECK(DeleteFileA(path)!=0);
#else
 CHECK(write(fd,b,1)==1);
 CHECK(!gk247_receipt_file_guard(fd,path,96u));
 CHECK(ftruncate(fd,96)==0);
 CHECK(fchmod(fd,0644)==0);
 CHECK(!gk247_receipt_file_guard(fd,path,96u));
 CHECK(fchmod(fd,0600)==0);
 /* Root-only hostile control: a foreign-owned file must be rejected.
  * Unprivileged Linux CI skips this branch without claiming it ran. */
 if(geteuid()==0){
  CHECK(fchown(fd,65534u,(gid_t)-1)==0);
  CHECK(!gk247_receipt_file_guard(fd,path,96u));
  CHECK(fchown(fd,0u,(gid_t)-1)==0);
 }
 char linkpath[1100];
 snprintf(linkpath,sizeof linkpath,"%s-link",path);
 CHECK(link(path,linkpath)==0);
 CHECK(!gk247_receipt_file_guard(fd,path,96u));
 CHECK(unlink(linkpath)==0);
 CHECK(gk247_receipt_file_guard(fd,path,96u));
 CHECK(close(fd)==0);
 CHECK(unlink(path)==0);
#endif
 printf("GK247 private receipt file guard: %u public platform assertions passed; no NES data used\n",checks);
 return 0;
}
