#include "gk204_selector_store_inventory.h"
#include "gk202_irq_rng_dispatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK204_HEADER 16u
#define GK204_BANK_SIZE 16384u
#define GK204_BANKS 16u
static int direct_store(uint8_t op){
 return op==0x84u||op==0x85u||op==0x86u;
}
static int direct_rmw(uint8_t op){
 return op==0x06u||op==0x26u||op==0x46u||op==0x66u||
        op==0xc6u||op==0xe6u;
}
static int indexed_candidate(uint8_t op){
 return op==0x94u||op==0x95u||op==0x96u||op==0x16u||
        op==0x36u||op==0x56u||op==0x76u||op==0xd6u||op==0xf6u;
}
static int absolute_candidate(uint8_t op){
 return op==0x8cu||op==0x8du||op==0x8eu||op==0x0eu||
        op==0x2eu||op==0x4eu||op==0x6eu||op==0xceu||op==0xeeu;
}
Gk204Status gk204_inventory_bytes(const uint8_t *rom,size_t len,Gk204Inventory *out){
 if(!rom||!out)return GK204_ARGUMENT;
 Gk202Proof proof={0};
 if(gk202_analyze_authenticated_bytes(rom,len,&proof)!=GK202_OK)
  return GK204_SOURCE_REFUSED;
 Gk204Inventory r={0};
 r.verified_original_stock=1;
 r.source_only_not_runtime=1;
 r.rng_selector=proof.rng_slot;
 for(unsigned bank=0;bank<GK204_BANKS;bank++){
  const uint8_t *p=rom+GK204_HEADER+(size_t)bank*GK204_BANK_SIZE;
  for(size_t i=0;i+1u<GK204_BANK_SIZE;i++){
   if(p[i+1u]!=0x50u)continue;
   uint8_t kind=0;
   if(direct_store(p[i]))kind=GK204_RAW_DIRECT_STORE;
   else if(direct_rmw(p[i]))kind=GK204_RAW_DIRECT_RMW;
   else if(indexed_candidate(p[i]))kind=GK204_RAW_INDEXED_CANDIDATE;
   else if(absolute_candidate(p[i])&&i+2u<GK204_BANK_SIZE&&p[i+2u]==0u)
    kind=GK204_RAW_ABSOLUTE_CANDIDATE;
   if(!kind)continue;
   if(r.site_count>=GK204_MAX_CANDIDATES)return GK204_TOO_MANY;
   Gk204Site *s=&r.sites[r.site_count++];
   s->bank=(uint8_t)bank;
   s->bank_offset=(uint16_t)i;
   s->hypothetical_pc=(uint16_t)(i+(bank==15u?0xc000u:0x8000u));
   s->opcode=p[i];s->kind=kind;
   if(kind==GK204_RAW_DIRECT_STORE){
    r.raw_direct_store_count++;
    if(p[i]==0x85u&&i>=2u&&p[i-2u]==0xa9u){
     s->adjacent_lda_immediate=1;s->immediate_value=p[i-1u];
     if(s->immediate_value==proof.rng_slot)r.adjacent_lda_rng_store_count++;
    }
   }else if(kind==GK204_RAW_DIRECT_RMW)r.raw_direct_rmw_count++;
   else if(kind==GK204_RAW_INDEXED_CANDIDATE)r.raw_indexed_count++;
   else r.raw_absolute_count++;
  }
 }
 /* This bounds only exact raw byte patterns from the immutable original
  * cartridge. Do not assert the original CPU executed any candidate. */
 *out=r;return GK204_OK;
}
Gk204Status gk204_inventory_file(const char *name,Gk204Inventory *out){
 if(!name||!*name||!out)return GK204_ARGUMENT;
 FILE *f=fopen(name,"rb");if(!f)return GK204_SOURCE_REFUSED;
 uint8_t *data=(uint8_t*)malloc(GK202_STOCK_ROM_BYTES+1u);
 if(!data){fclose(f);return GK204_SOURCE_REFUSED;}
 size_t n=fread(data,1,GK202_STOCK_ROM_BYTES+1u,f);
 int err=ferror(f),closed=fclose(f);
 Gk204Status result=(!err&&!closed&&n==GK202_STOCK_ROM_BYTES)?
  gk204_inventory_bytes(data,n,out):GK204_SOURCE_REFUSED;
 memset(data,0,GK202_STOCK_ROM_BYTES+1u);free(data);
 return result;
}
