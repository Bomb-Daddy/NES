#include "gk297_stock_reset_seed.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK297_ROM_LEN 262160u
#define GK297_FIXED_BANK (16u+15u*16384u)
static const uint8_t stock_hash[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3};
static int reset_store(const uint8_t *prg,uint16_t pc,uint8_t target){
 const size_t offset=(size_t)(pc-0xc000u);
 return prg[offset]==0x8du && prg[offset+1u]==target &&
        prg[offset+2u]==0u;
}
Gk297Status gk297_reset_initializer_from_source(
 const char *rom,const uint8_t observed_tail[2],Gk297ResetSeed *out){
 if(!rom||!*rom||!observed_tail||!out)return GK297_ARGUMENT;
 FILE *f=fopen(rom,"rb");if(!f)return GK297_ROM_IO;
 uint8_t *data=malloc(GK297_ROM_LEN+1u);
 if(!data){fclose(f);return GK297_ROM_IO;}
 const size_t len=fread(data,1,GK297_ROM_LEN+1u,f);
 const int read_error=ferror(f),close_error=fclose(f);
 if(read_error||close_error){free(data);return GK297_ROM_IO;}
 if(len!=GK297_ROM_LEN){free(data);return GK297_ROM_IDENTITY;}
 uint8_t digest[32];gk1_sha256_bytes(data,len,digest);
 if(memcmp(digest,stock_hash,32u)!=0||memcmp(data,"NES\032",4u)!=0){
  free(data);return GK297_ROM_IDENTITY;
 }
 const uint8_t *p=data+GK297_FIXED_BANK;
 /* Authenticate actual reset LDA#.../STA targets and adjacent C289
  * entry OR $89 behavior. Store the two LDA immediate bytes as derived
  * values, not arbitrary Spring-country constants. */
 const int authentic=p[0xb8u]==0xa9u &&
   reset_store(p,0xc0bau,0x84u) &&reset_store(p,0xc0bdu,0x86u) &&
   p[0xc0u]==0xa9u &&
   reset_store(p,0xc0c2u,0x85u) &&reset_store(p,0xc0c5u,0x87u) &&
   p[0x289u]==0xadu &&p[0x28au]==0x89u &&p[0x28bu]==0u &&
   p[0x28cu]==0x09u &&p[0x28du]==1u &&
   reset_store(p,0xc28eu,0x89u);
 if(!authentic){free(data);return GK297_SOURCE_MISMATCH;}
 Gk297ResetSeed proof={0};
 proof.reset_initializer_seed[0]=p[0xb9u];
 proof.reset_initializer_seed[2]=p[0xb9u];
 proof.reset_initializer_seed[1]=p[0xc1u];
 proof.reset_initializer_seed[3]=p[0xc1u];
 proof.reset_initializer_seed[4]=observed_tail[0];
 proof.reset_initializer_seed[5]=observed_tail[1];
 free(data);
 proof.original_source_written_mask=0x0fu;
 proof.externally_observed_ram_mask=0x30u;
 proof.original_rom_sha256_verified=1u;
 proof.source_static_only=1u;
 /* Crucial: 948 captured seed writes and 289 RNG calls in EXEC0296;
  * initializer values are NOT the general first-C289 preseed. */
 proof.first_c289_seed_proven=0u;
 proof.gameplay_spring_unlocked=0u;
 *out=proof;
 return GK297_OK;
}
