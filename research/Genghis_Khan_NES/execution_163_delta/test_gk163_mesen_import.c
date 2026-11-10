#include "gk163_mesen_import.h"
#include "gk1_source_identity.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef GK163_HAS_ZLIB
#include <zlib.h>
#endif
#define CHECK(c) do{if(!(c)){fprintf(stderr,"GK163 FAIL line %d: %s\n",__LINE__,#c);return 2;}checks++;}while(0)
static unsigned checks;
#ifdef GK163_HAS_ZLIB
static void put32(uint8_t *p,uint32_t v){
 for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8u*i));
}
/* A copyright-clean synthetic .mss, deliberately NOT an original record.
 * Records, CPU bytes, mapper contents and compressed state here are synthetic.
 * Every apparently valid synthetic container MUST be SHA-rejected. */
static int synthetic(const char *path,int bank,int corrupt){
 uint8_t *plain=calloc(1,25000),*packed=calloc(1,25000);if(!plain||!packed)return 0;
 size_t n=0;
 const struct {const char *key;uint32_t len;} f[]={
  {"cpu.pc",2},{"cpu.cycleCount",8},{"memoryManager.internalRam",2048},
  {"mapper.workRam",8192},{"mapper.saveRam",8192}
 };
 for(unsigned i=0;i<sizeof(f)/sizeof(f[0]);i++){
  size_t l=strlen(f[i].key);
  if(n+l+5u+f[i].len>25000u){free(plain);free(packed);return 0;}
  memcpy(plain+n,f[i].key,l);n+=l;plain[n++]=0;
  put32(plain+n,f[i].len);n+=4;
  if(i==(bank==1?3u:4u))plain[n+0x0dcd]=1u;
  n+=f[i].len;
 }
 uLongf compressed=25000u;
 if(compress2(packed,&compressed,plain,(uLong)n,Z_BEST_COMPRESSION)!=Z_OK){free(plain);free(packed);return 0;}
 static const char filename[]="Genghis Khan (USA).nes";
 FILE *fp=fopen(path,"wb");if(!fp){free(plain);free(packed);return 0;}
 uint8_t header[35]={0};memcpy(header,"MSS\1",4);put32(header+31,0u);
 uint8_t meta[9]={1u};put32(meta+1,(uint32_t)n);put32(meta+5,(uint32_t)compressed);
 uint8_t namelen[4];put32(namelen,sizeof(filename)-1u);
 int ok=fwrite(header,1,35,fp)==35&&fwrite(namelen,1,4,fp)==4&&
        fwrite(filename,1,sizeof filename-1,fp)==sizeof filename-1&&
        fwrite(meta,1,9,fp)==9&&fwrite(packed,1,compressed,fp)==compressed;
 if(fclose(fp)!=0)ok=0;
 if(corrupt){fp=fopen(path,"r+b");if(!fp)ok=0;
  else{if(fseek(fp,35u+4u+sizeof filename-1u+9u+3u,SEEK_SET))ok=0;
   else{uint8_t u=0xfa;if(fwrite(&u,1,1,fp)!=1)ok=0;}if(fclose(fp))ok=0;}
 }
 free(plain);free(packed);return ok;
}
#endif
int main(int argc,char **argv){
#ifndef GK163_HAS_ZLIB
 (void)argv;
#endif
 CHECK(argc==1||argc==5);
 uint8_t out[GK163_MAX_RECORDS],old[GK163_MAX_RECORDS];
 memset(out,0xa5,sizeof out);memcpy(old,out,sizeof out);size_t got=77u;
 CHECK(gk163_mesen_extract(NULL,GK77_ARCHIVED_MONGOL_SAVERAM,out,&got)==GK163_ARGUMENT);
 CHECK(gk163_mesen_extract("/invalid",GK77_FRESH_MONGOL_WORKRAM,out,&got)==GK163_ARGUMENT);
 CHECK(gk163_mesen_extract("/invalid",GK77_ARCHIVED_WORLD_SAVERAM,NULL,&got)==GK163_ARGUMENT);
 CHECK(gk163_mesen_extract("/invalid",GK77_ARCHIVED_WORLD_SAVERAM,out,NULL)==GK163_ARGUMENT);
 CHECK(got==77u&&!memcmp(out,old,sizeof out));
#ifdef GK163_HAS_ZLIB
 CHECK(gk163_mesen_extract("/invalid",GK77_ARCHIVED_WORLD_SAVERAM,out,&got)==GK163_IO);
 const char *tmp="GK163_fake_mesen_source.mss";
 CHECK(synthetic(tmp,2,0));
 CHECK(gk163_mesen_extract(tmp,GK77_ARCHIVED_MONGOL_SAVERAM,out,&got)==GK163_SHA);
 CHECK(got==77u&&!memcmp(out,old,sizeof out));
 CHECK(gk163_mesen_extract(tmp,GK77_ARCHIVED_WORLD_SAVERAM,out,&got)==GK163_SHA);
 CHECK(synthetic(tmp,1,0));
 CHECK(gk163_mesen_extract(tmp,GK77_ARCHIVED_MONGOL_SAVERAM,out,&got)==GK163_BANK);
 CHECK(synthetic(tmp,2,1));
 CHECK(gk163_mesen_extract(tmp,GK77_ARCHIVED_MONGOL_SAVERAM,out,&got)==GK163_FORMAT);
 CHECK(remove(tmp)==0);
 if(argc==5){
  for(int i=0;i<2;i++){
   Gk77Profile p=i==0?GK77_ARCHIVED_MONGOL_SAVERAM:GK77_ARCHIVED_WORLD_SAVERAM;
   const char *original=argv[1+i];const char *expected=argv[3+i];
   size_t length=i==0?14u*83u:27u*83u;
   CHECK(gk163_mesen_extract(original,p,out,&got)==GK163_OK);
   CHECK(got==length);
   FILE *f=fopen(expected,"rb");CHECK(f!=NULL);
   uint8_t raw[GK163_MAX_RECORDS]={0};
   CHECK(fread(raw,1,length,f)==length);CHECK(fgetc(f)==EOF);CHECK(!fclose(f));
   CHECK(!memcmp(raw,out,length));
   uint8_t digest[32];gk1_sha256_bytes(out,got,digest);
   CHECK(gk77_known_source_digest(p,digest));
   printf("OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED profile=%s 83-byte countries=%zu: AUTHENTIC EXACT STOCK HASH PASS\n",
     gk77_profile_name(p),got/83u);
  }
 }
#else
 CHECK(gk163_mesen_extract("/invalid",GK77_ARCHIVED_WORLD_SAVERAM,out,&got)==GK163_UNAVAILABLE);
#endif
 CHECK(got==77u||argc==5);
 printf("GK163 %u checks PASS. No first-Spring calculation or new Mesen gameplay.\n",checks);
 return 0;
}
