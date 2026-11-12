#include "gk165_fresh_json.h"
#include "gk77_stock_spring.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define JSON_LIMIT (256u*1024u)
static const char *field(const char *begin,const char *end,const char *key){
 size_t n=strlen(key);const char *p=begin;const char *hit=NULL;
 while(p<end){
  const char *s=strstr(p,key);if(!s||s+n>end)break;
  /* Exact quoted key, once in the bounded scope. */
  if(hit)return NULL;
  hit=s;p=s+n;
 }
 return hit;
}
static int number(const char *begin,const char *end,const char *key,unsigned *out){
 const char *p=field(begin,end,key);if(!p)return 0;
 p+=strlen(key);while(p<end&&isspace((unsigned char)*p))p++;
 if(p==end||*p++!=':')return 0;
 while(p<end&&isspace((unsigned char)*p))p++;
 if(p==end||!isdigit((unsigned char)*p))return 0;
 unsigned v=0u;
 while(p<end&&isdigit((unsigned char)*p)){
  unsigned d=(unsigned)(*p++-'0');if(v>1000000u)return 0;
  v=v*10u+d;
 }
 if(p<end&&*p!=','&&*p!='}'&&!isspace((unsigned char)*p))return 0;
 *out=v;return 1;
}
static int key_object(const char *begin,const char *end,const char *key,
 const char **first,const char **last){
 const char *p=field(begin,end,key);if(!p)return 0;
 p+=strlen(key);while(p<end&&isspace((unsigned char)*p))p++;
 if(p==end||*p++!=':')return 0;
 while(p<end&&isspace((unsigned char)*p))p++;
 if(p==end||*p!='{')return 0;
 *first=p+1;
 /* These specific receipt objects are flat; refuse nested or escaped fields
  * rather than mistakenly attributing arbitrary metadata to WorkRAM. */
 int depth=1;
 for(p++;p<end;p++){
  if(*p=='{')depth++;
  if(*p=='}'&&!--depth){*last=p;return 1;}
  if(depth>1)return 0;
 }
 return 0;
}
static int hexval(char c){
 if(c>='0'&&c<='9')return c-'0';
 if(c>='a'&&c<='f')return c-'a'+10;
 return -1;
}
Gk165Status gk165_extract_fresh_json(const char *path,
 uint8_t out[GK165_FRESH_SIZE]){
 if(!path||!*path||!out)return GK165_ARGUMENT;
 FILE *fp=fopen(path,"rb");if(!fp)return GK165_IO;
 char *text=malloc(JSON_LIMIT+1u);if(!text){fclose(fp);return GK165_IO;}
 size_t n=fread(text,1,JSON_LIMIT+1u,fp);
 int read_error=ferror(fp),close_error=fclose(fp);
 if(read_error||close_error){free(text);return GK165_IO;}
 if(n>JSON_LIMIT){free(text);return GK165_FORMAT;}
 text[n]='\0';
 if(memchr(text,'\0',n)){free(text);return GK165_FORMAT;}
 const char *end=text+n,*w0=NULL,*w1=NULL,*s0=NULL,*s1=NULL;
 unsigned year=0,rate=0,gold=0,orders=0,countries=0,save_countries=1;
 unsigned food=0,troops=0;
 /* EXEC0210: the original capture script incorrectly populated its root
  * display fields from SaveRAM even though fresh reset stores the live
  * original country records in WorkRAM. Trust *neither* root aliases nor
  * a single WorkRAM view without cross-checking the CPU-mapped address
  * view and the distinct blank SaveRAM view. The known erroneous root
  * zeroes are permitted only as one complete legacy display mode. */
 const char *m0=NULL,*m1=NULL,*cpu0=NULL,*cpu1=NULL;
 static const char *keys[]={"\"year\"","\"rate\"","\"gold\"",
   "\"food\"","\"troops\"","\"orders\"","\"country_count\""};
 unsigned mapped=0,work=0,saved=0;
 unsigned root_year=0,root_rate=0,root_gold=0,root_orders=0,root_countries=0;
 Gk165Status result=GK165_FORMAT;
 if(!key_object(text,end,"\"nesWorkRam\"",&w0,&w1)||
    !key_object(text,end,"\"nesSaveRam\"",&s0,&s1)||
    !key_object(text,end,"\"nesMemory\"",&m0,&m1)||
    !key_object(text,end,"\"cpu\"",&cpu0,&cpu1)||
    !number(w0,w1,"\"year\"",&year)||
    !number(w0,w1,"\"rate\"",&rate)||
    !number(w0,w1,"\"gold\"",&gold)||
    !number(w0,w1,"\"food\"",&food)||
    !number(w0,w1,"\"troops\"",&troops)||
    !number(w0,w1,"\"orders\"",&orders)||
    !number(w0,w1,"\"country_count\"",&countries)||
    !number(s0,s1,"\"country_count\"",&save_countries))goto done;
 for(size_t i=0;i<sizeof keys/sizeof keys[0];i++){
  if(!number(m0,m1,keys[i],&mapped)||
     !number(w0,w1,keys[i],&work)||
     !number(s0,s1,keys[i],&saved))goto done;
  if(mapped!=work || saved!=0u){result=GK165_PROFILE;goto done;}
 }
 if(!number(cpu1,end,"\"year\"",&root_year)||
    !number(cpu1,end,"\"rate\"",&root_rate)||
    !number(cpu1,end,"\"gold\"",&root_gold)||
    !number(cpu1,end,"\"orders\"",&root_orders)||
    !number(cpu1,end,"\"countries\"",&root_countries))goto done;
 /* The legacy original probe read those five labels from the wrong bank;
  * their zeros are NOT evidence of an empty Spring. A corrected reader
  * must instead report all five authentic WorkRAM values as a group. */
 if(!((root_year==0u && root_rate==0u && root_gold==0u &&
       root_orders==0u && root_countries==0u)||
      (root_year==year && root_rate==rate && root_gold==gold &&
       root_orders==orders && root_countries==countries))){
   result=GK165_PROFILE;goto done;
 }
 /* Receipt-specific checks: these values belong ONLY to the named owner
  * Mesen fresh observation, NOT a universally hardcoded initial Spring. */
 if(year!=1175u||rate!=121u||gold!=430u||food!=208u||
    troops!=61u||orders!=3u||countries!=14u||save_countries!=0u){
   result=GK165_PROFILE;goto done;
 }
 const char *stage=field(text,end,"\"stage\"");
 if(!stage){result=GK165_FORMAT;goto done;}
 stage+=strlen("\"stage\"");
 while(stage<end&&isspace((unsigned char)*stage))stage++;
 if(stage==end||*stage++!=':'){result=GK165_FORMAT;goto done;}
 while(stage<end&&isspace((unsigned char)*stage))stage++;
 if((size_t)(end-stage)<strlen("\"14_after_wait\"")||
    memcmp(stage,"\"14_after_wait\"",strlen("\"14_after_wait\""))!=0){
   result=GK165_PROFILE;goto done;
 }
 const char *p=field(text,end,"\"original_country_records_workram\"");
 if(!p){result=GK165_FORMAT;goto done;}
 p+=strlen("\"original_country_records_workram\"");
 while(p<end&&isspace((unsigned char)*p))p++;
 if(p==end||*p++!=':'){result=GK165_FORMAT;goto done;}
 while(p<end&&isspace((unsigned char)*p))p++;
 if(p==end||*p++!='\"'){result=GK165_FORMAT;goto done;}
 if((size_t)(end-p)<GK165_FRESH_SIZE*2u+1u){result=GK165_FORMAT;goto done;}
 uint8_t candidate[GK165_FRESH_SIZE];
 for(size_t i=0;i<GK165_FRESH_SIZE;i++){
  int hi=hexval(p[i*2u]),lo=hexval(p[i*2u+1u]);
  if(hi<0||lo<0){result=GK165_FORMAT;goto done;}
  candidate[i]=(uint8_t)((hi<<4)|lo);
 }
 if(p[GK165_FRESH_SIZE*2u]!='\"'){result=GK165_FORMAT;goto done;}
 uint8_t sha[32];gk1_sha256_bytes(candidate,sizeof candidate,sha);
 if(!gk77_known_source_digest(GK77_FRESH_MONGOL_WORKRAM,sha)){
  result=GK165_SHA;goto done;
 }
 memcpy(out,candidate,sizeof candidate);result=GK165_OK;
done:
 memset(text,0,n);free(text);
 return result;
}
