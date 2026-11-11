#include "gk181_bank_seed_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { checks++; if(!(c)){fprintf(stderr,"GK191 failure line %d: %s\n",__LINE__,#c);return 1;} }while(0)
static unsigned checks;
static void put(uint8_t *rom,unsigned bank,unsigned pc,const uint8_t *b,size_t n){
 size_t pos=16u+(pc>=0xc000u?15u:bank)*16384u+(pc&0x3fffu);
 memcpy(rom+pos,b,n);
}
static void table(uint8_t *rom,unsigned selector,uint16_t dest){
 size_t base=16u+15u*16384u;
 rom[base+0x3788u+selector]=(uint8_t)dest;
 rom[base+0x3888u+selector]=(uint8_t)(dest>>8);
}
int main(int argc,char **argv){
 uint8_t *rom=calloc(1,GK180_ROM_BYTES);CHECK(rom!=NULL);
 memcpy(rom,"NES\032",4u);rom[4]=16u;rom[6]=0x12u;
 for(unsigned b=0;b<16;b++)memset(rom+16u+b*16384u,0x60,16384u);
 static const uint8_t reset[]={0x20,0x85,0xef,0x05,0x60};
 static const uint8_t rngcall[]={0x20,0x89,0xc2,0x60};
 static const uint8_t rts[]={0x60};
 put(rom,0,0xc100,reset,sizeof reset);put(rom,0,0xc110,rts,sizeof rts);
 put(rom,0,0xc120,rts,sizeof rts);put(rom,0,0xc200,rngcall,sizeof rngcall);
 size_t base=16u+15u*16384u;
 rom[base+0x3ffc]=0x00;rom[base+0x3ffd]=0xc1;
 rom[base+0x3ffa]=0x10;rom[base+0x3ffb]=0xc1;
 rom[base+0x3ffe]=0x20;rom[base+0x3fff]=0xc1;
 table(rom,5u,0xc200u);
 Gk181Map *m=calloc(1,sizeof *m);CHECK(m!=NULL);
 CHECK(gk181_scan_image(rom,GK180_ROM_BYTES,0u,m)==GK181_OK);
 CHECK(m->dispatcher_calls==1u&&m->dispatcher_table_edges==1u);
 CHECK(m->rng_call_count==1u&&m->rng_calls[0].pc==0xc200u);
 CHECK(m->rng_calls[0].bank==15u&&m->rng_calls[0].roots==1u);
 table(rom,5u,0x6060u);
 CHECK(gk181_scan_image(rom,GK180_ROM_BYTES,0u,m)==GK181_OK);
 CHECK(m->dispatcher_unmapped_targets==1u&&m->dispatcher_table_edges==0u);
 CHECK(m->rng_call_count==0u);
 table(rom,5u,0x8120u);put(rom,0,0x8120u,rngcall,sizeof rngcall);
 CHECK(gk181_scan_image(rom,GK180_ROM_BYTES,0u,m)==GK181_OK);
 CHECK(m->rng_call_count==1u&&m->rng_calls[0].bank==0u&&m->rng_calls[0].pc==0x8120u);
 CHECK(gk181_scan_image(rom,GK180_ROM_BYTES,1u,m)==GK181_OK);
 CHECK(m->dispatcher_table_edges==1u&&m->rng_call_count==0u);
 /* An unrelated, unreachable table entry and disassembly byte are not calls. */
 table(rom,6u,0xc200u);
 CHECK(gk181_scan_image(rom,GK180_ROM_BYTES,1u,m)==GK181_OK);
 CHECK(m->rng_call_count==0u);
 Gk181Map *snap=malloc(sizeof *m);CHECK(snap!=NULL);
 memcpy(snap,m,sizeof *m);
 CHECK(gk181_scan_image(rom,GK180_ROM_BYTES-1u,0u,m)==GK181_BAD_INPUT);
 CHECK(memcmp(snap,m,sizeof *m)==0);
 CHECK(gk181_scan_authenticated("/missing-auth-stock-rom",0u,m)==GK181_SOURCE);
 CHECK(memcmp(snap,m,sizeof *m)==0);
 if(argc==2){
  unsigned total=0,edges=0,missing=0;
  for(unsigned b=0;b<15u;b++){
   CHECK(gk181_scan_authenticated(argv[1],(uint8_t)b,m)==GK181_OK);
   CHECK(m->assumed_low_bank==b);
   total+=m->rng_call_count;edges+=m->dispatcher_table_edges;
   missing+=m->dispatcher_unmapped_targets;
   printf("VERIFIED_STATIC BANK=%u conditional-RNG-JSR=%u dispatcher-calls=%u resolved-table=%u outside=%u mapper-barriers=%u\n",
    b,(unsigned)m->rng_call_count,(unsigned)m->dispatcher_calls,
    (unsigned)m->dispatcher_table_edges,(unsigned)m->dispatcher_unmapped_targets,
    (unsigned)m->mapper_barriers);
  }
  printf("VERIFIED_STATIC total=%u conditional-JSR, table-edges=%u outside=%u (NOT a runtime trace)\n",total,edges,missing);
 }
 free(snap);free(m);free(rom);
 printf("GK191 %u assertions PASS\n",checks);
 return 0;
}
