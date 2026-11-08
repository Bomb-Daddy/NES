#include "gk77_stock_spring.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* SHA256 fingerprints from 2026-11-07 exact stock NES/Mesen original RAM:
 * fresh nesWorkRam; restored nesSaveRam (NEVER confuse with nesMemory).
 * Public metadata only. Raw checkpoint bytes stay private in Drive.
 */
static const char *sha_of(Gk77Profile p){
 switch(p){
 case GK77_FRESH_MONGOL_WORKRAM:return "04eb853683a3dada2a6a127baf61b6be2332114213cedc9e295c183647c6c73e";
 case GK77_ARCHIVED_MONGOL_SAVERAM:return "ba3f26ae278356a8e5f5b2ecb93ac1d576ea579fff3920bda2e52a05a764c328";
 case GK77_ARCHIVED_WORLD_SAVERAM:return "0f9f254df49268a00cfb1599a5617364ba4511e2bb098da716dd845c1b1178e5";
 default:return NULL;
 }
}
const char *gk77_profile_name(Gk77Profile p){
 switch(p){
 case GK77_FRESH_MONGOL_WORKRAM:return "fresh";
 case GK77_ARCHIVED_MONGOL_SAVERAM:return "mongol";
 case GK77_ARCHIVED_WORLD_SAVERAM:return "world";
 default:return "invalid";
 }
}
Gk77Profile gk77_profile_parse(const char *s){
 if(!s)return 0;
 if(strcmp(s,"fresh")==0)return GK77_FRESH_MONGOL_WORKRAM;
 if(strcmp(s,"mongol")==0)return GK77_ARCHIVED_MONGOL_SAVERAM;
 if(strcmp(s,"world")==0)return GK77_ARCHIVED_WORLD_SAVERAM;
 return 0;
}
static unsigned limit(Gk77Profile p){return p==GK77_ARCHIVED_WORLD_SAVERAM?27u:14u;}
static uint16_t get16(const uint8_t *r,unsigned at){return (uint16_t)((uint16_t)r[at]|((uint16_t)r[at+1]<<8));}
Gk77Result gk77_decode(const uint8_t *b,size_t size,Gk77Country *out){
 if(!b||!out||size!=GK77_RECORD_BYTES)return GK77_INVALID;
 Gk77Country next={0};
 next.arms=get16(b,0x14);next.skill=get16(b,0x16);
 next.troops=get16(b,0x18);next.economy=get16(b,0x1a);
 next.town=get16(b,0x1c);next.defense=get16(b,0x1e);
 next.masons=get16(b,0x20);
 for(unsigned i=0;i<10;i++)next.goods[i]=get16(b,0x22u+2u*i);
 next.food=get16(b,0x36);next.artisans=get16(b,0x3a);
 next.food_workers=get16(b,0x3e);next.gold=get16(b,0x40);
 next.morale=get16(b,0x42);next.tax=get16(b,0x44);
 next.extra_tax=get16(b,0x46);next.undrafted=get16(b,0x49);
 memcpy(next.raw,b,GK77_RECORD_BYTES);
 *out=next;return GK77_OK;
}
static int same_hex(const uint8_t b[32],const char *hex){
 const char *digits="0123456789abcdef";
 for(unsigned i=0;i<32;i++){
  if(digits[b[i]>>4]!=hex[2*i]||digits[b[i]&15u]!=hex[2*i+1])return 0;
 }
 return 1;
}
Gk77Result gk77_load(const char *path,Gk77Profile profile,Gk77StockSpring *out){
 const char *sha=sha_of(profile);
 if(!path||!out||!sha)return GK77_INVALID;
 FILE *fp=fopen(path,"rb");if(!fp)return GK77_IO;
 size_t n=GK77_RECORD_BYTES*limit(profile);
 uint8_t *b=malloc(n+1u);
 if(!b){(void)fclose(fp);return GK77_IO;}
 size_t got=fread(b,1,n+1u,fp);
 int problem=ferror(fp),close_error=fclose(fp);
 if(problem||close_error){free(b);return GK77_IO;}
 if(got!=n){free(b);return GK77_SIZE;}
 uint8_t digest[32];gk1_sha256_bytes(b,n,digest);
 if(!same_hex(digest,sha)){free(b);return GK77_SHA;}
 Gk77StockSpring temp={0};temp.profile=profile;
 temp.scenario=(uint8_t)(profile==GK77_ARCHIVED_WORLD_SAVERAM?2:1);
 temp.country_count=(uint8_t)limit(profile);temp.orders=3;
 temp.year=(uint16_t)(temp.scenario==1?1175:1206);
 temp.rate=(uint16_t)(profile==GK77_FRESH_MONGOL_WORKRAM?121u:
                      profile==GK77_ARCHIVED_MONGOL_SAVERAM?101u:114u);
 for(unsigned i=0;i<limit(profile);i++){
  if(gk77_decode(b+i*GK77_RECORD_BYTES,GK77_RECORD_BYTES,&temp.countries[i])!=GK77_OK){
   free(b);return GK77_INVALID;
  }
 }
 free(b);*out=temp;return GK77_OK;
}
