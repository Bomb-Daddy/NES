/* EXEC0042: Linux/Windows native filesystem adapter for source-bounded research receipts.
 * No ROM bytes, images, audio, SRAM or private campaign data are embedded.
 */
#include "gk1_replay_store.h"
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

static int valid_path(const char *path){
 if(!path || !*path) return 0;
 /* Bound strlen without reading past a potentially missing terminator. */
 size_t n=0;while(n<1024 && path[n]) ++n;
 return n>0 && n<1024;
}
Gk1StoreResult gk1_store_save_new(const char *path,const Gk1BattleSession *session){
 if(!valid_path(path)||!session)return GK1_STORE_BAD;
 uint8_t buffer[GK1_SESSION_SAVE_BYTES];
 if(gk1_session_encode(session,buffer,sizeof buffer)!=GK1_SESSION_OK)return GK1_STORE_BAD;
#ifdef _WIN32
 HANDLE f=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_FILE_EXISTS||GetLastError()==ERROR_ALREADY_EXISTS?GK1_STORE_EXISTS:GK1_STORE_IO;
 DWORD written=0;
 int ok=WriteFile(f,buffer,(DWORD)sizeof buffer,&written,NULL)&&written==sizeof buffer&&FlushFileBuffers(f);
 if(!CloseHandle(f))ok=0;
 if(!ok){DeleteFileA(path);return GK1_STORE_IO;}
#else
 int fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return errno==EEXIST?GK1_STORE_EXISTS:GK1_STORE_IO;
 size_t offset=0;int ok=1;
 while(offset<sizeof buffer){ssize_t n=write(fd,buffer+offset,sizeof buffer-offset);
  if(n<0&&errno==EINTR)continue;
  if(n<=0){ok=0;break;}offset+=(size_t)n;}
 if(ok&&fsync(fd)!=0)ok=0;
 if(close(fd)!=0)ok=0;
 if(!ok){unlink(path);return GK1_STORE_IO;}
#endif
 return GK1_STORE_OK;
}
Gk1StoreResult gk1_store_load(const char *path,Gk1BattleSession *session){
 if(!valid_path(path)||!session)return GK1_STORE_BAD;
 uint8_t buffer[GK1_SESSION_SAVE_BYTES+1];size_t count=0;
#ifdef _WIN32
 HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE)return GK1_STORE_IO;
 DWORD n=0;int ok=ReadFile(f,buffer,(DWORD)sizeof buffer,&n,NULL);
 count=(size_t)n;if(!CloseHandle(f))ok=0;
 if(!ok)return GK1_STORE_IO;
#else
 int fd=open(path,O_RDONLY);
 if(fd<0)return GK1_STORE_IO;
 int ok=1;
 while(count<sizeof buffer){ssize_t n=read(fd,buffer+count,sizeof buffer-count);
  if(n<0&&errno==EINTR)continue;
  if(n<0){ok=0;break;}if(n==0)break;count+=(size_t)n;}
 if(close(fd)!=0)ok=0;
 if(!ok)return GK1_STORE_IO;
#endif
 if(count!=GK1_SESSION_SAVE_BYTES)return GK1_STORE_CORRUPT;
 Gk1BattleSession candidate;
 if(gk1_session_decode(buffer,count,&candidate)!=GK1_SESSION_OK)return GK1_STORE_CORRUPT;
 *session=candidate;return GK1_STORE_OK;
}
