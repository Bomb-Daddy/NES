#include "gk216_fresh_cpu_pc.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Paths identify the exact already authenticated original Mesen captures.
 * ROM bytes and licensed capture bytes MUST remain private. */
static const char *const files[GK211_STAGES]={
 "fresh_01_after_start.json", "fresh_02_after_a.json",
 "fresh_03_after_a.json", "fresh_04_after_a.json",
 "fresh_05_after_a.json", "fresh_06_after_b.json",
 "fresh_07_after_a.json", "fresh_08_after_a.json",
 "fresh_09_after_a.json", "fresh_10_after_wait.json",
 "fresh_11_after_wait.json", "fresh_12_after_wait.json",
 "fresh_13_after_a.json", "fresh_14_after_wait.json"
};
/* Data is parsed only after its entire file has passed the SHA gates from
 * GK211. The local parser still checks bounds and decimal terminators. */
static const char *cpu_scope(const char *text){
 const char *p=strstr(text,"\"cpu\"");
 if(!p)return NULL;
 p=strchr(p,':');if(!p)return NULL;
 for(++p;isspace((unsigned char)*p);p++);
 return *p=='{'?p:NULL;
}
static int read_field(const char *cpu,const char *key,uint64_t *value){
 const char *end=strchr(cpu,'}'),*p=strstr(cpu,key);
 if(!end||!p||p>=end)return 0;
 p+=strlen(key);
 while(p<end&&isspace((unsigned char)*p))p++;
 if(p>=end||*p++!=':')return 0;
 while(p<end&&isspace((unsigned char)*p))p++;
 if(p>=end||!isdigit((unsigned char)*p))return 0;
 uint64_t v=0;
 while(p<end&&isdigit((unsigned char)*p)){
  unsigned d=(unsigned)(*p++-'0');
  if(v>(UINT64_MAX-d)/10u)return 0;
  v=10u*v+d;
 }
 if(p<end&&*p!=','&&*p!='}'&&!isspace((unsigned char)*p))return 0;
 *value=v;return 1;
}
static int read_cpu(const char *path,Gk216CpuStage *stage,
 const Gk211Stage *source){
 FILE *f=fopen(path,"rb");if(!f)return 0;
 char text[16385];size_t n=fread(text,1,16385,f);
 int failed=ferror(f);if(fclose(f)!=0)failed=1;
 if(failed||!n||n>16384||memchr(text,'\0',n))return 0;
 text[n]='\0';
 const char *cpu=cpu_scope(text);
 if(!cpu)return 0;
 const char *keys[]={"\"pc\"","\"a\"","\"x\"","\"y\"",
  "\"sp\"","\"ps\"","\"cycleCount\""};
 uint64_t v[7]={0};
 for(unsigned k=0;k<7;k++)if(!read_field(cpu,keys[k],v+k))return 0;
 if(v[0]<0x8000u||v[0]>0xffffu||
    v[1]>255u||v[2]>255u||v[3]>255u||
    v[4]>255u||v[5]>255u||v[6]!=source->cpu_cycles)return 0;
 stage->pc=(uint16_t)v[0];stage->a=(uint8_t)v[1];
 stage->x=(uint8_t)v[2];stage->y=(uint8_t)v[3];
 stage->sp=(uint8_t)v[4];stage->ps=(uint8_t)v[5];
 stage->cycles=v[6];stage->frame=source->frame;
 stage->change_mask=source->changed_mask;
 stage->upper_program_window=(uint8_t)(stage->pc>=0xc000u);
 return 1;
}
Gk216Status gk216_verify_fresh_cpu_pc(const char *stock_rom,
 const char *private_dir,Gk216CpuReport *out){
 if(!stock_rom||!*stock_rom||!private_dir||!*private_dir||!out)
  return GK216_BAD_ARGUMENT;
 Gk82RomBootstrap rom={0};
 if(gk82_probe_stock_rom(stock_rom,&rom)!=GK82_ROM_OK)
  return GK216_SOURCE_REFUSED;
 Gk211Report captures={0};
 if(gk211_verify_fresh_reset_sequence(private_dir,&captures)!=GK211_OK)
  return GK216_EVIDENCE_REFUSED;
 if(captures.validated_stages!=GK211_STAGES||
    captures.source_observation_only!=1u||
    captures.independent_initialization_implemented!=0u)
  return GK216_EVIDENCE_REFUSED;
 Gk216CpuReport candidate={0};
 for(unsigned i=0;i<GK211_STAGES;i++){
  char path[4096];
  int n=snprintf(path,sizeof path,"%s/%s",private_dir,files[i]);
  if(n<0||(size_t)n>=sizeof path)return GK216_BAD_ARGUMENT;
  if(!read_cpu(path,&candidate.stages[i],&captures.stage[i]))
   return GK216_CPU_REFUSED;
  if(i&&candidate.stages[i].cycles<=candidate.stages[i-1].cycles)
   return GK216_CHRONOLOGY_REFUSED;
  candidate.count=(uint8_t)(i+1u);
 }
 candidate.fresh_workram_only=1u;
 candidate.sampled_at_capture_end=1u;
 candidate.mapper_bank_not_derived=1u;
 candidate.actual_memory_writes_not_traced=1u;
 candidate.first_spring_not_generated=1u;
 *out=candidate;
 return GK216_OK;
}
