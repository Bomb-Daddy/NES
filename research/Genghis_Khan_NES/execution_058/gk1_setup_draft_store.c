/* C11/Win32/Linux: persist explicit controller-input selection while keeping
 * the prior EXEC0051 source-validated record exact and backward compatible.
 * Neither the phase nor the original 41-country economics change here.
 */
#include "gk1_setup_draft_store.h"
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif
static uint32_t hash_crc32(const uint8_t *p,size_t n){
 uint32_t c=UINT32_C(0xffffffff);
 for(size_t i=0;i<n;i++){c^=p[i];for(unsigned j=0;j<8;j++)
  c=(c>>1)^((c&1u)?UINT32_C(0xedb88320):0u);}
 return ~c;
}
static void store_u32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(i*8));}
static uint32_t load_u32(const uint8_t *p){return (uint32_t)p[0] | ((uint32_t)p[1]<<8) | ((uint32_t)p[2]<<16) | ((uint32_t)p[3]<<24);}
static int valid_intent(const Gk1App *s){
 return s && s->rules_profile<=1 &&
        (s->setup_candidate[0]==255||s->setup_candidate[0]<14) &&
        (s->setup_candidate[1]==255||s->setup_candidate[1]<27);
}
Gk1StoreResult gk52_setup_encode(const Gk1App *s,const Gk1TemplateDataset *src,uint8_t *out,size_t n){
 if(!s||!src||!out||n!=GK52_PRE_SETUP_BYTES||!valid_intent(s))return GK1_STORE_BAD;
 uint8_t b[GK52_PRE_SETUP_BYTES];memset(b,0,sizeof b);
 Gk1StoreResult r=gk51_pre_setup_encode(s,src,b,GK51_PRE_SETUP_BYTES);
 if(r!=GK1_STORE_OK)return r;
 size_t offset=GK51_PRE_SETUP_BYTES;
 memcpy(b+offset,"GK1DRF52",8);
 b[offset+8]=1; /* extension version */
 b[offset+9]=s->rules_profile;
 b[offset+10]=s->setup_candidate[0];b[offset+11]=s->setup_candidate[1];
 /* 8 reserved zero bytes must stay zero; CRC covers both old payload and new. */
 store_u32(b+n-4,hash_crc32(b,n-4));
 memcpy(out,b,n);
 return GK1_STORE_OK;
}
Gk1StoreResult gk52_setup_decode(const uint8_t *data,size_t n,const Gk1TemplateDataset *src,Gk1App *s){
 if(!data||!src||!s)return GK1_STORE_BAD;
 if(n!=GK51_PRE_SETUP_BYTES&&n!=GK52_PRE_SETUP_BYTES)return GK1_STORE_CORRUPT;
 uint8_t profile=0,candidates[2]={255,255};
 if(n==GK52_PRE_SETUP_BYTES){
  size_t o=GK51_PRE_SETUP_BYTES;
  if(memcmp(data+o,"GK1DRF52",8)||data[o+8]!=1||data[o+9]>1 ||
     (data[o+10]!=255&&data[o+10]>=14) ||
     (data[o+11]!=255&&data[o+11]>=27) ||
     load_u32(data+n-4)!=hash_crc32(data,n-4))return GK1_STORE_CORRUPT;
  for(size_t j=o+12;j<o+20;j++)if(data[j]!=0)return GK1_STORE_CORRUPT;
  profile=data[o+9];candidates[0]=data[o+10];candidates[1]=data[o+11];
 }
 Gk1App next=*s;
 Gk1StoreResult r=gk51_pre_setup_decode(data,GK51_PRE_SETUP_BYTES,src,&next);
 if(r!=GK1_STORE_OK)return r;
 next.rules_profile=profile;
 next.setup_candidate[0]=candidates[0];next.setup_candidate[1]=candidates[1];
 *s=next;
 return GK1_STORE_OK;
}
static int valid_path(const char *path){
 if(!path)return 0;
 size_t i=0;while(i<1024&&path[i])i++;
 return i>0&&i<1024;
}
Gk1StoreResult gk52_setup_save_new(const char *path,const Gk1App *s,const Gk1TemplateDataset *src){
 if(!valid_path(path))return GK1_STORE_BAD;
 uint8_t data[GK52_PRE_SETUP_BYTES];
 Gk1StoreResult r=gk52_setup_encode(s,src,data,sizeof data);
 if(r!=GK1_STORE_OK)return r;
#ifdef _WIN32
 HANDLE f=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE){DWORD err=GetLastError();return (err==ERROR_FILE_EXISTS||err==ERROR_ALREADY_EXISTS)?GK1_STORE_EXISTS:GK1_STORE_IO;}
 DWORD wrote=0;int ok=WriteFile(f,data,(DWORD)sizeof data,&wrote,NULL)&&wrote==sizeof data&&FlushFileBuffers(f);
 if(!CloseHandle(f))ok=0;
 if(!ok){DeleteFileA(path);return GK1_STORE_IO;}
#else
 int fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return errno==EEXIST?GK1_STORE_EXISTS:GK1_STORE_IO;
 int ok=1;size_t off=0;
 while(off<sizeof data){ssize_t count=write(fd,data+off,sizeof data-off);
  if(count<0&&errno==EINTR)continue;
  if(count<=0){ok=0;break;}off+=(size_t)count;}
 if(ok&&fsync(fd)!=0)ok=0;
 if(close(fd)!=0)ok=0;
 if(!ok){unlink(path);return GK1_STORE_IO;}
#endif
 return GK1_STORE_OK;
}
Gk1StoreResult gk52_setup_load(const char *path,const Gk1TemplateDataset *src,Gk1App *s){
 if(!valid_path(path)||!src||!s)return GK1_STORE_BAD;
 uint8_t b[GK52_PRE_SETUP_BYTES+1];size_t n=0;
#ifdef _WIN32
 HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE)return GK1_STORE_IO;
 DWORD got=0;int ok=ReadFile(f,b,(DWORD)sizeof b,&got,NULL);n=got;
 if(!CloseHandle(f))ok=0;
 if(!ok)return GK1_STORE_IO;
#else
 int fd=open(path,O_RDONLY);if(fd<0)return GK1_STORE_IO;
 int ok=1;while(n<sizeof b){ssize_t got=read(fd,b+n,sizeof b-n);
  if(got<0&&errno==EINTR)continue;
  if(got<0){ok=0;break;}if(got==0)break;n+=(size_t)got;}
 if(close(fd)!=0)ok=0;
 if(!ok)return GK1_STORE_IO;
#endif
 return gk52_setup_decode(b,n,src,s);
}
