/* Historic NES setup-step input persistence in the inherited native game.
 * Original ROM stat allocation, CPU player setup and Spring are NOT executed.
 */
#include "gk1_manual_setup_store.h"
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
static void u32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(i*8));}
static uint32_t read32(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static int valid_manual_setup(const Gk1App *a){
 if(!a||a->active_world>1||a->rules_profile>1)return 0;
 for(unsigned w=0;w<2;w++){
  unsigned count=a->setup_player_count[w];
  if(count<1||count>(w?4u:1u)||a->setup_difficulty[w]<1||a->setup_difficulty[w]>5||
   a->setup_stage[w]>GK53_SETUP_AWAIT_SOURCE||a->setup_player_slot[w]>count||
   a->setup_ability_method[w]>2||a->setup_method_cursor[w]>1)return 0;
  if(a->setup_stage[w]>GK53_SETUP_PLAYERS &&
    !gk53_setup_allowed_lord((uint8_t)w,a->setup_candidate[w]))return 0;
  if(a->setup_stage[w]==GK53_SETUP_AWAIT_SOURCE && !a->setup_ability_method[w])return 0;
  if(a->setup_stage[w]<GK53_SETUP_AWAIT_SOURCE && a->setup_ability_method[w])return 0;
  if(a->setup_stage[w]>=GK53_SETUP_ABILITIES && a->setup_player_slot[w]!=count)return 0;
  if(a->setup_stage[w]<GK53_SETUP_LORD && a->setup_player_slot[w])return 0;
  for(unsigned i=0;i<4;i++){
   uint8_t idx=a->setup_rulers[w][i];
   if(idx!=255 && (!gk53_setup_allowed_lord((uint8_t)w,idx) || i>=count))return 0;
   for(unsigned j=0;j<i;j++)if(idx!=255&&idx==a->setup_rulers[w][j])return 0;
   if(i==0&&idx!=255&&idx!=a->setup_candidate[w])return 0;
   if(a->setup_stage[w]>=GK53_SETUP_ABILITIES && i<count && idx==255)return 0;
   if(a->setup_stage[w]==GK53_SETUP_LORD){
    if(i<a->setup_player_slot[w] && idx==255)return 0;
    if(i>=a->setup_player_slot[w] && idx!=255)return 0;
   }
   if(a->setup_stage[w]<GK53_SETUP_LORD && idx!=255)return 0;
  }
 }
 return 1;
}
Gk1StoreResult gk53_setup_encode(const Gk1App *a,const Gk1TemplateDataset *source,
                                 uint8_t *out,size_t n){
 if(!a||!source||!out||n!=GK53_PRE_SETUP_BYTES||!valid_manual_setup(a))return GK1_STORE_BAD;
 uint8_t b[GK53_PRE_SETUP_BYTES]={0};
 Gk1StoreResult r=gk52_setup_encode(a,source,b,GK52_PRE_SETUP_BYTES);
 if(r!=GK1_STORE_OK)return r;
 unsigned o=GK52_PRE_SETUP_BYTES;
 memcpy(b+o,"GK1DRF53",8);b[o+8]=1;
 b[o+9]=a->setup_player_count[0];b[o+10]=a->setup_player_count[1];
 b[o+11]=a->setup_difficulty[0];b[o+12]=a->setup_difficulty[1];
 b[o+13]=a->setup_stage[0];b[o+14]=a->setup_stage[1];
 b[o+15]=a->setup_player_slot[0];b[o+16]=a->setup_player_slot[1];
 b[o+17]=a->setup_ability_method[0];b[o+18]=a->setup_ability_method[1];
 b[o+19]=a->setup_method_cursor[0];b[o+20]=a->setup_method_cursor[1];
 memcpy(b+o+21,a->setup_rulers[0],4);memcpy(b+o+25,a->setup_rulers[1],4);
 u32(b+n-4,crc32(b,n-4));memcpy(out,b,n);return GK1_STORE_OK;
}
Gk1StoreResult gk53_setup_decode(const uint8_t *data,size_t n,
                                 const Gk1TemplateDataset *source,Gk1App *app){
 if(!data||!source||!app)return GK1_STORE_BAD;
 if(n==GK51_PRE_SETUP_BYTES||n==GK52_PRE_SETUP_BYTES)
  return gk52_setup_decode(data,n,source,app);
 if(n!=GK53_PRE_SETUP_BYTES)return GK1_STORE_CORRUPT;
 size_t o=GK52_PRE_SETUP_BYTES;
 if(memcmp(data+o,"GK1DRF53",8)||data[o+8]!=1||read32(data+n-4)!=crc32(data,n-4))return GK1_STORE_CORRUPT;
 for(size_t i=o+29;i<o+32;i++)if(data[i]!=0)return GK1_STORE_CORRUPT;
 Gk1App next=*app;
 Gk1StoreResult r=gk52_setup_decode(data,GK52_PRE_SETUP_BYTES,source,&next);
 if(r!=GK1_STORE_OK)return r;
 next.setup_player_count[0]=data[o+9];next.setup_player_count[1]=data[o+10];
 next.setup_difficulty[0]=data[o+11];next.setup_difficulty[1]=data[o+12];
 next.setup_stage[0]=data[o+13];next.setup_stage[1]=data[o+14];
 next.setup_player_slot[0]=data[o+15];next.setup_player_slot[1]=data[o+16];
 next.setup_ability_method[0]=data[o+17];next.setup_ability_method[1]=data[o+18];
 next.setup_method_cursor[0]=data[o+19];next.setup_method_cursor[1]=data[o+20];
 memcpy(next.setup_rulers[0],data+o+21,4);memcpy(next.setup_rulers[1],data+o+25,4);
 if(!valid_manual_setup(&next))return GK1_STORE_CORRUPT;
 next.screen=next.setup_stage[next.active_world]>GK53_SETUP_PLAYERS?
  GK1_SCREEN_SETUP_WIZARD:GK1_SCREEN_SCENARIO_COUNTRIES;
 strcpy(next.status,"MANUAL-VERIFIED SETUP DRAFT RESTORED; NOT PLAYABLE SPRING");
 *app=next;return GK1_STORE_OK;
}
static int valid_path(const char *p){
 if(!p)return 0;
 size_t i=0;while(i<1024&&p[i])i++;
 return i>0&&i<1024;
}
Gk1StoreResult gk53_setup_save_new(const char *path,const Gk1App *app,
                                   const Gk1TemplateDataset *source){
 if(!valid_path(path))return GK1_STORE_BAD;
 uint8_t b[GK53_PRE_SETUP_BYTES];Gk1StoreResult r=gk53_setup_encode(app,source,b,sizeof b);
 if(r!=GK1_STORE_OK)return r;
#ifdef _WIN32
 HANDLE f=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE){DWORD e=GetLastError();return e==ERROR_FILE_EXISTS||e==ERROR_ALREADY_EXISTS?GK1_STORE_EXISTS:GK1_STORE_IO;}
 DWORD written=0;int ok=WriteFile(f,b,(DWORD)sizeof b,&written,NULL)&&written==sizeof b&&FlushFileBuffers(f);
 if(!CloseHandle(f))ok=0;
 if(!ok){DeleteFileA(path);return GK1_STORE_IO;}
#else
 int fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return errno==EEXIST?GK1_STORE_EXISTS:GK1_STORE_IO;
 size_t off=0;int ok=1;
 while(off<sizeof b){ssize_t nwrite=write(fd,b+off,sizeof b-off);
  if(nwrite<0&&errno==EINTR)continue;
  if(nwrite<=0){ok=0;break;}off+=(size_t)nwrite;}
 if(ok&&fsync(fd)!=0)ok=0;
 if(close(fd)!=0)ok=0;
 if(!ok){unlink(path);return GK1_STORE_IO;}
#endif
 return GK1_STORE_OK;
}
Gk1StoreResult gk53_setup_load(const char *path,const Gk1TemplateDataset *source,Gk1App *app){
 if(!valid_path(path)||!source||!app)return GK1_STORE_BAD;
 uint8_t b[GK53_PRE_SETUP_BYTES+1];size_t n=0;
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
 return gk53_setup_decode(b,n,source,app);
}
