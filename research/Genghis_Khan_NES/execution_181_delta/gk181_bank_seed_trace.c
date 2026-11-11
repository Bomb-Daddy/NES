#include "gk181_bank_seed_trace.h"
#include "gk142_fixed_cfg.h"
#include "gk1_source_identity.h"
#include "gk82_stock_cartridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BANK_SIZE 16384u
#define LOW_FIRST 0x8000u
#define FIXED_FIRST 0xc000u
#define ROOTS 3u
#define QUEUE_CAP 32768u
static const uint8_t expected_hash[32]={
 0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,
 0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,
 0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,
 0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3
};
static int header_ok(const uint8_t *p,size_t n){
 return p&&n==GK180_ROM_BYTES&&memcmp(p,"NES\032",4)==0&&
  p[4]==16u&&p[5]==0u&&p[6]==0x12u&&p[7]==0u;
}
static uint8_t at(const uint8_t *img,uint8_t bank,uint16_t pc){
 size_t offset=(pc>=FIXED_FIRST)?(15u*BANK_SIZE+(size_t)(pc-FIXED_FIRST)):
  ((size_t)bank*BANK_SIZE+(size_t)(pc-LOW_FIRST));
 return img[16u+offset];
}
static int is_branch(uint8_t op){
 return op==0x10||op==0x30||op==0x50||op==0x70||
        op==0x90||op==0xb0||op==0xd0||op==0xf0;
}
static uint8_t direct_kind(uint8_t op,uint16_t addr){
 if(addr<0x84u||addr>0x89u)return 0u;
 switch(op){
 case 0x84:case 0x85:case 0x86:case 0x8c:case 0x8d:case 0x8e:
  return GK180_STORE;
 case 0x06:case 0x26:case 0x46:case 0x66:case 0xc6:case 0xe6:
 case 0x0e:case 0x2e:case 0x4e:case 0x6e:case 0xce:case 0xee:
  return GK180_RMW;
 default:return 0u;
 }
}
static int unbounded_indexed_write(uint8_t op){
 switch(op){
 case 0x94:case 0x95:case 0x96:case 0x99:case 0x9d:
 case 0x81:case 0x91:case 0x16:case 0x36:case 0x56:
 case 0x76:case 0xd6:case 0xf6:case 0x1e:case 0x3e:
 case 0x5e:case 0x7e:case 0xde:case 0xfe:
  return 1;
 default:return 0;
 }
}
static int insert(Gk181Map *r,uint16_t pc,uint8_t bank,
 uint8_t address,uint8_t kind,uint8_t root){
 for(uint32_t i=0;i<r->source_sites;i++){
  Gk181Site *s=&r->sites[i];
  if(s->pc==pc&&s->bank==bank){
   if(s->seed_addr!=address||s->kind!=kind)return 0;
   s->roots|=(uint8_t)(1u<<root);return 1;
  }
 }
 if(r->source_sites==GK181_MAX_SITES)return 0;
 Gk181Site *s=&r->sites[r->source_sites++];
 s->pc=pc;s->bank=bank;s->seed_addr=address;s->kind=kind;
 s->roots=(uint8_t)(1u<<root);
 if(bank==15u)r->high_seed_candidates++;
 else r->low_seed_candidates++;
 return 1;
}
static void enqueue(uint16_t pc,uint16_t *q,size_t *tail,
 uint8_t seen[QUEUE_CAP],Gk181Map *r){
 if(pc<LOW_FIRST){r->external_edges++;return;}
 size_t idx=(size_t)pc-LOW_FIRST;
 if(!seen[idx]){seen[idx]=1u;q[(*tail)++]=pc;}
}
Gk181Result gk181_scan_image(const uint8_t *image,size_t n,
 uint8_t bank,Gk181Map *out){
 if(!out||!header_ok(image,n)||bank>=15u)return GK181_BAD_INPUT;
 Gk181Map *r=(Gk181Map*)calloc(1,sizeof *r);
 if(!r)return GK181_BAD_INPUT;
 r->assumed_low_bank=bank;
 r->vectors[0]=(uint16_t)(at(image,bank,0xfffc)|((uint16_t)at(image,bank,0xfffd)<<8));
 r->vectors[1]=(uint16_t)(at(image,bank,0xfffa)|((uint16_t)at(image,bank,0xfffb)<<8));
 r->vectors[2]=(uint16_t)(at(image,bank,0xfffe)|((uint16_t)at(image,bank,0xffff)<<8));
 Gk181Result rc=GK181_OK;
 for(uint8_t root=0;root<ROOTS&&rc==GK181_OK;root++){
  uint8_t seen[QUEUE_CAP]={0};uint16_t queue[QUEUE_CAP];size_t head=0,tail=0;
  enqueue(r->vectors[root],queue,&tail,seen,r);
  while(head<tail){
   uint16_t pc=queue[head++];
   uint8_t op=at(image,bank,pc),len=gk142_nmos_opcode_length(op);
   if(!len||(uint32_t)pc+len>65536u){r->unsupported_opcodes++;continue;}
   r->decoded[root]++;
   if(pc<FIXED_FIRST)r->low_bank_decoded[root]++;
   else r->fixed_bank_decoded[root]++;
   uint8_t arg0=0;uint16_t target=0;
   if(len>=2u)arg0=at(image,bank,(uint16_t)(pc+1u));
   target=arg0;
   if(len==3u)target|=(uint16_t)at(image,bank,(uint16_t)(pc+2u))<<8u;
   uint8_t kind=direct_kind(op,target);
   if(kind&&!insert(r,pc,(uint8_t)(pc<FIXED_FIRST?bank:15u),
      (uint8_t)target,kind,root)){rc=GK181_OVERFLOW;break;}
   if(unbounded_indexed_write(op))r->unresolved_indexed_writes++;
   /* MMC1 PRG bank register range $8000..FFFF. No dynamic mapper state
    * is inferred, so all absolute indexed stores conservatively stop;
    * indirect zero-page writes are unknown-address barriers too. */
   if((op==0x8d&&target>=0x8000u)||op==0x9du||op==0x99u||
       op==0x81u||op==0x91u){r->mapper_barriers++;continue;}
   if(op==0x00u||op==0x40u||op==0x60u)continue;
   if(op==0x6cu){r->indirect_jumps++;continue;}
   uint32_t next=(uint32_t)pc+len;
   if(op==0x20u||op==0x4cu){
    if(op==0x20u&&target==0xef85u){
     /* Known dispatcher inline selector after JSR. We do not resolve a
      * bank/mapping change through the dispatcher. */
     r->dispatcher_calls++;
     enqueue(target,queue,&tail,seen,r);
     if(next<65535u)enqueue((uint16_t)(next+1u),queue,&tail,seen,r);
    }else{
     enqueue(target,queue,&tail,seen,r);
     if(op==0x20u&&next<=65535u)enqueue((uint16_t)next,queue,&tail,seen,r);
    }
    continue;
   }
   if(is_branch(op)&&next<=65535u){
    uint16_t dest=(uint16_t)((int32_t)next+(int8_t)arg0);
    enqueue(dest,queue,&tail,seen,r);
   }
   if(next<=65535u)enqueue((uint16_t)next,queue,&tail,seen,r);
  }
 }
 if(rc==GK181_OK)*out=*r;
 free(r);return rc;
}
Gk181Result gk181_scan_authenticated(const char *path,uint8_t bank,Gk181Map *out){
 if(!path||!*path||bank>=15u||!out)return GK181_BAD_INPUT;
 Gk82RomBootstrap probe={0};
 if(gk82_probe_stock_rom(path,&probe)!=GK82_ROM_OK)return GK181_SOURCE;
 FILE *f=fopen(path,"rb");if(!f)return GK181_SOURCE;
 uint8_t *img=malloc(GK180_ROM_BYTES+1u);
 if(!img){fclose(f);return GK181_BAD_INPUT;}
 size_t n=fread(img,1,GK180_ROM_BYTES+1u,f);
 int bad=ferror(f),closed=fclose(f);Gk181Result rc=GK181_SOURCE;
 if(!bad&&!closed&&n==GK180_ROM_BYTES){
  uint8_t hash[32];gk1_sha256_bytes(img,n,hash);
  if(memcmp(hash,expected_hash,32u)==0)rc=gk181_scan_image(img,n,bank,out);
 }
 memset(img,0,GK180_ROM_BYTES+1u);free(img);return rc;
}
