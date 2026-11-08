#include "gk74_spring_archive.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static uint16_t le16(const uint8_t *b,unsigned i){return (uint16_t)((uint16_t)b[i]|((uint16_t)b[i+1]<<8));}
Gk74Result gk74_decode_record(const uint8_t *b,size_t n,Gk74SpringCountry *out){
 if(!b||!out||n!=GK74_REC_BYTES)return GK74_BAD_INPUT;
 /* $6DCD + 83*i: fields confirmed by EXEC0070 archived-memory crosswalk;
    not the original game's initial Spring transition formula. */
 Gk74SpringCountry next={0};
 next.arms=le16(b,20);next.skill=le16(b,22);next.troops=le16(b,24);
 next.economy=le16(b,26);next.food=le16(b,54);next.gold=le16(b,64);
 *out=next;return GK74_OK;
}
Gk74Result gk74_load_file(const char *path,Gk74SpringArchive *out){
 static const uint8_t exact[32]={
  0xdd,0x22,0x84,0x71,0x40,0xcc,0x03,0x97,0xab,0x8f,0x42,0xe2,0x9f,0x18,0x32,0x68,
  0x16,0x28,0xed,0x52,0xbf,0xa1,0x7e,0x95,0x04,0x1c,0x28,0x8a,0xfc,0x4e,0xf7,0x73};
 if(!path||!out)return GK74_BAD_INPUT;
 FILE *f=fopen(path,"rb");if(!f)return GK74_IO;
 uint8_t *b=(uint8_t*)malloc(GK74_RAM_BYTES+1u);
 if(!b){(void)fclose(f);return GK74_IO;}
 size_t n=fread(b,1,GK74_RAM_BYTES+1u,f);
 int fail=ferror(f),closed=fclose(f);
 if(fail||closed){free(b);return GK74_IO;}
 if(n!=GK74_RAM_BYTES){free(b);return GK74_SIZE;}
 uint8_t digest[32];gk1_sha256_bytes(b,n,digest);
 if(memcmp(digest,exact,32)!=0){free(b);return GK74_SHA;}
 Gk74SpringArchive next={0};
 for(unsigned i=0;i<GK74_COUNTRIES;i++){
  Gk74Result r=gk74_decode_record(b+GK74_FIRST_RAM_OFFSET+i*GK74_REC_BYTES,
                                  GK74_REC_BYTES,&next.countries[i]);
  if(r!=GK74_OK){free(b);return r;}
 }
 free(b);*out=next;return GK74_OK;
}
