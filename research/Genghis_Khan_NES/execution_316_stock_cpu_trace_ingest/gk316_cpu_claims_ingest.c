#include "gk316_cpu_claims_ingest.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK316_MAX_FILE_BYTES 1048576u
#define GK316_HASH "ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3"
static int digit(char c,unsigned base,unsigned *v){
 unsigned x;
 if(c>='0'&&c<='9')x=(unsigned)(c-'0');
 else if(base==16u&&c>='a'&&c<='f')x=(unsigned)(c-'a'+10);
 else return 0;
 if(x>=base)return 0;
 *v=x;return 1;
}
static int number(const char *s,unsigned base,uint64_t max,uint64_t *out){
 uint64_t n=0;unsigned v;
 if(!s||!*s)return 0;
 for(;*s;s++){
  if(!digit(*s,base,&v)||n>(max-v)/base)return 0;
  n=n*base+v;
 }
 *out=n;return 1;
}
static int hexbytes(const char *s,size_t count,uint8_t *out){
 if(!s||strlen(s)!=count*2u)return 0;
 for(size_t i=0;i<count;i++){
  unsigned a,b;
  if(!digit(s[i*2u],16u,&a)||!digit(s[i*2u+1u],16u,&b))return 0;
  out[i]=(uint8_t)(a*16u+b);
 }
 return 1;
}
static int fields(char *line,char *v[],size_t max){
 size_t count=0;char *start=line;
 for(char *p=line;;p++){
  if(*p==','||*p=='\0'){
   if(count>=max||p==start)return -1;
   char end=*p;*p=0;v[count++]=start;
   if(end==0)break;
   start=p+1u;
  }
 }
 return (int)count;
}
static int parse_header(char *line,Gk316Trace *result){
 char *f[4];uint8_t seed[6];
 if(fields(line,f,4)!=4||strcmp(f[0],GK316_TRACE_HEADER)!=0||
    strcmp(f[1],GK316_HASH)!=0||
    (strcmp(f[2],"M")&&strcmp(f[2],"W"))||
    !hexbytes(f[3],6u,seed))return 0;
 memcpy(result->initial_seed,seed,6u);
 result->world=(uint8_t)(f[2][0]=='W');return 1;
}
static Gk316Status parse_event(char *line,Gk316Event *e){
 char *f[10];
 int count=fields(line,f,10);
 if(count<0)return GK316_SYNTAX;
 uint64_t num=0;memset(e,0,sizeof *e);
 if(strcmp(f[0],"N")==0){
  if(count!=3||!number(f[1],10u,UINT64_MAX,&e->cycle)||!e->cycle||
     !number(f[2],16u,255u,&num))return GK316_SYNTAX;
  e->kind=GK316_NMI;e->gate=(uint8_t)num;e->pc=0xc106u;e->bank=15u;
 }else if(strcmp(f[0],"S")==0){
  if(count!=10||!number(f[1],10u,UINT64_MAX,&e->cycle)||!e->cycle||
     strlen(f[2])!=4u||!number(f[2],16u,65535u,&num)||num<0x8000u)
    return GK316_SYNTAX;
  e->pc=(uint16_t)num;e->kind=GK316_STORE;
  if(!number(f[3],10u,15u,&num))return GK316_SYNTAX;
  e->bank=(uint8_t)num;
  uint8_t bytes[6];
  for(unsigned i=0;i<6u;i++)if(!hexbytes(f[i+4u],1u,bytes+i))return GK316_SYNTAX;
  e->target=bytes[0];e->before=bytes[1];e->after=bytes[2];
  e->a=bytes[3];e->x=bytes[4];e->y=bytes[5];
  if(e->target<0x84u||e->target>0x89u)return GK316_SYNTAX;
 }else if(strcmp(f[0],"C")==0){
  if(count!=7||!number(f[1],10u,UINT64_MAX,&e->cycle)||!e->cycle||
     strcmp(f[2],"c289")||strcmp(f[3],"15")||
     !hexbytes(f[4],6u,e->rng_before)||
     !hexbytes(f[5],6u,e->rng_after)||
     !number(f[6],10u,32767u,&num))return GK316_SYNTAX;
  e->kind=GK316_C289;e->pc=0xc289u;e->bank=15u;e->draw15=(uint16_t)num;
 }else return GK316_SYNTAX;
 return GK316_OK;
}
Gk316Status gk316_load_cpu_claims(const char *path,Gk316Trace *out){
 if(!path||!*path||!out)return GK316_ARGUMENT;
 FILE *f=fopen(path,"rb");if(!f)return GK316_IO;
 /* One private log cannot force an unbounded allocation, and never updates
  * the caller on partial EOF, bad checksum claim, malformed trace or I/O. */
 if(fseek(f,0,SEEK_END)!=0){fclose(f);return GK316_IO;}
 long size=ftell(f);
 if(size<0||size>GK316_MAX_FILE_BYTES||fseek(f,0,SEEK_SET)!=0){
  fclose(f);return size>GK316_MAX_FILE_BYTES?GK316_LIMIT:GK316_IO;
 }
 unsigned char *bytes=malloc((size_t)size+1u);
 Gk316Trace *next=calloc(1,sizeof *next);
 if(!bytes||!next){free(bytes);free(next);fclose(f);return GK316_IO;}
 size_t read=fread(bytes,1,(size_t)size,f);
 int err=ferror(f),closed=fclose(f);
 if(err||closed||read!=(size_t)size){free(bytes);free(next);return GK316_IO;}
 Gk316Status result=GK316_OK;
 if(!size||bytes[size-1]!='\n'||memchr(bytes,0,(size_t)size)||
    memchr(bytes,'\r',(size_t)size))result=GK316_SYNTAX;
 if(result==GK316_OK){
  bytes[size]=0;char *start=(char *)bytes;size_t lineno=0;
  uint64_t prior=0;
  for(size_t i=0;i<(size_t)size;i++)if(bytes[i]=='\n'){
   bytes[i]=0;lineno++;
   if(i==0||(lineno==1u&&!parse_header(start,next))){result=GK316_SYNTAX;break;}
   if(lineno>1u){
    if(next->event_count>=GK316_EVENT_LIMIT){result=GK316_LIMIT;break;}
    Gk316Event event;
    result=parse_event(start,&event);
    if(result!=GK316_OK)break;
    if(event.cycle<=prior){result=GK316_ORDER;break;}
    prior=event.cycle;
    next->events[next->event_count++]=event;
    if(event.kind==GK316_NMI)next->nmi_count++;
    if(event.kind==GK316_STORE)next->store_count++;
    if(event.kind==GK316_C289)next->c289_count++;
   }
   start=(char *)&bytes[i+1u];
  }
  if(result==GK316_OK&&(!next->event_count||!next->c289_count))result=GK316_SYNTAX;
 }
 if(result==GK316_OK){next->source_claim_only=1u;*out=*next;}
 free(bytes);free(next);return result;
}