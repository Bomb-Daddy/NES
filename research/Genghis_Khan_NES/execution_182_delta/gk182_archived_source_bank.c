#include "gk182_archived_source_bank.h"
#include "gk163_mesen_import.h"
#include "gk82_stock_cartridge.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef GK163_HAS_ZLIB
#include <zlib.h>
#endif
#define GK182_MAX_MSS (1024u*1024u)
#define GK182_MAX_STATE (1024u*1024u)
/* Published DIGESTS ONLY, no original ROM, graphics, RAM or savestate bytes. */
static const uint8_t archived_mongol_sha[32]={
 0xa3,0x83,0x87,0x05,0xc5,0x80,0x10,0xa1,
 0x1c,0xac,0x37,0xae,0x2d,0x8f,0x44,0x6c,
 0xcc,0xf2,0x9a,0xa7,0xb9,0x04,0xf4,0x1b,
 0x19,0x9a,0x60,0x4a,0x98,0x24,0x5c,0xce};
static const uint8_t archived_world_sha[32]={
 0xd0,0x80,0x6e,0x73,0xbd,0x98,0xf3,0xf2,
 0xc4,0xdf,0x1d,0x14,0x58,0xdc,0x66,0x8d,
 0xbe,0x8a,0x71,0x1d,0x92,0x5d,0xa9,0x5a,
 0xe5,0xef,0x82,0x39,0x3d,0x13,0x89,0xfb};
#ifdef GK163_HAS_ZLIB
static uint32_t le32(const uint8_t *p){
 return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static void erase_bytes(void *ptr,size_t count){
 volatile uint8_t *p=(volatile uint8_t*)ptr;while(count--)*p++=0;
}
static int read_single_original(const char *path,Gk77Profile profile,
 uint8_t **raw_out,size_t *length){
 FILE *f=fopen(path,"rb");if(!f)return 0;
 int ok=0;uint8_t *p=NULL;size_t n=0;
 if(fseek(f,0,SEEK_END))goto close_file;
 long len=ftell(f);
 if(len<70L||len>(long)GK182_MAX_MSS||fseek(f,0,SEEK_SET))goto close_file;
 n=(size_t)len;p=malloc(n+1u);if(!p)goto close_file;
 if(fread(p,1,n+1u,f)!=n||ferror(f))goto close_file;
 uint8_t sha[32];gk1_sha256_bytes(p,n,sha);
 const uint8_t *expected=profile==GK77_ARCHIVED_MONGOL_SAVERAM?
   archived_mongol_sha:archived_world_sha;
 ok=memcmp(sha,expected,sizeof sha)==0;
 erase_bytes(sha,sizeof sha);
close_file:
 if(fclose(f))ok=0;
 if(!ok){if(p){erase_bytes(p,n+1u);free(p);}return 0;}
 *raw_out=p;*length=n;return 1;
}
static int inflate_archived(const uint8_t *file,size_t n,
 uint8_t **state,size_t *state_len){
 if(n<70u||memcmp(file,"MSS\1",4u))return 0;
 size_t img=le32(file+31u);
 if(img>n-35u)return 0;
 size_t pos=35u+img;
 if(n-pos<4u)return 0;
 size_t namelen=le32(file+pos);pos+=4u;
 static const char name[]="Genghis Khan (USA).nes";
 if(namelen!=sizeof(name)-1u||namelen>n-pos||
    memcmp(file+pos,name,namelen))return 0;
 pos+=namelen;
 if(n-pos<9u||file[pos++]!=1u)return 0;
 size_t plain=le32(file+pos);pos+=4u;
 size_t packed=le32(file+pos);pos+=4u;
 if(plain<18000u||plain>GK182_MAX_STATE||
    packed<16u||packed!=n-pos)return 0;
 uint8_t *b=malloc(plain);if(!b)return 0;
 uLongf dest=(uLongf)plain;
 int ok=uncompress(b,&dest,file+pos,(uLong)packed)==Z_OK&&dest==plain;
 if(!ok){erase_bytes(b,plain);free(b);return 0;}
 *state=b;*state_len=plain;return 1;
}
#endif
Gk182Status gk182_original_saved_bank(const char *private_rom,
 const char *private_mss,Gk77Profile profile,Gk182SavedBankProof *out){
 if(!private_rom||!*private_rom||!private_mss||!*private_mss||!out||
    (profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK182_ARGUMENT;
#ifndef GK163_HAS_ZLIB
 return GK182_UNAVAILABLE;
#else
 Gk82RomBootstrap rom={0};
 if(gk82_probe_stock_rom(private_rom,&rom)!=GK82_ROM_OK)return GK182_SOURCE;
 /* The archived file is source-authenticated as a WHOLE before any parse,
  * including mapper, CPU seed, or game records: no simulated state can pass. */
 uint8_t *file=NULL,*state=NULL;
 size_t n=0u,state_len=0u;
 if(!read_single_original(private_mss,profile,&file,&n))return GK182_ARCHIVE;
 Gk182Status result=GK182_STATE;
 Gk182SavedBankProof *candidate=calloc(1,sizeof *candidate);
 if(!candidate){result=GK182_UNAVAILABLE;goto done;}
 if(!inflate_archived(file,n,&state,&state_len))goto done;
 Gk139MesenProof cpu={0};
 if(gk139_parse_uncompressed_state(state,state_len,&cpu)!=GK139_STATE_OK||
    cpu.country_bank!=GK139_BANK_SAVE)goto done;
 if(gk144_probe_archived_prg_map(state,state_len,
    &candidate->mapped_at_saved_instant)!=GK144_MAPPER_OK)goto done;
 if(candidate->mapped_at_saved_instant.observed_country_ram_bank!=GK139_BANK_SAVE||
    !candidate->mapped_at_saved_instant.upper_bank_is_fixed_last||
    candidate->mapped_at_saved_instant.saved_program_counter!=cpu.program_counter||
    candidate->mapped_at_saved_instant.saved_cpu_cycles!=cpu.cpu_cycles)goto done;
 /* Independent existing original-stock country-record SHA oracle. */
 uint8_t authentic[GK163_MAX_RECORDS]={0};size_t authentic_len=0;
 Gk163Status r=gk163_mesen_extract(private_mss,profile,authentic,&authentic_len);
 erase_bytes(authentic,sizeof authentic);
 if(r!=GK163_OK||authentic_len!=(profile==GK77_ARCHIVED_MONGOL_SAVERAM?14u:27u)*83u)
  goto done;
 result=GK182_SCAN;
 if(candidate->mapped_at_saved_instant.lower_prg_bank>=15u||
    gk181_scan_authenticated(private_rom,
      candidate->mapped_at_saved_instant.lower_prg_bank,
      &candidate->conditional_source_sites)!=GK181_OK)goto done;
 candidate->profile=profile;candidate->saved_seed48=cpu.seed48;
 candidate->original_mss_sha_verified=1u;
 candidate->stock_rom_sha_verified=1u;
 candidate->proof_is_saved_instant_only=1u;
 candidate->first_spring_generated=0u;
 *out=*candidate;result=GK182_OK;
done:
 if(state){erase_bytes(state,state_len);free(state);}
 if(file){erase_bytes(file,n+1u);free(file);}
 if(candidate){erase_bytes(candidate,sizeof *candidate);free(candidate);}
 return result;
#endif
}
