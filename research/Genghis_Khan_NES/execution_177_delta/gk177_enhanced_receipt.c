#define _GNU_SOURCE
#include "gk177_enhanced_receipt.h"
#include "gk1_source_identity.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#define BASE 48u
#define FIRST_SEASONS 5u
#define RECORD 14u
#define CONT (BASE + FIRST_SEASONS*RECORD)
#define HASH 416u
static uint16_t amount(const uint8_t *s){return (uint16_t)((unsigned)s[2]|((unsigned)s[3]<<8));}
static void put(uint8_t *p,uint8_t n,uint8_t passed,const Gk116LaborAction *a){
 p[0]=n;p[1]=passed;
 for(unsigned i=0;i<n;i++){
  p[2u+i*4u]=a[i].from;p[3u+i*4u]=a[i].to;
  p[4u+i*4u]=(uint8_t)a[i].quantity;
  p[5u+i*4u]=(uint8_t)(a[i].quantity>>8);
 }
}
static int canonical_stages(const Gk79Checkpoint *o,const Gk177Stages *s){
 return o&&s&&gk176_attestation_intact(&s->continuation,
   &s->year2,&s->spring,&s->summer,&s->fall,&s->winter,o);
}
Gk177Status gk177_encode(const Gk79Checkpoint *o,const Gk177Stages *s,
 uint8_t raw[GK177_BYTES]){
 if(!raw||!o||!s)return GK177_INPUT;
 if(!canonical_stages(o,s))return GK177_SOURCE;
 memset(raw,0,GK177_BYTES);
 memcpy(raw,"GC77",4);raw[4]=1u;raw[5]=1u;
 raw[6]=(uint8_t)s->spring.mutable_world.scenario_id;
 raw[7]=s->spring.player_index;
 raw[8]=(uint8_t)s->spring.year;raw[9]=(uint8_t)(s->spring.year>>8);
 raw[10]=(uint8_t)s->spring.rate;raw[11]=(uint8_t)(s->spring.rate>>8);
 raw[12]=s->continuation.season_index;raw[13]=1u;
 memcpy(raw+16,s->spring.source_sha,32);
 put(raw+BASE,s->spring.action_count,s->spring.pass_ended,s->spring.actions);
 put(raw+BASE+RECORD,s->summer.action_count,s->summer.pass_ended,s->summer.actions);
 put(raw+BASE+2u*RECORD,s->fall.action_count,s->fall.pass_ended,s->fall.actions);
 put(raw+BASE+3u*RECORD,s->winter.action_count,s->winter.pass_ended,s->winter.actions);
 put(raw+BASE+4u*RECORD,s->year2.action_count,s->year2.pass_ended,s->year2.actions);
 for(unsigned i=0;i<=s->continuation.season_index;i++){
  const Gk176SeasonReceipt *r=&s->continuation.receipts[i];
  put(raw+CONT+i*RECORD,r->action_count,r->pass_ended,r->actions);
 }
 gk1_sha256_bytes(raw,HASH,raw+HASH);
 return GK177_OK;
}
static int valid_rec(const uint8_t *p){return p[0]<=3u&&p[1]<=1u;}
#define ACT(P) ((P)+2u)
#define LOOP_TRANSFER(R,OBJ,CALL) do{ \
 const uint8_t *r=(R);if(!valid_rec(r))return GK177_CORRUPT; \
 for(unsigned j=0;j<r[0];j++){const uint8_t *a=ACT(r)+4u*j; \
  if((CALL)!=GK177_OK)return GK177_CORRUPT;} \
}while(0)
/* Replay every preserved stage against the authenticated original source.
 * Do not deserialize mutable world state, budgets, RNG or licensed source. */
Gk177Status gk177_decode(const Gk79Checkpoint *o,uint8_t enhanced,
 const uint8_t *raw,size_t length,Gk177Stages *out){
 if(!o||!raw||!out)return GK177_INPUT;
 if(enhanced!=1u)return GK177_SOURCE;
 if(length!=GK177_BYTES||memcmp(raw,"GC77",4)||raw[4]!=1u||
    raw[5]!=1u||raw[12]>=GK176_MAX_SEASONS||raw[13]!=1u||
    raw[14]!=0u||raw[15]!=0u)return GK177_CORRUPT;
 uint8_t digest[32];gk1_sha256_bytes(raw,HASH,digest);
 if(memcmp(digest,raw+HASH,32))return GK177_CORRUPT;
 Gk177Stages *tmp=calloc(1,sizeof *tmp);if(!tmp)return GK177_IO;
 Gk177Status status=GK177_CORRUPT;
 if(gk116_open(o,1,&tmp->spring)!=GK116_OK){status=GK177_SOURCE;goto end;}
 const uint8_t *r=raw+BASE;
 if(!valid_rec(r))goto end;
 for(unsigned j=0;j<r[0];j++){
  const uint8_t *a=r+2u+4u*j;
  if(gk116_transfer(&tmp->spring,o,a[0],a[1],amount(a))!=GK116_OK)goto end;
 }
 if(r[1]&&gk116_pass_remaining(&tmp->spring,o)!=GK116_OK)goto end;
 if(gk158_open(&tmp->spring,o,&tmp->summer)!=GK158_OK)goto end;
 r=raw+BASE+RECORD;if(!valid_rec(r))goto end;
 for(unsigned j=0;j<r[0];j++){
  const uint8_t *a=r+2u+4u*j;
  if(gk158_transfer(&tmp->summer,&tmp->spring,o,a[0],a[1],amount(a))!=GK158_OK)goto end;
 }
 if(r[1]&&gk158_pass(&tmp->summer,&tmp->spring,o)!=GK158_OK)goto end;
 if(gk159_open(&tmp->spring,&tmp->summer,o,&tmp->fall)!=GK159_OK)goto end;
 r=raw+BASE+2u*RECORD;if(!valid_rec(r))goto end;
 for(unsigned j=0;j<r[0];j++){
  const uint8_t *a=r+2u+4u*j;
  if(gk159_transfer(&tmp->fall,&tmp->spring,&tmp->summer,o,a[0],a[1],amount(a))!=GK159_OK)goto end;
 }
 if(r[1]&&gk159_pass(&tmp->fall,&tmp->spring,&tmp->summer,o)!=GK159_OK)goto end;
 if(gk160_open(&tmp->spring,&tmp->summer,&tmp->fall,o,&tmp->winter)!=GK160_OK)goto end;
 r=raw+BASE+3u*RECORD;if(!valid_rec(r))goto end;
 for(unsigned j=0;j<r[0];j++){
  const uint8_t *a=r+2u+4u*j;
  if(gk160_transfer(&tmp->winter,&tmp->spring,&tmp->summer,&tmp->fall,o,a[0],a[1],amount(a))!=GK160_OK)goto end;
 }
 if(r[1]&&gk160_pass(&tmp->winter,&tmp->spring,&tmp->summer,&tmp->fall,o)!=GK160_OK)goto end;
 if(gk161_open(&tmp->spring,&tmp->summer,&tmp->fall,&tmp->winter,o,&tmp->year2)!=GK161_OK)goto end;
 r=raw+BASE+4u*RECORD;if(!valid_rec(r))goto end;
 for(unsigned j=0;j<r[0];j++){
  const uint8_t *a=r+2u+4u*j;
  if(gk161_transfer(&tmp->year2,&tmp->spring,&tmp->summer,&tmp->fall,&tmp->winter,
    o,a[0],a[1],amount(a))!=GK161_OK)goto end;
 }
 if(r[1]&&gk161_pass(&tmp->year2,&tmp->spring,&tmp->summer,&tmp->fall,
    &tmp->winter,o)!=GK161_OK)goto end;
 if(gk176_open(&tmp->year2,&tmp->spring,&tmp->summer,&tmp->fall,
    &tmp->winter,o,&tmp->continuation)!=GK176_OK)goto end;
 for(unsigned i=0;i<=raw[12];i++){
  r=raw+CONT+i*RECORD;if(!valid_rec(r))goto end;
  if(i&&gk176_advance(&tmp->continuation,&tmp->year2,&tmp->spring,
     &tmp->summer,&tmp->fall,&tmp->winter,o)!=GK176_OK)goto end;
  for(unsigned j=0;j<r[0];j++){
   const uint8_t *a=r+2u+4u*j;
   if(gk176_transfer(&tmp->continuation,&tmp->year2,&tmp->spring,
      &tmp->summer,&tmp->fall,&tmp->winter,o,a[0],a[1],amount(a))!=GK176_OK)goto end;
  }
  if(r[1]&&gk176_pass(&tmp->continuation,&tmp->year2,&tmp->spring,
      &tmp->summer,&tmp->fall,&tmp->winter,o)!=GK176_OK)goto end;
 }
 /* Compare the whole canonical encoding, not merely an unkeyed digest.
  * Reject unused actions, padding, forged event histories and mismatched source. */
 uint8_t canonical[GK177_BYTES];
 if(gk177_encode(o,tmp,canonical)!=GK177_OK||memcmp(raw,canonical,sizeof canonical))goto end;
 *out=*tmp;status=GK177_OK;
end:
 memset(tmp,0,sizeof *tmp);free(tmp);return status;
}
Gk177Status gk177_save_new(const char *path,const Gk79Checkpoint *o,
 const Gk177Stages *s){
 if(!path||!*path)return GK177_INPUT;
 uint8_t raw[GK177_BYTES];Gk177Status r=gk177_encode(o,s,raw);
 if(r!=GK177_OK)return r;
#ifdef _WIN32
 errno=0;FILE *fp=fopen(path,"wbx");if(!fp)return errno==EEXIST?GK177_EXISTS:GK177_IO;
 size_t n=fwrite(raw,1,sizeof raw,fp);int er=ferror(fp);int flush=fflush(fp);
 int sync=flush?1:_commit(_fileno(fp));int close_result=fclose(fp);
#else
 int fd=open(path,O_WRONLY|O_CREAT|O_EXCL,0600);
 if(fd<0)return errno==EEXIST?GK177_EXISTS:GK177_IO;
 size_t n=0;int er=0;
 while(n<sizeof raw){ssize_t k=write(fd,raw+n,sizeof raw-n);
   if(k<0&&errno==EINTR)continue;
   if(k<=0){er=1;break;} n+=(size_t)k;}
 int sync=er?1:fsync(fd);int close_result=close(fd);
 /* The directory entry itself must also survive a crash before success. */
 if(!er&&!sync&&!close_result){
  const char *slash=strrchr(path,'/');
  char dir[4096];size_t len=slash?(size_t)(slash-path):1u;
  if(len==0u)len=1u;
  if(len>=sizeof dir)sync=1;
  else {
   if(slash){memcpy(dir,path,len);dir[len]=0;}
   else {dir[0]='.';dir[1]=0;}
   int dfd=open(dir,O_RDONLY|O_DIRECTORY|O_NOFOLLOW);
   if(dfd<0)sync=1;
   else {if(fsync(dfd))sync=1;if(close(dfd))sync=1;}
  }
 }
#endif
 if(n!=sizeof raw||er||sync||close_result){(void)remove(path);return GK177_IO;}
 return GK177_OK;
}
Gk177Status gk177_cold_load(const char *path,const Gk79Checkpoint *o,
 uint8_t enhanced,Gk177Stages *out){
 if(!path||!*path||!o||!out)return GK177_INPUT;
 uint8_t raw[GK177_BYTES+1u];size_t n=0;int er=0;
#ifdef _WIN32
 FILE *fp=fopen(path,"rb");if(!fp)return GK177_IO;
 n=fread(raw,1,sizeof raw,fp);er=ferror(fp);int cl=fclose(fp);
#else
 int fd=open(path,O_RDONLY|O_NOFOLLOW);
 if(fd<0)return GK177_IO;
 struct stat st;
 if(fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_nlink!=1||
     (st.st_mode&077u)!=0u||st.st_uid!=geteuid()||
     st.st_size!=(off_t)GK177_BYTES){(void)close(fd);return GK177_CORRUPT;}
 while(n<sizeof raw){ssize_t k=read(fd,raw+n,sizeof raw-n);
   if(k<0&&errno==EINTR)continue;
   if(k<0){er=1;break;}if(k==0)break;n+=(size_t)k;}
 int cl=close(fd);
#endif
 if(er||cl)return GK177_IO;
 return gk177_decode(o,enhanced,raw,n,out);
}
