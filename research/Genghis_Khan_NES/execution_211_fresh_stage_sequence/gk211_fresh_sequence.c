#include "gk211_fresh_sequence.h"
#include "gk1_source_identity.h"
#include "gk165_fresh_json.h"
#include "gk77_stock_spring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#define GK211_JSON_MAX 16384u
#define MASK_ALL 127u
/* Complete SHA-256 identities only, NOT ROM/RAM/audio/art or copied game text.
 * Every snapshot is one actual archived fresh ORIGINAL-USA Mesen observation;
 * do not extrapolate these values to other RNG/difficulty histories. */
static const struct {const char *name,*sha;} samples[GK211_STAGES]={
 {"fresh_01_after_start.json","217df40eaf6698cef2b395cbacabbf079cc9eb122c3ab2412489e88334188d15"},
 {"fresh_02_after_a.json","636ca8892b1c0d0d24ab9bcf2161f206127d0e38dfdeb7c47c403b6326657319"},
 {"fresh_03_after_a.json","d37e4f9ddb1494e62831bcc3557de14bf2255c73fa77cab417a7344a06bc3503"},
 {"fresh_04_after_a.json","42a2b1668572bcfffa1279180c438340fbc153133fa249adc295a860f0786452"},
 {"fresh_05_after_a.json","b269e1fa37a1ea163f571b1a1afe6111543b19bcfb1c430ca2b749dc42a99154"},
 {"fresh_06_after_b.json","01db2c106feeb8eb417fae05e02ca3450fd25e05b849882bf8f1bbc8e64dc34d"},
 {"fresh_07_after_a.json","3923330e9f763b38af0eca683d62866658a16675eea04297c62680278e9663fc"},
 {"fresh_08_after_a.json","3d7749ca42cbe9e510968be27db797111113f74ea3e5bf29a39f9eec0ea13b2d"},
 {"fresh_09_after_a.json","faf5917010c182c4736bff031e9e869d1b320d9969a6a3329235ca38026b0ca7"},
 {"fresh_10_after_wait.json","5c11be560f945c838c4e8e36eaa33c0c449e2a1ce1a60556d958a7dc186092f3"},
 {"fresh_11_after_wait.json","6bab5b6499dbbfacc93b371501925f472b75c20facf7d883574029941f1bc6d9"},
 {"fresh_12_after_wait.json","b7abfff044307c6eb4ffb55f9c507f543c758e2546b0ee69dd228ef42f5d8d7d"},
 {"fresh_13_after_a.json","a5ff637291776a8a6abc916531ee5de773fad8d33e895476ddf56123ae10a143"},
 {"fresh_14_after_wait.json","70bddae0b818474ef646b9a08fda0f2266055b6e01d1c5bb30f46e545d4ee7f0"}
};
static int hexval(char c){
 if(c>='0'&&c<='9')return c-'0';
 if(c>='a'&&c<='f')return c-'a'+10;
 return -1;
}
static int hash_equals(const uint8_t *bytes,size_t n,const char *hex){
 uint8_t hash[32];gk1_sha256_bytes(bytes,n,hash);
 if(strlen(hex)!=64u)return 0;
 for(unsigned i=0;i<32u;i++){
  int hi=hexval(hex[2u*i]),lo=hexval(hex[2u*i+1u]);
  if(hi<0||lo<0||hash[i]!=(uint8_t)((hi<<4)|lo))return 0;
 }
 return 1;
}
/* This deliberately small parser is ONLY allowed to interpret byte-for-byte
 * authenticated fixed receipts. Never use it on arbitrary untrusted JSON. */
static const char *named_block(const char *all,const char *name){
 const char *p=strstr(all,name);if(!p)return NULL;
 p=strchr(p,':');if(!p)return NULL;
 p++;while(isspace((unsigned char)*p))p++;
 return *p=='{'?p:NULL;
}
static int read_u64(const char *scope,const char *key,uint64_t *out){
 const char *end=strchr(scope,'}'),*at=strstr(scope,key);
 if(!end||!at||at>=end)return 0;
 at+=strlen(key);while(at<end&&isspace((unsigned char)*at))at++;
 if(at>=end||*at++!=':')return 0;
 while(at<end&&isspace((unsigned char)*at))at++;
 if(at>=end||!isdigit((unsigned char)*at))return 0;
 uint64_t n=0;
 while(at<end&&isdigit((unsigned char)*at)){
  unsigned d=(unsigned)(*at++-'0');
  if(n>(UINT64_MAX-d)/10u)return 0;
  n=n*10u+d;
 }
 if(at<end&&*at!=','&&*at!='}'&&!isspace((unsigned char)*at))return 0;
 *out=n;return 1;
}
static int one_view(const char *j,const char *name,uint64_t v[7]){
 static const char *keys[]={"\"year\"","\"gold\"","\"food\"","\"troops\"",
   "\"rate\"","\"orders\"","\"country_count\""};
 const char *at=named_block(j,name);if(!at)return 0;
 for(unsigned i=0;i<7u;i++)if(!read_u64(at,keys[i],v+i))return 0;
 return 1;
}
static int read_stage(const char *buf,Gk211Stage *row){
 uint64_t work[7],memory[7],save[7],frame=0,cycles=0;
 if(!one_view(buf,"\"nesWorkRam\"",work)||
    !one_view(buf,"\"nesMemory\"",memory)||
    !one_view(buf,"\"nesSaveRam\"",save))return 0;
 for(unsigned i=0;i<7u;i++){
  if(work[i]!=memory[i]||save[i]!=0u||work[i]>65535u)return 0;
 }
 /* Frame is a unique top-level capture field. Exact-file SHA already gated. */
 const char *v=strstr(buf,"\"frame\"");if(!v)return 0;
 v=strchr(v,':');if(!v)return 0;
 unsigned long long parsed_frame=0;
 if(sscanf(v+1," %llu",&parsed_frame)!=1)return 0;
 frame=(uint64_t)parsed_frame;
 const char *cpu=named_block(buf,"\"cpu\"");
 if(!cpu||!read_u64(cpu,"\"cycleCount\"",&cycles)||
    !frame||frame>UINT32_MAX||!cycles)return 0;
 row->frame=(uint32_t)frame;row->cpu_cycles=cycles;
 row->year=(uint16_t)work[0];row->gold=(uint16_t)work[1];
 row->food=(uint16_t)work[2];row->troops=(uint16_t)work[3];
 row->rate=(uint16_t)work[4];row->orders=(uint8_t)work[5];
 row->countries=(uint8_t)work[6];
 return work[5]<=3u&&work[6]<=27u;
}
static uint8_t changes(const Gk211Stage *a,const Gk211Stage *b){
 return (uint8_t)((a->year!=b->year?1u:0u)|
  (a->gold!=b->gold?2u:0u)|(a->food!=b->food?4u:0u)|
  (a->troops!=b->troops?8u:0u)|(a->rate!=b->rate?16u:0u)|
  (a->orders!=b->orders?32u:0u)|(a->countries!=b->countries?64u:0u));
}
Gk211Status gk211_verify_fresh_reset_sequence(const char *private_dir,
 Gk211Report *out){
 if(!private_dir||!*private_dir||!out)return GK211_ARGUMENT;
 Gk211Report result={0};
 for(unsigned i=0;i<GK211_STAGES;i++){
  char path[4096];
  int w=snprintf(path,sizeof path,"%s/%s",private_dir,samples[i].name);
  if(w<0||(size_t)w>=sizeof path)return GK211_ARGUMENT;
  FILE *f=fopen(path,"rb");if(!f)return GK211_EVIDENCE_REFUSED;
  char data[GK211_JSON_MAX+1];
  size_t n=fread(data,1,sizeof data,f);
  int fail=ferror(f),closefail=fclose(f);
  if(fail||closefail||n==0||n>GK211_JSON_MAX||memchr(data,'\0',n))
   return GK211_EVIDENCE_REFUSED;
  data[n]='\0';
  if(!hash_equals((const uint8_t *)data,n,samples[i].sha))
   return GK211_EVIDENCE_REFUSED;
  if(!read_stage(data,result.stage+i))return GK211_BANK_REFUSED;
  if(i){
   if(result.stage[i].frame<=result.stage[i-1].frame||
      result.stage[i].cpu_cycles<=result.stage[i-1].cpu_cycles)
    return GK211_CHRONOLOGY_REFUSED;
   result.stage[i].changed_mask=changes(result.stage+i-1,result.stage+i);
  }else result.stage[i].changed_mask=MASK_ALL;
  result.validated_stages=(uint8_t)(i+1);
 }
 /* Verify the actual archived sequence's event ordering; masks and stats
  * describe ONE stock game history, never formulas for random new games. */
 static const uint8_t masks[GK211_STAGES]={
  127u,0x4fu,0u,0u,0u,0u,0u,0u,4u,17u,0u,14u,0u,32u
 };
 for(unsigned i=1;i<GK211_STAGES;i++)
  if(result.stage[i].changed_mask!=masks[i])return GK211_CHRONOLOGY_REFUSED;
 const Gk211Stage *a=result.stage;
 if(a[0].year||a[0].gold||a[0].food||a[0].troops||
    a[0].rate||a[0].orders||a[0].countries||
    a[1].year!=1174u||a[1].countries!=14u||a[1].orders!=0u||
    a[8].food!=(uint16_t)(2u*a[7].food)||
    a[9].year!=a[8].year+1u||a[9].rate==0u||
    a[11].year!=1175u||a[11].food>=a[10].food||
    a[11].gold<a[10].gold||a[11].troops<a[10].troops||
    a[13].orders!=3u||a[13].countries!=14u||
    a[13].gold!=430u||a[13].food!=208u||
    a[13].troops!=61u||a[13].rate!=121u)
  return GK211_CHRONOLOGY_REFUSED;
 /* Last JSON must ALSO pass the actual native game admission/83-byte
  * source digest check, not just its full-document identity. */
 char final_path[4096];
 int w=snprintf(final_path,sizeof final_path,"%s/%s",private_dir,samples[13].name);
 if(w<0||(size_t)w>=sizeof final_path)return GK211_ARGUMENT;
 uint8_t raw[GK165_FRESH_SIZE];
 if(gk165_extract_fresh_json(final_path,raw)!=GK165_OK)
  return GK211_EVIDENCE_REFUSED;
 uint8_t sha[32];gk1_sha256_bytes(raw,sizeof raw,sha);
 if(!gk77_known_source_digest(GK77_FRESH_MONGOL_WORKRAM,sha))
  return GK211_EVIDENCE_REFUSED;
 result.observed_setup_step=2u;
 result.observed_food_doubling_step=9u;
 result.observed_year_rate_step=10u;
 result.observed_spring_economy_step=12u;
 result.observed_order_grant_step=14u;
 result.source_observation_only=1u;
 result.independent_initialization_implemented=0u;
 *out=result;
 return GK211_OK;
}
