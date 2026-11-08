#include "gk82_stock_cartridge.h"
#include "gk81_stock_attach.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK82_IMAGE_SIZE 262160u
#define GK82_PRG_START 16u
#define GK82_LAST_BANK_START (GK82_PRG_START+15u*16384u)
static const char *const fingerprint="ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3";
static unsigned get16(const unsigned char *p){return (unsigned)p[0]|((unsigned)p[1]<<8);}
static int digest_equal(const uint8_t sha[32],const char *hex){
 static const char *digits="0123456789abcdef";
 for(unsigned i=0;i<32;i++)if(digits[sha[i]>>4]!=hex[2*i]||digits[sha[i]&15u]!=hex[2*i+1])return 0;
 return 1;
}
int gk82_rom_path(const char *root,char *output,size_t cap){
 const char *const filename="stock_genghis_khan_usa.nes";
 char derived[2300];
 if(!output||!gk81_stock_path(root,GK77_FRESH_MONGOL_WORKRAM,derived,sizeof derived))return 0;
 size_t n=strlen(derived),m=strlen(filename);
 while(n && derived[n-1]!='/' && derived[n-1]!='\\')n--;
 if(!n||n+m>=cap)return 0;
 memcpy(output,derived,n);memcpy(output+n,filename,m+1);
 return 1;
}
Gk82RomStatus gk82_probe_stock_rom(const char *filename,Gk82RomBootstrap *out){
 if(!filename||!*filename||!out)return GK82_ROM_INVALID;
 FILE *file=fopen(filename,"rb");if(!file)return GK82_ROM_IO;
 unsigned char *p=malloc(GK82_IMAGE_SIZE+1u);
 if(!p){fclose(file);return GK82_ROM_IO;}
 const size_t n=fread(p,1,GK82_IMAGE_SIZE+1u,file);
 const int bad=ferror(file),close_bad=fclose(file);
 if(bad||close_bad){free(p);return GK82_ROM_IO;}
 if(n!=GK82_IMAGE_SIZE){free(p);return GK82_ROM_LENGTH;}
 static const unsigned char header[16]={
  'N','E','S',0x1a,16,0,0x12,0,0,0,0,0,0,0,0,0
 };
 if(memcmp(p,header,sizeof header)!=0){free(p);return GK82_ROM_HEADER;}
 uint8_t sha[32];gk1_sha256_bytes(p,GK82_IMAGE_SIZE,sha);
 if(!digest_equal(sha,fingerprint)){free(p);return GK82_ROM_SHA;}
 /* MMC1 maps the fixed final 16 KiB PRG bank into CPU $C000-$FFFF.
  * This physical mapping is source-static and requires no simulated RNG. */
 const unsigned vectors=GK82_LAST_BANK_START+0x3ffau;
 Gk82RomBootstrap next={0};
 next.nmi_vector=(uint16_t)get16(p+vectors);
 next.reset_vector=(uint16_t)get16(p+vectors+2);
 next.irq_vector=(uint16_t)get16(p+vectors+4);
 next.mapper=1;next.prg_banks=16;next.chr_banks=0;
 /* Independently observed in the exact original ROM: reset $C000 begins
  * SEI, CLD, LDX #$FD, TXS, LDA #$FF. Do not claim further steps/RNG. */
 static const unsigned char first[]={0x78,0xd8,0xa2,0xfd,0x9a,0xa9,0xff};
 const unsigned boot=GK82_LAST_BANK_START+(unsigned)(next.reset_vector-0xc000u);
 if(next.reset_vector!=0xc000u||next.nmi_vector!=0xc0d9u||next.irq_vector!=0xc14bu||
    boot+sizeof first>GK82_IMAGE_SIZE||memcmp(p+boot,first,sizeof first)!=0){
   free(p);return GK82_ROM_RESET;
 }
 next.boot_instruction_count=5u;
 free(p);*out=next;return GK82_ROM_OK;
}
