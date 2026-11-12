#include "gk208_archived_rng.h"
#include "gk163_mesen_import.h"
#include "gk178_source_rng.h"
#include "gk137_stock_rng.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint64_t from_bytes(const uint8_t x[6]){
 uint64_t n=0;for(unsigned i=0;i<6;i++)n=(n<<8)|x[i];return n;
}
static void scrub(void *ptr,size_t n){
 volatile uint8_t *p=(volatile uint8_t *)ptr;while(n--)*p++=0;
}
/* Exact Director-provided archived original Mesen savestate identities.
 * These are SHA-256 fingerprints ONLY, not original NES binary or save data.
 * Verified country records alone do not authenticate CPU RNG bytes: a
 * forged state could preserve all country records while changing seed.
 * Source: private original checkpoint files inside verified EXEC0076 ZIP. */
static const uint8_t trusted_sha[2][32]={
 {0xa3,0x83,0x87,0x05,0xc5,0x80,0x10,0xa1,0x1c,0xac,0x37,0xae,0x2d,0x8f,0x44,0x6c,
  0xcc,0xf2,0x9a,0xa7,0xb9,0x04,0xf4,0x1b,0x19,0x9a,0x60,0x4a,0x98,0x24,0x5c,0xce},
 {0xd0,0x80,0x6e,0x73,0xbd,0x98,0xf3,0xf2,0xc4,0xdf,0x1d,0x14,0x58,0xdc,0x66,0x8d,
  0xbe,0x8a,0x71,0x1d,0x92,0x5d,0xa9,0x5a,0xe5,0xef,0x82,0x39,0x3d,0x13,0x89,0xfb}
};
static int exact_archived_mss(const char *filename,Gk77Profile profile){
 FILE *fp=fopen(filename,"rb");if(!fp)return 0;
 if(fseek(fp,0,SEEK_END)!=0){fclose(fp);return 0;}
 long length=ftell(fp);
 if(length<64 || length>1024*1024 ||fseek(fp,0,SEEK_SET)!=0){fclose(fp);return 0;}
 uint8_t *bytes=(uint8_t*)malloc((size_t)length);
 if(!bytes){fclose(fp);return 0;}
 int ok=(fread(bytes,1,(size_t)length,fp)==(size_t)length);
 if(ferror(fp)||fclose(fp)!=0)ok=0;
 uint8_t digest[32]={0};
 if(ok){gk1_sha256_bytes(bytes,(size_t)length,digest);
  ok=!memcmp(digest,trusted_sha[profile==GK77_ARCHIVED_WORLD_SAVERAM?1:0],32u);}
 scrub(bytes,(size_t)length);free(bytes);scrub(digest,sizeof digest);
 return ok;
}
Gk208Status gk208_archived_source_rng(const char *stock_rom,
 const char *archived_mss,Gk77Profile profile,Gk208ArchiveProof *out){
 if(!stock_rom||!*stock_rom||!archived_mss||!*archived_mss||!out||
   (profile!=GK77_ARCHIVED_MONGOL_SAVERAM &&
    profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK208_ARGUMENT;
 if(!exact_archived_mss(archived_mss,profile))return GK208_ARCHIVE_IDENTITY;
 Gk208Status status=GK208_ARCHIVE;
 uint8_t countries[GK163_MAX_RECORDS]={0};
 Gk139MesenProof snapshot={0};
 Gk178RngResult source={0};
 Gk137StockRng native={0};
 uint16_t draw=0;size_t length=0;
 if(gk163_mesen_extract_with_proof(archived_mss,profile,countries,
    &length,&snapshot)!=GK163_OK)goto finish;
 status=GK208_SOURCE;
 if(gk178_step_from_authenticated_stock(stock_rom,
   snapshot.nes_seed,&source)!=GK178_OK)goto finish;
 status=GK208_PARITY;
 if(gk137_from_nes_bytes(&native,snapshot.nes_seed)!=GK137_OK||
    gk137_draw15(&native,&draw)!=GK137_OK||
    native.seed48!=from_bytes(source.after)||
    draw!=(uint16_t)(((native.seed48>>32u)&0x7fffu))||
    snapshot.country_bank!=GK139_BANK_SAVE)goto finish;
 Gk208ArchiveProof proof={0};
 proof.profile=profile;
 proof.archived_pc=snapshot.program_counter;
 proof.archived_cpu_cycles=snapshot.cpu_cycles;
 proof.archived_seed48=snapshot.seed48;
 proof.hypothetical_next_seed48=native.seed48;
 proof.hypothetical_next_draw15=draw;
 proof.country_count=(unsigned)(length/83u);
 *out=proof;
 status=GK208_OK;
finish:
 scrub(countries,sizeof countries);scrub(&snapshot,sizeof snapshot);
 scrub(&source,sizeof source);scrub(&native,sizeof native);
 return status;
}
