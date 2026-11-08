/* Copyright-clean portable state persistence for the verified original
 * 41-country PRE_SETUP_TEMPLATE. No NES ROM data or fabricated setup rules.
 * Values are encoded in a versioned, fixed-width little-endian format;
 * original historical identity/metadata come from the trusted v1.07 CSV.
 */
#include "gk1_pre_setup_store.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif
#define FIELDS 31u
#define HEAD 48u
#define COUNTRY_BYTES (FIELDS*2u)
static const uint8_t sha256_v107[32]={
 0xfa,0x39,0x58,0xf1,0x56,0xf0,0x9d,0x4b,0x4c,0x15,0xbb,0x55,0x9c,0x12,0xaa,0x35,
 0x7f,0xc6,0x17,0xa9,0xb1,0xf9,0x21,0x75,0x05,0xfe,0x07,0xe8,0x80,0x0d,0xe5,0x81};
static uint32_t crc32(const uint8_t *p,size_t n){
 uint32_t c=UINT32_C(0xffffffff);
 for(size_t i=0;i<n;i++){c^=p[i];for(int j=0;j<8;j++)c=(c>>1)^((c&1u)?UINT32_C(0xedb88320):0u);}
 return ~c;
}
static void word(uint8_t *p,uint16_t x){p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);}
static uint16_t unword(const uint8_t *p){return (uint16_t)(p[0]|((uint16_t)p[1]<<8));}
static void longword(uint8_t *p,uint32_t x){for(int j=0;j<4;j++)p[j]=(uint8_t)(x>>(8*j));}
static uint32_t unlong(const uint8_t *p){return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static void pack_fields(const Gk1CountryTemplate *c,uint16_t v[FIELDS]){
 size_t k=0;
 for(unsigned i=0;i<GK1_GOODS;i++)v[k++]=c->goods[i];
 v[k++]=c->arms;v[k++]=c->skill;v[k++]=c->troops;v[k++]=c->economy;
 v[k++]=c->town;v[k++]=c->defense;v[k++]=c->masons;
 v[k++]=c->commodity_history;v[k++]=c->artisans;
 v[k++]=c->food_history;v[k++]=c->food_workers;
 v[k++]=c->gold;v[k++]=c->morale;v[k++]=c->tax;v[k++]=c->extra_tax_used;
 v[k++]=c->undrafted_reserve;v[k++]=c->eligibility_percent;
 v[k++]=c->drafted_population;v[k++]=c->combined_population;
 v[k++]=c->eligible_draft_count;
}
static void unpack_fields(Gk1CountryTemplate *c,const uint16_t v[FIELDS]){
 size_t k=0;
 for(unsigned i=0;i<GK1_GOODS;i++)c->goods[i]=v[k++];
 c->arms=v[k++];c->skill=v[k++];c->troops=v[k++];c->economy=v[k++];
 c->town=v[k++];c->defense=v[k++];c->masons=v[k++];
 c->commodity_history=v[k++];c->artisans=v[k++];
 c->food_history=v[k++];c->food_workers=v[k++];
 c->gold=v[k++];c->morale=v[k++];c->tax=v[k++];c->extra_tax_used=v[k++];
 c->undrafted_reserve=v[k++];c->eligibility_percent=v[k++];
 c->drafted_population=v[k++];c->combined_population=v[k++];
 c->eligible_draft_count=v[k++];
}
static int valid_values(const Gk1CountryTemplate *c){
 uint16_t v[FIELDS];pack_fields(c,v);
 for(unsigned k=0;k<FIELDS;k++)if(v[k]>999u)return 0;
 if(c->tax>100u||c->eligibility_percent>100u||c->extra_tax_used>1u||
    c->drafted_population!=(uint32_t)c->troops+c->town+c->masons+c->artisans+c->food_workers||
    (uint32_t)c->drafted_population+c->undrafted_reserve!=c->combined_population||
    (uint32_t)c->undrafted_reserve*c->eligibility_percent/100u!=c->eligible_draft_count)return 0;
 return 1;
}
static int identical_source_identity(const Gk1CountryTemplate *a,const Gk1CountryTemplate *b){
 return a->scenario_id==b->scenario_id&&a->country_id==b->country_id&&a->index==b->index&&
 a->specialty_index==b->specialty_index&&
 !strcmp(a->country_name,b->country_name)&&!strcmp(a->lord_name,b->lord_name)&&
 !strcmp(a->specialty_name,b->specialty_name)&&
 a->country_sram_base==b->country_sram_base&&a->country_rom_offset==b->country_rom_offset&&
 a->specialty_sram_address==b->specialty_sram_address&&
 a->specialty_rom_offset==b->specialty_rom_offset&&a->name_rom_offset==b->name_rom_offset;
}
static int valid_layout(const Gk1App *s,const Gk1TemplateDataset *src){
 if(!s||!src||src->mongol_count!=14||src->world_count!=27||s->active_world>1||
  !s->world_ready[0]||!s->world_ready[1]||s->country_cursor[0]>=14||s->country_cursor[1]>=27)return 0;
 for(unsigned w=0;w<2;w++){
  const Gk1LoadedScenario *z=&s->worlds[w];unsigned count=w?27u:14u;
  if(z->scenario_id!=(Gk1ScenarioId)(w+1u)||z->phase!=GK1_PRE_SETUP_TEMPLATE||z->country_count!=count)return 0;
  for(unsigned j=0;j<count;j++){
    const Gk1CountryTemplate *c=&z->countries[j];
    const Gk1CountryTemplate *ref=&src->countries[j+(w?14u:0u)];
    if(!identical_source_identity(c,ref)||!valid_values(c))return 0;
  }
 }
 return 1;
}
Gk1StoreResult gk51_pre_setup_encode(const Gk1App *s,const Gk1TemplateDataset *src,
                                     uint8_t *out,size_t n){
 if(!out||n!=GK51_PRE_SETUP_BYTES||!valid_layout(s,src))return GK1_STORE_BAD;
 uint8_t b[GK51_PRE_SETUP_BYTES];memset(b,0,sizeof b);
 memcpy(b,"GK1PRE51",8);word(b+8,1);b[10]=1;b[11]=14;b[12]=27;
 b[13]=s->active_world;b[14]=s->country_cursor[0];b[15]=s->country_cursor[1];
 memcpy(b+16,sha256_v107,32);
 size_t offset=HEAD;
 for(unsigned w=0;w<2;w++)for(unsigned j=0;j<(w?27u:14u);j++){
  uint16_t vals[FIELDS];pack_fields(&s->worlds[w].countries[j],vals);
  for(unsigned k=0;k<FIELDS;k++){word(b+offset,vals[k]);offset+=2;}
 }
 if(offset!=GK51_PRE_SETUP_BYTES-4u)return GK1_STORE_BAD;
 longword(b+offset,crc32(b,offset));memcpy(out,b,sizeof b);return GK1_STORE_OK;
}
Gk1StoreResult gk51_pre_setup_decode(const uint8_t *bytes,size_t size,
                                     const Gk1TemplateDataset *src,Gk1App *s){
 if(!bytes||size!=GK51_PRE_SETUP_BYTES||!src||!s)return GK1_STORE_BAD;
 if(memcmp(bytes,"GK1PRE51",8)||unword(bytes+8)!=1||bytes[10]!=1||
    bytes[11]!=14||bytes[12]!=27||bytes[13]>1||bytes[14]>=14||bytes[15]>=27||
    memcmp(bytes+16,sha256_v107,32)||
    unlong(bytes+size-4)!=crc32(bytes,size-4))return GK1_STORE_CORRUPT;
 Gk1App *next=malloc(sizeof *next);
 if(!next)return GK1_STORE_IO;
 *next=*s; /* Retain battle/replay and unrelated UI state */
 if(!gk1_app_install_templates(next,src)){free(next);return GK1_STORE_CORRUPT;}
 size_t offset=HEAD;
 for(unsigned w=0;w<2;w++)for(unsigned j=0;j<(w?27u:14u);j++){
  uint16_t vals[FIELDS];
  for(unsigned k=0;k<FIELDS;k++){vals[k]=unword(bytes+offset);offset+=2;}
  unpack_fields(&next->worlds[w].countries[j],vals);
 }
 next->active_world=bytes[13];next->country_cursor[0]=bytes[14];next->country_cursor[1]=bytes[15];
 if(!valid_layout(next,src)){free(next);return GK1_STORE_CORRUPT;}
 next->screen=GK1_SCREEN_SCENARIO_COUNTRIES;
 strcpy(next->status,"PRE_SETUP SNAPSHOT RESTORED; NOT PLAYABLE SPRING");
 *s=*next;free(next);return GK1_STORE_OK;
}
static int valid_path(const char *p){
 if(!p)return 0;
 size_t i=0;
 while(i<1024&&p[i])i++;
 return i>0&&i<1024;
}
Gk1StoreResult gk51_pre_setup_save_new(const char *file,const Gk1App *s,const Gk1TemplateDataset *src){
 if(!valid_path(file))return GK1_STORE_BAD;
 uint8_t b[GK51_PRE_SETUP_BYTES];Gk1StoreResult r=gk51_pre_setup_encode(s,src,b,sizeof b);
 if(r!=GK1_STORE_OK)return r;
#ifdef _WIN32
 HANDLE f=CreateFileA(file,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_FILE_EXISTS||GetLastError()==ERROR_ALREADY_EXISTS?GK1_STORE_EXISTS:GK1_STORE_IO;
 DWORD done=0;int ok=WriteFile(f,b,(DWORD)sizeof b,&done,NULL)&&done==sizeof b&&FlushFileBuffers(f);
 if(!CloseHandle(f))ok=0;
 if(!ok){DeleteFileA(file);return GK1_STORE_IO;}
#else
 int fd=open(file,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return errno==EEXIST?GK1_STORE_EXISTS:GK1_STORE_IO;
 size_t off=0;int ok=1;
 while(off<sizeof b){ssize_t d=write(fd,b+off,sizeof b-off);
  if(d<0&&errno==EINTR)continue;
  if(d<=0){ok=0;break;}off+=(size_t)d;}
 if(ok&&fsync(fd)!=0)ok=0;
 if(close(fd)!=0)ok=0;
 if(!ok){unlink(file);return GK1_STORE_IO;}
#endif
 return GK1_STORE_OK;
}
Gk1StoreResult gk51_pre_setup_load(const char *file,const Gk1TemplateDataset *src,Gk1App *s){
 if(!valid_path(file)||!src||!s)return GK1_STORE_BAD;
 uint8_t b[GK51_PRE_SETUP_BYTES+1];size_t n=0;
#ifdef _WIN32
 HANDLE f=CreateFileA(file,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 if(f==INVALID_HANDLE_VALUE)return GK1_STORE_IO;
 DWORD d=0;int ok=ReadFile(f,b,(DWORD)sizeof b,&d,NULL);n=d;
 if(!CloseHandle(f))ok=0;
 if(!ok)return GK1_STORE_IO;
#else
 int fd=open(file,O_RDONLY);if(fd<0)return GK1_STORE_IO;
 int ok=1;while(n<sizeof b){ssize_t d=read(fd,b+n,sizeof b-n);
  if(d<0&&errno==EINTR)continue;
  if(d<0){ok=0;break;}if(!d)break;n+=(size_t)d;}
 if(close(fd)!=0)ok=0;
 if(!ok)return GK1_STORE_IO;
#endif
 if(n!=GK51_PRE_SETUP_BYTES)return GK1_STORE_CORRUPT;
 return gk51_pre_setup_decode(b,n,src,s);
}
