/* EXEC0055: content-addressed prior 2654-byte setup preserved as prefix.
 * Manual point inputs are STAGED/DRAFT and cannot trigger NES seasons.
 */
#include "gk1_manual_bonus_store.h"
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif
static uint32_t crc32(const uint8_t *p,size_t n){
 uint32_t v=UINT32_C(0xffffffff);
 for(size_t i=0;i<n;i++){v^=p[i];for(unsigned j=0;j<8;j++)
  v=(v>>1)^((v&1u)?UINT32_C(0xedb88320):0u);}
 return ~v;
}
static void u32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static uint32_t read32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
unsigned gk55_bonus_spent(const Gk1App *a,unsigned w){
 if(!a||w>1)return 0;
 unsigned sum=0;
 for(unsigned p=0;p<5;p++)for(unsigned k=0;k<6;k++)sum+=a->manual_bonus[w][p][k];
 return sum;
}
static int valid_bonus(const Gk1App *a){
 if(!a)return 0;
 for(unsigned w=0;w<2;w++){
  if(a->manual_bonus_active[w]>1||a->manual_bonus_person[w]>4||a->manual_bonus_ability[w]>5)return 0;
  if(gk55_bonus_spent(a,w)>100)return 0;
  if(a->setup_stage[w]!=GK53_SETUP_AWAIT_SOURCE||a->setup_ability_method[w]!=2){
   if(a->manual_bonus_active[w]||a->manual_bonus_person[w]||a->manual_bonus_ability[w]||gk55_bonus_spent(a,w))return 0;
  }
  for(unsigned p=0;p<5;p++)for(unsigned k=0;k<6;k++)if(a->manual_bonus[w][p][k]>99)return 0;
 }
 return 1;
}
Gk1StoreResult gk55_setup_encode(const Gk1App *a,const Gk1TemplateDataset *source,uint8_t *out,size_t n){
 if(!a||!source||!out||n!=GK55_PRE_SETUP_BYTES||!valid_bonus(a))return GK1_STORE_BAD;
 uint8_t b[GK55_PRE_SETUP_BYTES]={0};
 Gk1StoreResult r=gk53_setup_encode(a,source,b,GK53_PRE_SETUP_BYTES);
 if(r!=GK1_STORE_OK)return r;
 size_t o=GK53_PRE_SETUP_BYTES;
 memcpy(b+o,"GK1AL055",8);b[o+8]=1;
 b[o+9]=a->manual_bonus_active[0];b[o+10]=a->manual_bonus_active[1];
 b[o+11]=a->manual_bonus_person[0];b[o+12]=a->manual_bonus_person[1];
 b[o+13]=a->manual_bonus_ability[0];b[o+14]=a->manual_bonus_ability[1];
 memcpy(b+o+15,a->manual_bonus[0],30);
 memcpy(b+o+45,a->manual_bonus[1],30);
 u32(b+n-4,crc32(b,n-4));memcpy(out,b,n);return GK1_STORE_OK;
}
Gk1StoreResult gk55_setup_decode(const uint8_t *data,size_t n,const Gk1TemplateDataset *source,Gk1App *app){
 if(!data||!source||!app)return GK1_STORE_BAD;
 if(n==GK51_PRE_SETUP_BYTES||n==GK52_PRE_SETUP_BYTES||n==GK53_PRE_SETUP_BYTES){
  Gk1App next=*app;
  Gk1StoreResult r=gk53_setup_decode(data,n,source,&next);
  if(r!=GK1_STORE_OK)return r;
  memset(next.manual_bonus,0,sizeof next.manual_bonus);
  memset(next.manual_bonus_active,0,sizeof next.manual_bonus_active);
  memset(next.manual_bonus_person,0,sizeof next.manual_bonus_person);
  memset(next.manual_bonus_ability,0,sizeof next.manual_bonus_ability);
  *app=next;return GK1_STORE_OK;
 }
 if(n!=GK55_PRE_SETUP_BYTES)return GK1_STORE_CORRUPT;
 size_t o=GK53_PRE_SETUP_BYTES;
 if(memcmp(data+o,"GK1AL055",8)||data[o+8]!=1||data[o+75]!=0||
    read32(data+n-4)!=crc32(data,n-4))return GK1_STORE_CORRUPT;
 Gk1App next=*app;
 Gk1StoreResult r=gk53_setup_decode(data,GK53_PRE_SETUP_BYTES,source,&next);
 if(r!=GK1_STORE_OK)return r;
 next.manual_bonus_active[0]=data[o+9];next.manual_bonus_active[1]=data[o+10];
 next.manual_bonus_person[0]=data[o+11];next.manual_bonus_person[1]=data[o+12];
 next.manual_bonus_ability[0]=data[o+13];next.manual_bonus_ability[1]=data[o+14];
 memcpy(next.manual_bonus[0],data+o+15,30);
 memcpy(next.manual_bonus[1],data+o+45,30);
 if(!valid_bonus(&next))return GK1_STORE_CORRUPT;
 *app=next;return GK1_STORE_OK;
}
static int valid_path(const char *p){
 if(!p)return 0;
 size_t n=0;while(n<1024&&p[n])n++;
 return n>0&&n<1024;
}
Gk1StoreResult gk55_setup_save_new(const char *path,const Gk1App *app,const Gk1TemplateDataset *source){
 if(!valid_path(path))return GK1_STORE_BAD;
 uint8_t b[GK55_PRE_SETUP_BYTES];Gk1StoreResult r=gk55_setup_encode(app,source,b,sizeof b);
 if(r!=GK1_STORE_OK)return r;
#ifdef _WIN32
 HANDLE f=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE){DWORD e=GetLastError();return e==ERROR_FILE_EXISTS||e==ERROR_ALREADY_EXISTS?GK1_STORE_EXISTS:GK1_STORE_IO;}
 DWORD done=0;int ok=WriteFile(f,b,(DWORD)sizeof b,&done,NULL)&&done==sizeof b&&FlushFileBuffers(f);
 if(!CloseHandle(f))ok=0;
 if(!ok){DeleteFileA(path);return GK1_STORE_IO;}
#else
 int fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return errno==EEXIST?GK1_STORE_EXISTS:GK1_STORE_IO;
 size_t off=0;int ok=1;
 while(off<sizeof b){ssize_t m=write(fd,b+off,sizeof b-off);
  if(m<0&&errno==EINTR)continue;
  if(m<=0){ok=0;break;}off+=(size_t)m;}
 if(ok&&fsync(fd)!=0)ok=0;
 if(close(fd)!=0)ok=0;
 if(!ok){unlink(path);return GK1_STORE_IO;}
#endif
 return GK1_STORE_OK;
}
Gk1StoreResult gk55_setup_load(const char *path,const Gk1TemplateDataset *source,Gk1App *app){
 if(!valid_path(path)||!source||!app)return GK1_STORE_BAD;
 uint8_t b[GK55_PRE_SETUP_BYTES+1];size_t n=0;
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
 return gk55_setup_decode(b,n,source,app);
}
