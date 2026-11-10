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

#define REQUIRE(x) do{if(!(x)){fprintf(stderr,"GK165 assertion at %u: %s\n",(unsigned)__LINE__,#x);return 1;}}while(0)
int main(int argc,char **argv){
 uint8_t out[GK165_FRESH_SIZE],unchanged[GK165_FRESH_SIZE];
 memset(out,0xa5,sizeof out);memcpy(unchanged,out,sizeof out);
 REQUIRE(gk165_extract_fresh_json(NULL,out)==GK165_ARGUMENT);
 REQUIRE(gk165_extract_fresh_json("/NONEXISTENT/GK165",out)==GK165_IO);
 REQUIRE(memcmp(out,unchanged,sizeof out)==0);
 if(argc==2){
  REQUIRE(gk165_extract_fresh_json(argv[1],out)==GK165_OK);
  uint8_t sha[32];gk1_sha256_bytes(out,sizeof out,sha);
  REQUIRE(gk77_known_source_digest(GK77_FRESH_MONGOL_WORKRAM,sha));
  Gk77Country one;REQUIRE(gk77_decode(out,83u,&one)==GK77_OK);
  REQUIRE(one.gold==430u&&one.food==208u&&one.troops==61u);
  /* Mutate a matching owner file and verify SHA refuses before output writes. */
  FILE *fp=fopen(argv[1],"rb");REQUIRE(fp!=NULL);
  REQUIRE(fseek(fp,0,SEEK_END)==0);long l=ftell(fp);REQUIRE(l>2300&&l<100000);
  REQUIRE(fseek(fp,0,SEEK_SET)==0);char *buf=malloc((size_t)l+1u);
  REQUIRE(buf!=NULL);REQUIRE(fread(buf,1,(size_t)l,fp)==(size_t)l);
  REQUIRE(fclose(fp)==0);buf[l]='\0';
  char *key=strstr(buf,"\"original_country_records_workram\"");REQUIRE(key!=NULL);
  char *hex=strchr(key,':');REQUIRE(hex!=NULL);
  hex=strchr(hex,'\"');REQUIRE(hex!=NULL);
  hex[1]=(hex[1]=='0'?'1':'0');
#ifndef _WIN32
  char tmp[]="/tmp/gk165_bad_json_test_XXXXXX";
  int tmpfd=mkstemp(tmp);REQUIRE(tmpfd>=0);
  fp=fdopen(tmpfd,"wb");REQUIRE(fp!=NULL);
#else
  /* Windows synthetic tests run without this optional private-owner probe. */
  char tmp[L_tmpnam];REQUIRE(tmpnam_s(tmp,sizeof tmp)==0);
  fp=fopen(tmp,"wbx");REQUIRE(fp!=NULL);
#endif
  REQUIRE(fwrite(buf,1,(size_t)l,fp)==(size_t)l);REQUIRE(fclose(fp)==0);
  free(buf);memset(out,0x5a,sizeof out);memset(unchanged,0x5a,sizeof unchanged);
  REQUIRE(gk165_extract_fresh_json(tmp,out)==GK165_SHA);
  REQUIRE(memcmp(out,unchanged,sizeof out)==0);
  REQUIRE(remove(tmp)==0);
  puts("GK165 VERIFIED_STATIC + AUTHENTIC PRIVATE FRESH_MESEN_WORKRAM receipt SHA and negative rollback PASS");
 }else puts("GK165 public synthetic failure gates PASS; authenticated fresh owner JSON not supplied");
 return 0;
}
