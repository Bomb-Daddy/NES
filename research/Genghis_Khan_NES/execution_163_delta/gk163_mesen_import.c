#include "gk163_mesen_import.h"
#include "gk139_mesen_state.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef GK163_HAS_ZLIB
#include <zlib.h>
#endif

#define GK163_MSS_LIMIT (1024u*1024u)
#define GK163_STATE_LIMIT (1024u*1024u)
#define GK163_SAVE_OFFSET 0x0dcdu
#ifdef GK163_HAS_ZLIB
static uint32_t get32(const uint8_t *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static void wipe(void *p,size_t n){
 volatile uint8_t *v=(volatile uint8_t*)p;
 while(n--)*v++=0;
}
#endif
/* The Mesen1 v2-serialized .mss envelope has two DEFLATE blocks: an
 * untrusted screenshot followed by the length-prefixed serialized CPU/PRG
 * state. We NEVER decode the screenshot. Input length, image block length,
 * ROM display filename, expected state size, DEFLATE completion and exact
 * end-of-file are independently checked. None authenticates source by itself.
 */
Gk163Status gk163_mesen_extract(const char *path,Gk77Profile profile,
 uint8_t out[GK163_MAX_RECORDS],size_t *out_size){
 if(!path||!*path||!out||!out_size||
    (profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK163_ARGUMENT;
#ifndef GK163_HAS_ZLIB
 return GK163_UNAVAILABLE;
#else
 Gk163Status status=GK163_FORMAT;
 uint8_t *src=NULL,*raw=NULL;
 size_t raw_len=0u;
 long signed_size=0;
 size_t size=0;
 FILE *fp=fopen(path,"rb");
 if(!fp)return GK163_IO;
 if(fseek(fp,0,SEEK_END)){status=GK163_IO;goto finish;}
 signed_size=ftell(fp);
 if(signed_size<50||signed_size>(long)GK163_MSS_LIMIT||fseek(fp,0,SEEK_SET)){
  status=GK163_FORMAT;goto finish;
 }
 size=(size_t)signed_size;
 src=(uint8_t*)malloc(size);
 if(!src){status=GK163_IO;goto finish;}
 if(fread(src,1,size,fp)!=size||ferror(fp)){
  status=GK163_IO;goto finish;
 }
 if(fclose(fp)!=0){fp=NULL;status=GK163_IO;goto finish;}
 fp=NULL;
 if(memcmp(src,"MSS\1",4)!=0){status=GK163_FORMAT;goto finish;}
 size_t image_len=get32(src+31);
 if(image_len>size-35u){status=GK163_FORMAT;goto finish;}
 size_t pos=35u+image_len;
 if(pos>size||size-pos<4u){status=GK163_FORMAT;goto finish;}
 uint32_t name_len=get32(src+pos);pos+=4u;
 static const char name[]="Genghis Khan (USA).nes";
 if(name_len!=sizeof name-1u||name_len>size-pos||
    memcmp(src+pos,name,sizeof name-1u)!=0){status=GK163_FORMAT;goto finish;}
 pos+=name_len;
 if(size-pos<9u||src[pos++]!=1u){status=GK163_FORMAT;goto finish;}
 uint32_t uncompressed=get32(src+pos);pos+=4;
 uint32_t compressed=get32(src+pos);pos+=4;
 if(uncompressed<18000u||uncompressed>GK163_STATE_LIMIT||
    compressed<16u||compressed>GK163_MSS_LIMIT||
    (size_t)compressed!=size-pos){status=GK163_FORMAT;goto finish;}
 raw_len=uncompressed;
 raw=(uint8_t*)malloc(raw_len);
 if(!raw){status=GK163_IO;goto finish;}
 uLongf destination=(uLongf)uncompressed;
 if(uncompress(raw,&destination,src+pos,(uLong)compressed)!=Z_OK||
    destination!=uncompressed){status=GK163_FORMAT;goto finish;}
 Gk139MesenProof proof={0};
 if(gk139_parse_uncompressed_state(raw,uncompressed,&proof)!=GK139_STATE_OK){
  status=GK163_FORMAT;goto finish;
 }
 if(proof.country_bank!=GK139_BANK_SAVE){status=GK163_BANK;goto finish;}
 const uint8_t *save=NULL;
 size_t cursor=0;
 while(cursor<(size_t)uncompressed){
  size_t start=cursor;
  while(cursor<(size_t)uncompressed&&raw[cursor]!=0u){
   if(cursor-start>=127u||raw[cursor]<33u||raw[cursor]>126u){
    status=GK163_FORMAT;goto finish;
   }
   cursor++;
  }
  if(cursor==(size_t)uncompressed||cursor==start||
     (size_t)uncompressed-cursor<5u){status=GK163_FORMAT;goto finish;}
  size_t key_len=cursor-start;
  ++cursor;
  uint32_t field_len=get32(raw+cursor);cursor+=4;
  if((size_t)field_len>(size_t)uncompressed-cursor){status=GK163_FORMAT;goto finish;}
  if(key_len==sizeof("mapper.saveRam")-1u &&
     memcmp(raw+start,"mapper.saveRam",sizeof("mapper.saveRam")-1u)==0){
   if(save||field_len!=8192u){status=GK163_FORMAT;goto finish;}
   save=raw+cursor;
  }
  cursor+=field_len;
 }
 if(!save){status=GK163_FORMAT;goto finish;}
 size_t n=(profile==GK77_ARCHIVED_MONGOL_SAVERAM?14u:27u)*GK77_RECORD_BYTES;
 if(n>8192u-GK163_SAVE_OFFSET){status=GK163_FORMAT;goto finish;}
 uint8_t digest[32];
 gk1_sha256_bytes(save+GK163_SAVE_OFFSET,n,digest);
 if(!gk77_known_source_digest(profile,digest)){
  wipe(digest,sizeof digest);status=GK163_SHA;goto finish;
 }
 wipe(digest,sizeof digest);
 /* This is the first and ONLY caller-visible mutation. */
 memcpy(out,save+GK163_SAVE_OFFSET,n);
 *out_size=n;
 status=GK163_OK;
finish:
 if(fp)fclose(fp);
 if(raw){wipe(raw,raw_len);free(raw);}
 if(src){wipe(src,(size_t)(signed_size>0?signed_size:0));free(src);}
 return status;
#endif
}
