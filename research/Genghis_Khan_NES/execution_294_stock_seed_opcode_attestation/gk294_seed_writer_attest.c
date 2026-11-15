#include "gk294_seed_writer_attest.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK294_STOCK_LEN 262160u
#define GK294_PRG_BYTES 16384u
/* Fingerprint is public metadata. No copyrighted program data embedded. */
static const uint8_t expected_sha[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3
};
static Gk294Status read_authenticated_rom(const char *path,uint8_t **out){
 FILE *f=fopen(path,"rb");if(!f)return GK294_ROM_IO;
 uint8_t *bytes=malloc(GK294_STOCK_LEN+1u);
 if(!bytes){fclose(f);return GK294_ROM_IO;}
 size_t n=fread(bytes,1,GK294_STOCK_LEN+1u,f);
 int err=ferror(f),closed=fclose(f);
 if(err||closed){free(bytes);return GK294_ROM_IO;}
 if(n!=GK294_STOCK_LEN){free(bytes);return GK294_ROM_IDENTITY;}
 uint8_t sha[32];gk1_sha256_bytes(bytes,n,sha);
 if(memcmp(sha,expected_sha,sizeof sha)!=0){free(bytes);return GK294_ROM_IDENTITY;}
 *out=bytes;return GK294_OK;
}
Gk294Status gk294_attest_seed_writers(const char *rom,
 const Gk294Write *writes,size_t count,Gk294SourceProof *out){
 if(!rom||!*rom||!writes||!out||count==0||count>GK294_MAX_WRITES)
  return GK294_ARGUMENT;
 for(size_t k=0;k<count;k++){
  const Gk294Write *w=&writes[k];
  if(w->destination_zp<0x84u||w->destination_zp>0x89u||
     w->mapped_prg_bank>15u||
     (w->source_pc>=0xc000u ? w->mapped_prg_bank!=15u :
       (w->source_pc<0x8000u||w->mapped_prg_bank==15u)))
    return GK294_BANK_MISMATCH;
  if(k&&w->cpu_cycle<=writes[k-1].cpu_cycle)return GK294_CHRONOLOGY;
 }
 uint8_t *source=NULL;
 Gk294Status r=read_authenticated_rom(rom,&source);
 if(r!=GK294_OK)return r;
 uint8_t mask=0u;
 for(size_t k=0;k<count;k++){
  const Gk294Write *w=&writes[k];
  const size_t pos=16u+((size_t)w->mapped_prg_bank*GK294_PRG_BYTES)+
    (w->source_pc>=0xc000u?(size_t)(w->source_pc-0xc000u):
                             (size_t)(w->source_pc-0x8000u));
  const size_t relative=pos-(16u+((size_t)w->mapped_prg_bank*GK294_PRG_BYTES));
  const uint8_t opcode=source[pos];
  unsigned expected_size=0,operand=0;
  if(opcode==0x84u||opcode==0x85u||opcode==0x86u)expected_size=2;
  else if(opcode==0x8cu||opcode==0x8du||opcode==0x8eu)expected_size=3;
  /* Never read an operand past the end of the 16 KiB mapped PRG bank. */
  if(!expected_size||relative+expected_size>GK294_PRG_BYTES){
   free(source);return GK294_SOURCE_MISMATCH;
  }
  operand=source[pos+1u];
  if(expected_size==3)operand|=((unsigned)source[pos+2u]<<8);
  if(!expected_size||relative+expected_size>GK294_PRG_BYTES||
     operand!=(unsigned)w->destination_zp){
   free(source);return GK294_SOURCE_MISMATCH;
  }
  mask|=(uint8_t)(1u<<(w->destination_zp-0x84u));
 }
 free(source);
 if(mask!=0x3fu)return GK294_INCOMPLETE;
 Gk294SourceProof next={0};
 next.opcode=0u; /* Multiple opcodes: no single 'canonical writer' exists. */
 next.target=0u;
 next.mapped_prg_bank=0xffu; /* Claimed banks may differ per record. */
 next.count=(uint8_t)count;
 next.unique_target_mask=mask;
 next.source_candidates_only=1u;
 next.original_rom_sha256_verified=1u;
 *out=next;
 return GK294_OK;
}
