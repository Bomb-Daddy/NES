/* EXEC0046. Native app event-script save using verified EXEC0045 mod-only actions.
 * Transactional decode replays *all* actions against source-bounded C11 model;
 * CRC and ROM hash metadata prevent accidents, not malicious authentication.
 */
#include "gk1_path_store.h"
#include <errno.h>
#include <string.h>
#ifdef _WIN32
# include <windows.h>
#else
# include <fcntl.h>
# include <unistd.h>
#endif
static const uint8_t magic[8]={'G','K','1','O','P','0','4','6'};
static const uint8_t modsha[32]={
 0x47,0x64,0xd4,0x63,0x59,0x58,0x1b,0xe1,0xe7,0xd1,0x2b,0xcb,0x85,0xbd,0x37,0x5c,
 0x84,0xcd,0x44,0x97,0xd1,0x08,0x4d,0x9d,0x30,0x6a,0x6e,0x88,0x4a,0xfb,0xe7,0xc5};
static uint32_t crc32(const uint8_t *p,size_t n){
 uint32_t c=UINT32_MAX;
 for(size_t i=0;i<n;i++){
  c^=p[i];for(int bit=0;bit<8;bit++)c=(c>>1)^(UINT32_C(0xEDB88320)&(0u-(c&1u)));
 }
 return ~c;
}
static void save_crc(uint8_t *b){uint32_t v=crc32(b,60);for(int i=0;i<4;i++)b[60+i]=(uint8_t)(v>>(8*i));}
static int exact_state(const Gk45Turn *a,const Gk45Turn *b){
 return a->fixture==b->fixture && a->phase==b->phase && a->soldiers==b->soldiers &&
 a->ally1_soldiers==b->ally1_soldiers && a->mobility==b->mobility &&
 a->completion_word==b->completion_word && a->x==b->x && a->y==b->y &&
 a->ally1_x==b->ally1_x && a->ally1_y==b->ally1_y &&
 a->steps==b->steps && a->rng48==b->rng48;
}
int gk46_path_encode(const Gk1App *app,uint8_t *dst,size_t len){
 if(!app||!dst||len!=GK46_PATH_BYTES||app->screen!=GK1_SCREEN_PATH_BATTLE||
    app->path_case>2||app->path_event_count>4)return 0;
 Gk1App reconstructed;
 if(!gk1_app_restore_path(&reconstructed,(uint8_t)(app->path_case+1),app->path_events,app->path_event_count)
   ||!exact_state(&reconstructed.path,&app->path))return 0;
 uint8_t b[GK46_PATH_BYTES]={0};memcpy(b,magic,8);b[8]=1;
 b[9]=(uint8_t)(app->path_case+1);b[10]=app->path_event_count;
 memcpy(b+11,app->path_events,app->path_event_count);
 memcpy(b+16,modsha,32);save_crc(b);memcpy(dst,b,sizeof b);return 1;
}
int gk46_path_decode(const uint8_t *src,size_t len,Gk1App *out){
 if(!src||!out||len!=GK46_PATH_BYTES||memcmp(src,magic,8)||src[8]!=1||
    src[9]<1||src[9]>3||src[10]>4||src[15]!=0||memcmp(src+16,modsha,32))return 0;
 for(size_t i=11+src[10];i<15;i++)if(src[i])return 0;
 for(size_t i=48;i<60;i++)if(src[i])return 0;
 uint32_t expected=crc32(src,60),recorded=0;
 for(int i=0;i<4;i++)recorded|=(uint32_t)src[60+i]<<(8*i);
 if(expected!=recorded)return 0;
 Gk1App result;
 if(!gk1_app_restore_path(&result,src[9],src+11,src[10]))return 0;
 uint8_t canonical[GK46_PATH_BYTES];
 if(!gk46_path_encode(&result,canonical,sizeof canonical)||memcmp(src,canonical,sizeof canonical))return 0;
 *out=result;return 1;
}
static int valid_path(const char *s){
 if(!s||!*s)return 0;
 size_t n=0;
 while(n<1024&&s[n])n++;
 return n>0&&n<1024;
}
Gk1StoreResult gk46_path_save_new(const char *path,const Gk1App *app){
 if(!valid_path(path))return GK1_STORE_BAD;
 uint8_t b[GK46_PATH_BYTES];if(!gk46_path_encode(app,b,sizeof b))return GK1_STORE_BAD;
#ifdef _WIN32
 HANDLE h=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
 if(h==INVALID_HANDLE_VALUE){DWORD e=GetLastError();return e==ERROR_FILE_EXISTS||e==ERROR_ALREADY_EXISTS?GK1_STORE_EXISTS:GK1_STORE_IO;}
 DWORD count=0;int ok=WriteFile(h,b,(DWORD)sizeof b,&count,NULL)&&count==sizeof b&&FlushFileBuffers(h);
 if(!CloseHandle(h))ok=0;
 if(!ok){DeleteFileA(path);return GK1_STORE_IO;}
#else
 int fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return errno==EEXIST?GK1_STORE_EXISTS:GK1_STORE_IO;
 size_t offset=0;int ok=1;
 while(offset<sizeof b){ssize_t n=write(fd,b+offset,sizeof b-offset);
  if(n<0&&errno==EINTR)continue;
  if(n<=0){ok=0;break;}
  offset+=(size_t)n;}
 if(ok&&fsync(fd))ok=0;
 if(close(fd))ok=0;
 if(!ok){unlink(path);return GK1_STORE_IO;}
#endif
 return GK1_STORE_OK;
}
Gk1StoreResult gk46_path_load(const char *path,Gk1App *out){
 if(!valid_path(path)||!out)return GK1_STORE_BAD;
 uint8_t b[GK46_PATH_BYTES+1];size_t n=0;
#ifdef _WIN32
 HANDLE h=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 if(h==INVALID_HANDLE_VALUE)return GK1_STORE_IO;
 DWORD read_n=0;int ok=ReadFile(h,b,(DWORD)sizeof b,&read_n,NULL);n=(size_t)read_n;
 if(!CloseHandle(h))ok=0;
 if(!ok)return GK1_STORE_IO;
#else
 int fd=open(path,O_RDONLY);if(fd<0)return GK1_STORE_IO;
 int ok=1;while(n<sizeof b){ssize_t k=read(fd,b+n,sizeof b-n);
  if(k<0&&errno==EINTR)continue;
  if(k<0){ok=0;break;}
  if(!k)break;
  n+=(size_t)k;}
 if(close(fd))ok=0;
 if(!ok)return GK1_STORE_IO;
#endif
 Gk1App candidate;
 if(n!=GK46_PATH_BYTES||!gk46_path_decode(b,n,&candidate))return GK1_STORE_CORRUPT;
 *out=candidate;return GK1_STORE_OK;
}
