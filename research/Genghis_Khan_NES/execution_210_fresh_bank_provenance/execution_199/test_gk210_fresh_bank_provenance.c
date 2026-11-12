#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk165_fresh_json.h"
#include "gk1_source_identity.h"
#include "gk77_stock_spring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#define CHECK(x) do {if(!(x)){fprintf(stderr,"GK210 check %u failed: %s\n",__LINE__,#x);return 1;}++checks;}while(0)
static unsigned checks;
/* Only tests source-file membership and metadata. Never embeds the original
 * 83-byte licensed country records in the source or public QA artifacts. */
static char *read_text(const char *path,size_t *n){
 FILE *f=fopen(path,"rb");if(!f)return NULL;
 if(fseek(f,0,SEEK_END)!=0){fclose(f);return NULL;}
 long l=ftell(f);if(l<1||l>100000){fclose(f);return NULL;}
 if(fseek(f,0,SEEK_SET)!=0){fclose(f);return NULL;}
 char *b=malloc((size_t)l+1u);
 if(!b){fclose(f);return NULL;}
 if(fread(b,1,(size_t)l,f)!=(size_t)l||fclose(f)!=0){free(b);return NULL;}
 b[l]=0;*n=(size_t)l;return b;
}
/* A replacement MUST be confined to an exact named object. For the original
 * root aliases, use the unique tail after the flat CPU register object. */
static int patch_digit(char *b,const char *scope,const char *key,char old,char newer){
 char *p=strstr(b,scope);if(!p)return 0;
 p=strstr(p,key);if(!p)return 0;
 p=strchr(p,':');if(!p)return 0;
 p++;while(*p==' '||*p=='\t')p++;
 if(*p!=old)return 0;
 *p=newer;return 1;
}
static int probe(const char *source,const char *scope,const char *key,
 char old,char newer,Gk165Status expected){
 size_t n;char *b=read_text(source,&n);if(!b)return 0;
 if(!patch_digit(b,scope,key,old,newer)){free(b);return 0;}
 #ifdef _WIN32
 char tmp[L_tmpnam];
#endif
#ifndef _WIN32
 char name[]="/tmp/gk210_fresh_bank_XXXXXX";
 int fd=mkstemp(name);if(fd<0){free(b);return 0;}
 FILE *f=fdopen(fd,"wb");if(!f){close(fd);remove(name);free(b);return 0;}
#else
 if(tmpnam_s(tmp,sizeof tmp)!=0){free(b);return 0;}
 FILE *f=fopen(tmp,"wbx");if(!f){free(b);return 0;}
#endif
 int ok=fwrite(b,1,n,f)==n&&fclose(f)==0;
 free(b);
 if(!ok){
#ifndef _WIN32
  remove(name);
#else
  remove(tmp);
#endif
  return 0;
 }
 uint8_t out[GK165_FRESH_SIZE],baseline[GK165_FRESH_SIZE];
 memset(out,0xa6,sizeof out);memcpy(baseline,out,sizeof out);
#ifndef _WIN32
 Gk165Status r=gk165_extract_fresh_json(name,out);remove(name);
#else
 Gk165Status r=gk165_extract_fresh_json(tmp,out);remove(tmp);
#endif
 return r==expected && (r==GK165_OK || memcmp(out,baseline,sizeof out)==0);
}
int main(int argc,char **argv){
 uint8_t out[GK165_FRESH_SIZE],before[GK165_FRESH_SIZE];
 memset(out,0xa6,sizeof out);memcpy(before,out,sizeof out);
 CHECK(gk165_extract_fresh_json(NULL,out)==GK165_ARGUMENT);
 CHECK(gk165_extract_fresh_json("/GK210/missing",out)==GK165_IO);
 CHECK(memcmp(out,before,sizeof out)==0);
 if(argc==2 || argc==3){
  const char *p=argv[1];
  CHECK(gk165_extract_fresh_json(p,out)==GK165_OK);
  uint8_t sha[32];gk1_sha256_bytes(out,sizeof out,sha);
  CHECK(gk77_known_source_digest(GK77_FRESH_MONGOL_WORKRAM,sha));
  Gk77Country first;CHECK(gk77_decode(out,83u,&first)==GK77_OK);
  CHECK(first.gold==430u&&first.food==208u&&first.troops==61u);
  /* Same valid signed country array, but inconsistent CPU mapping: rejected. */
  CHECK(probe(p,"\"nesMemory\"","\"gold\"",'4','5',GK165_PROFILE));
  CHECK(probe(p,"\"nesMemory\"","\"rate\"",'1','2',GK165_PROFILE));
  /* SaveRAM is empty in this original fresh capture, never a stock save. */
  CHECK(probe(p,"\"nesSaveRam\"","\"gold\"",'0','1',GK165_PROFILE));
  CHECK(probe(p,"\"nesSaveRam\"","\"food\"",'0','1',GK165_PROFILE));
  CHECK(probe(p,"\"nesSaveRam\"","\"orders\"",'0','1',GK165_PROFILE));
  CHECK(probe(p,"\"nesSaveRam\"","\"country_count\"",'0','1',GK165_PROFILE));
  /* Legacy probe's root aliases are all zero because it read the wrong
   * bank. A mixed root display is corruption, NOT an empty original spring. */
  CHECK(probe(p,"\"cpu\"","\"gold\"",'0','1',GK165_PROFILE));
  CHECK(probe(p,"\"cpu\"","\"year\"",'0','1',GK165_PROFILE));
  CHECK(probe(p,"\"cpu\"","\"countries\"",'0','1',GK165_PROFILE));
  /* The authentic WorkRAM receipt is pinned and is not universal. */
  CHECK(probe(p,"\"nesWorkRam\"","\"rate\"",'1','2',GK165_PROFILE));
  if(argc==3){
   memset(out,0xa6,sizeof out);
   CHECK(gk165_extract_fresh_json(argv[2],out)==GK165_OK);
   gk1_sha256_bytes(out,sizeof out,sha);
   CHECK(gk77_known_source_digest(GK77_FRESH_MONGOL_WORKRAM,sha));
  }
  puts("GK210 authentic fresh WorkRAM/CPU mapping and blank SaveRAM negative-bank controls PASS");
 }else CHECK(argc==1);
 printf("GK210 %u assertions PASS; private original fresh source %s\n",checks,argc>=2?"TESTED":"NOT SUPPLIED");
 return 0;
}
