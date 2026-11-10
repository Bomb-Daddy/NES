#include "gk144_archived_mapper.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(v) do{checks++;if(!(v)){fprintf(stderr,"GK144 fail at %d: %s\n",__LINE__,#v);return 1;}}while(0)
static void put32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4u;i++)p[i]=(uint8_t)(v>>(8u*i));}
static void put64(uint8_t *p,uint64_t v){for(unsigned i=0;i<8u;i++)p[i]=(uint8_t)(v>>(8u*i));}
static int append(uint8_t *dst,size_t cap,size_t *pos,const char *name,const uint8_t *src,size_t bytes){
 size_t len=strlen(name);
 if(*pos+len+1u+4u+bytes>cap)return 0;
 memcpy(dst+*pos,name,len);*pos+=len;dst[(*pos)++]=0;
 put32(dst+*pos,(uint32_t)bytes);*pos+=4;
 memcpy(dst+*pos,src,bytes);*pos+=bytes;return 1;
}
static size_t synthetic(uint8_t *buf,size_t cap,unsigned low_bank){
 uint8_t cpu[2048]={0},work[8192]={0},save[8192]={0};
 uint8_t offset[1024]={0},type[1024]={0},access[1024]={0};
 uint8_t pc[2]={0xc9,0xef},cycles[8],mode[1]={1},slot[1]={1},reg[1]={(uint8_t)low_bank};
 work[0x0dcd]=5;put64(cycles,1234567u);
 for(unsigned page=0;page<256u;page++){
  put32(offset+4u*page,0xffffffffu);
  if(page>=0x80u){
   unsigned j=page-0x80u;
   put32(offset+4u*page,(j<64u?low_bank*16384u:15u*16384u)+(j%64u)*256u);
   put32(access+4u*page,1u);
  }
 }
 cpu[0x84]=1;size_t p=0;
#define ADD(k,v) do{if(!append(buf,cap,&p,k,v,sizeof(v)))return 0;}while(0)
 ADD("cpu.pc",pc);ADD("cpu.cycleCount",cycles);
 ADD("memoryManager.internalRam",cpu);ADD("mapper.workRam",work);ADD("mapper.saveRam",save);
 ADD("mapper.prgMemoryOffset",offset);ADD("mapper.prgMemoryType",type);
 ADD("mapper.prgMemoryAccess",access);ADD("mapper.prgMode",mode);
 ADD("mapper.slotSelect",slot);ADD("mapper.prgReg",reg);
#undef ADD
 return p;
}
static uint8_t *field_value(uint8_t *b,size_t n,const char *key){
 size_t sz=strlen(key);for(size_t i=0;i+sz+5u<n;i++)
  if(memcmp(b+i,key,sz)==0&&b[i+sz]==0)
   return b+i+sz+5u;
 return NULL;
}
static int file_check(const char *path,uint8_t expected){
 FILE *f=fopen(path,"rb");if(!f)return 0;
 uint8_t *b=malloc(16u*1024u*1024u);if(!b){fclose(f);return 0;}
 size_t n=fread(b,1,16u*1024u*1024u,f);int e=ferror(f),c=fclose(f);
 Gk144ArchivedMapper proof={0};
 Gk144Status status=e||c?GK144_MAPPER_BAD_INPUT:gk144_probe_archived_prg_map(b,n,&proof);
 free(b);
 if(status!=GK144_MAPPER_OK||proof.lower_prg_bank!=expected||proof.upper_prg_bank!=15u)return 0;
 printf("OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED low=%u high=%u saved_pc=$%04x cpu_cycles=%llu country_ram=%u; NOT FRESH RESET\n",
 proof.lower_prg_bank,proof.upper_prg_bank,proof.saved_program_counter,
 (unsigned long long)proof.saved_cpu_cycles,proof.observed_country_ram_bank);
 return 1;
}
int main(int argc,char **argv){
 uint8_t b[40000];size_t n=synthetic(b,sizeof b,4u);CHECK(n>0);
 Gk144ArchivedMapper r={0};CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_OK);
 CHECK(r.lower_prg_bank==4u&&r.upper_prg_bank==15u&&r.confirmed_prg_pages==128u);
 CHECK(r.saved_program_counter==0xefc9u&&r.mapper_prg_reg==4u&&r.mapping_observed_at_saved_instant_only);
 Gk144ArchivedMapper original=r;
 CHECK(gk144_probe_archived_prg_map(b,n-1u,&r)==GK144_MAPPER_STATE_REFUSED);
 CHECK(memcmp(&r,&original,sizeof r)==0);
 CHECK(gk144_probe_archived_prg_map(NULL,n,&r)==GK144_MAPPER_BAD_INPUT);
 CHECK(memcmp(&r,&original,sizeof r)==0);
 CHECK(gk144_probe_archived_prg_map(b,n,NULL)==GK144_MAPPER_BAD_INPUT);
 for(unsigned bank=0;bank<15u;bank++){
  n=synthetic(b,sizeof b,bank);CHECK(n>0);
  CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_OK);
  CHECK(r.lower_prg_bank==bank&&r.upper_prg_bank==15u);
 }
 n=synthetic(b,sizeof b,4u);CHECK(n>0);
 /* Target exactly one named mapper field; corrupt its low-page mapping. */
 const char *needle="mapper.prgMemoryOffset";
 uint8_t *found=NULL;
 for(size_t i=0;i+strlen(needle)+5u<n;i++)if(memcmp(b+i,needle,strlen(needle))==0){found=b+i+strlen(needle)+5u;break;}
 CHECK(found);
 Gk144ArchivedMapper before=r;
 found[0x91u*4u]^=1u;
 CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_CONTRADICTION);
 CHECK(memcmp(&r,&before,sizeof r)==0);
 n=synthetic(b,sizeof b,4u);CHECK(n>0);
 uint8_t *regp=field_value(b,n,"mapper.prgReg");CHECK(regp);
 *regp=7u;CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_CONTRADICTION);
 n=synthetic(b,sizeof b,4u);CHECK(n>0);
 uint8_t *mode=field_value(b,n,"mapper.prgMode");CHECK(mode);
 *mode=0u;CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_UNSUPPORTED_MODE);
 n=synthetic(b,sizeof b,4u);CHECK(n>0);
 uint8_t *slot=field_value(b,n,"mapper.slotSelect");CHECK(slot);
 *slot=0u;CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_UNSUPPORTED_MODE);
 n=synthetic(b,sizeof b,4u);CHECK(n>0);
 uint8_t *t=field_value(b,n,"mapper.prgMemoryType");CHECK(t);
 t[0x80u*4u]=1u;CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_CONTRADICTION);
 n=synthetic(b,sizeof b,4u);CHECK(n>0);
 uint8_t *acc=field_value(b,n,"mapper.prgMemoryAccess");CHECK(acc);
 acc[0xc0u*4u]=2u;CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_CONTRADICTION);
 n=synthetic(b,sizeof b,4u);CHECK(n>0);
 uint8_t repeated=1u;
 CHECK(append(b,sizeof b,&n,"mapper.prgMode",&repeated,1u));
 CHECK(gk144_probe_archived_prg_map(b,n,&r)==GK144_MAPPER_STATE_REFUSED);
 CHECK(memcmp(&r,&before,sizeof r)==0);
 n=synthetic(b,sizeof b,3u);CHECK(n>0);
 Gk1App *app=calloc(1,sizeof *app),*saved=malloc(sizeof *app);
 CHECK(app&&saved);gk1_app_init(app);memcpy(saved,app,sizeof *app);
 CHECK(gk1_app_source_archived_mapper(app,b,n,&r)==GK144_MAPPER_BAD_INPUT);
 CHECK(memcmp(saved,app,sizeof *app)==0);
 app->setup_stage[app->active_world]=GK53_SETUP_AWAIT_SOURCE;
 app->setup_difficulty[app->active_world]=2;
 memcpy(saved,app,sizeof *app);
 CHECK(gk1_app_source_archived_mapper(app,b,n,&r)==GK144_MAPPER_OK);
 CHECK(r.lower_prg_bank==3u&&memcmp(saved,app,sizeof *app)==0);
 free(app);free(saved);
 if(argc==4&&strcmp(argv[1],"--private-archived")==0){
  CHECK(file_check(argv[2],0u));CHECK(file_check(argv[3],0u));
 }
 printf("GK144 PASS %u assertions: observed archived mapper bank is not startup bank\n",checks);
 return 0;
}
