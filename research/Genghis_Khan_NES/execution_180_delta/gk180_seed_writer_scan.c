#include "gk180_seed_writer_scan.h"
#include "gk142_fixed_cfg.h"
#include "gk1_source_identity.h"
#include "gk82_stock_cartridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GK180_BASE (16u+15u*16384u)
#define GK180_MIN_PC 0xC000u
#define GK180_MAX_PC 0xFFFFu
#define GK180_FIXED 16384u
static const uint8_t original_sha256[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3
};
static int header(const uint8_t *image,size_t n){
 return n==GK180_ROM_BYTES&&image[0]=='N'&&image[1]=='E'&&
 image[2]=='S'&&image[3]==0x1au&&image[4]==16u&&image[5]==0u&&
 image[6]==0x12u&&image[7]==0u;
}
static uint8_t write_kind(uint8_t op,uint16_t address){
 if(address<0x84u||address>0x89u)return 0;
 switch(op){
 case 0x84:case 0x85:case 0x86:case 0x8c:case 0x8d:case 0x8e:
  return GK180_STORE;
 case 0x06:case 0x26:case 0x46:case 0x66:case 0xc6:case 0xe6:
 case 0x0e:case 0x2e:case 0x4e:case 0x6e:case 0xce:case 0xee:
  return GK180_RMW;
 default:return 0;
 }
}
static uint16_t le16(const uint8_t *p){return (uint16_t)(p[0]|((uint16_t)p[1]<<8u));}
static int add_site(Gk180WriterMap *out,uint16_t pc,uint16_t target,uint8_t kind,uint8_t root){
 for(uint32_t k=0;k<out->source_sites;k++){
  Gk180Site *s=&out->sites[k];
  if(s->pc==pc){
   if(s->address!=target||s->kind!=kind)return 0;
   s->roots|=(uint8_t)(1u<<root);return 1;
  }
 }
 if(out->source_sites>=GK180_MAX_SITES)return 0;
 Gk180Site *s=&out->sites[out->source_sites++];
 s->pc=pc;s->address=(uint8_t)target;s->kind=kind;
 s->roots=(uint8_t)(1u<<root);
 s->in_rng=(uint8_t)(pc>=0xc289u&&pc<=0xc2ebu);
 return 1;
}
static void enqueue(uint16_t pc,uint16_t *queue,size_t *tail,
 uint8_t seen[GK180_FIXED],Gk180WriterMap *r){
 if(pc<GK180_MIN_PC){r->unmapped_edges++;return;}
 size_t pos=(size_t)pc-GK180_MIN_PC;
 if(!seen[pos]){seen[pos]=1u;queue[(*tail)++]=pc;}
}
Gk180Status gk180_scan_image(const uint8_t *image,size_t n,Gk180WriterMap *out){
 if(!image||!out||!header(image,n))return GK180_BAD_INPUT;
 Gk180WriterMap *r=(Gk180WriterMap*)calloc(1u,sizeof *r);
 if(!r)return GK180_BAD_INPUT;
 const uint8_t *rom=image+GK180_BASE;
 r->vectors[0]=le16(rom+0x3ffcu); /* RESET */
 r->vectors[1]=le16(rom+0x3ffau); /* NMI */
 r->vectors[2]=le16(rom+0x3ffeu); /* IRQ */
 Gk180Status status=GK180_OK;
 for(uint8_t root=0;root<3u&&status==GK180_OK;root++){
  uint8_t seen[GK180_FIXED]={0};uint16_t queue[GK180_FIXED];
  size_t head=0,tail=0;
  enqueue(r->vectors[root],queue,&tail,seen,r);
  while(head<tail){
   uint16_t pc=queue[head++];size_t at=(size_t)pc-GK180_MIN_PC;
   uint8_t op=rom[at],len=gk142_nmos_opcode_length(op);
   if(!len||at+len>GK180_FIXED){r->unsupported_opcodes++;continue;}
   r->decoded[root]++;
   uint16_t target=0u,fall=(uint16_t)(pc+len);
   if(len==2u)target=rom[at+1u];
   else if(len==3u)target=le16(rom+at+1u);
   /* All accepted accesses are direct zero-page OR absolute-unindexed.
    * Indexed and indirect operations can alter the seed but cannot be
    * attributed without registers; this scan never declares completeness. */
   uint8_t kind=write_kind(op,target);
   if(kind&&!add_site(r,pc,target,kind,root)){status=GK180_OVERFLOW;break;}
   if(op==0x94u||op==0x95u||op==0x96u||op==0x99u||
      op==0x9du||op==0x81u||op==0x91u)r->unverified_indexed_writes++;
   if(op==0x00u||op==0x40u||op==0x60u)continue;
   if(op==0x6cu){r->unknown_indirect_jumps++;continue;}
   if(op==0x20u||op==0x4cu){
    if(target==0xc289u)r->rng_entry_edges++;
    if(op==0x20u&&target==0xef85u){
     /* This game's fixed-bank dispatcher uses one inline selector byte.
      * No low-bank dispatch target is inferred from this site. */
     if(at+3u>=GK180_FIXED){r->unsupported_opcodes++;continue;}
     r->dispatch_candidates++;
     enqueue(target,queue,&tail,seen,r);
     if(pc<=0xfffbu)enqueue((uint16_t)(pc+4u),queue,&tail,seen,r);
     continue;
    }
    enqueue(target,queue,&tail,seen,r);
    if(op==0x20u&&pc<=0xfffcu)enqueue(fall,queue,&tail,seen,r);
    continue;
   }
   if(op==0x10u||op==0x30u||op==0x50u||op==0x70u||
      op==0x90u||op==0xb0u||op==0xd0u||op==0xf0u){
    int32_t dest=(int32_t)((uint32_t)pc+len)+(int8_t)rom[at+1u];
    if(dest>=0&&dest<=0xffff)enqueue((uint16_t)dest,queue,&tail,seen,r);
    else r->unmapped_edges++;
   }
   if((uint32_t)pc+len<=0xffffu)enqueue(fall,queue,&tail,seen,r);
  }
 }
 if(status==GK180_OK){
  for(uint32_t k=0;k<r->source_sites;k++){
   if(r->sites[k].in_rng)r->internal_rng_sites++;
   else r->external_seed_site_candidates++;
  }
  *out=*r;
 }
 free(r);return status;
}
Gk180Status gk180_scan_authenticated(const char *path,Gk180WriterMap *out){
 if(!path||!*path||!out)return GK180_BAD_INPUT;
 Gk82RomBootstrap header_result={0};
 if(gk82_probe_stock_rom(path,&header_result)!=GK82_ROM_OK)return GK180_SOURCE;
 FILE *f=fopen(path,"rb");if(!f)return GK180_SOURCE;
 uint8_t *bytes=(uint8_t*)malloc(GK180_ROM_BYTES+1u);
 if(!bytes){fclose(f);return GK180_BAD_INPUT;}
 size_t n=fread(bytes,1u,GK180_ROM_BYTES+1u,f);
 int bad=ferror(f),closed=fclose(f);
 Gk180Status result=GK180_SOURCE;
 if(!bad&&!closed&&n==GK180_ROM_BYTES){
  uint8_t digest[32];gk1_sha256_bytes(bytes,n,digest);
  if(memcmp(digest,original_sha256,sizeof digest)==0)
   result=gk180_scan_image(bytes,n,out);
 }
 memset(bytes,0,GK180_ROM_BYTES+1u);free(bytes);
 return result;
}
