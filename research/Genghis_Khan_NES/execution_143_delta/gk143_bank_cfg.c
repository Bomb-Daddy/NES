#include "gk143_bank_cfg.h"
#include "gk142_fixed_cfg.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BANK_SZ 16384u
#define IMAGE_OFF 16u
#define PRG_FIXED 15u
#define QUEUE_CAP 32768u
static const uint8_t stock_sha[32]={0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3};
static uint8_t byte_at(const uint8_t *img,uint8_t bank,uint16_t address){
 uint32_t off=(address>=0xc000u)?(IMAGE_OFF+PRG_FIXED*BANK_SZ+(address-0xc000u)):(IMAGE_OFF+bank*BANK_SZ+(address-0x8000u));
 return img[off];
}
static int in_prg(uint16_t pc){return pc>=0x8000u;}
static void push(uint16_t pc,uint16_t *queue,size_t *tail,uint8_t *seen,Gk143BankCfg *r){
 if(!in_prg(pc)){r->outside_prg_targets++;return;}
 unsigned i=(unsigned)(pc-0x8000u);
 if(seen[i])return;
 if(*tail==QUEUE_CAP){r->queue_overflow++;return;}
 seen[i]=1;queue[(*tail)++]=pc;
}
Gk141Result gk143_scan_bank_hypothesis(const uint8_t *img,size_t bytes,uint8_t bank,Gk143BankCfg *out){
 if(!img||!out||bank>=PRG_FIXED)return GK141_BAD_INPUT;
 Gk141DispatchMap validated;
 if(gk141_scan_image(img,bytes,&validated)!=GK141_OK)return GK141_BAD_INPUT;
 Gk143BankCfg r={0};r.assumed_boot_bank=bank;
 r.vector_nmi=(uint16_t)(byte_at(img,bank,0xfffa)|((uint16_t)byte_at(img,bank,0xfffb)<<8));
 r.vector_reset=(uint16_t)(byte_at(img,bank,0xfffc)|((uint16_t)byte_at(img,bank,0xfffd)<<8));
 r.vector_irq=(uint16_t)(byte_at(img,bank,0xfffe)|((uint16_t)byte_at(img,bank,0xffff)<<8));
 uint16_t *queue=malloc(QUEUE_CAP*sizeof(*queue));
 uint8_t *seen=calloc(QUEUE_CAP,1);
 if(!queue||!seen){free(queue);free(seen);return GK141_BAD_INPUT;}
 size_t head=0,tail=0;
 const uint16_t vectors[3]={r.vector_reset,r.vector_nmi,r.vector_irq};
 for(unsigned i=0;i<3u;i++){if(!in_prg(vectors[i]))r.unmapped_entrypoints++;else push(vectors[i],queue,&tail,seen,&r);}
 while(head<tail){
  uint16_t pc=queue[head++];uint8_t op=byte_at(img,bank,pc),len=gk142_nmos_opcode_length(op);
  uint32_t end=(uint32_t)pc+len;
  if(!len){r.unsupported_opcodes++;continue;}
  /* $FFFF is a hard PC wrap, not a mapped sequential ROM address. */
  if(end>65536u){r.truncated_instructions++;continue;}
  r.decoded_instructions++;r.scanned_bytes+=len;
  if(pc<0xc000u)r.decoded_low++;else r.decoded_fixed++;
  uint8_t a=0,b=0;
  if(len>=2u)a=byte_at(img,bank,(uint16_t)(pc+1u));
  if(len>=3u)b=byte_at(img,bank,(uint16_t)(pc+2u));
  uint16_t fall=(uint16_t)end;
  uint16_t target=(uint16_t)(a|((uint16_t)b<<8));
  if(op==0x00u||op==0x40u||op==0x60u)continue;
  /* Stop all paths at known possible MMC1 bank-register writes. Otherwise
   * continuing would assign source bytes to a bank that may have changed.
   * Indexed absolute writes near the mapping boundary are conservative. */
  if((op==0x8du||op==0x9du||op==0x99u) && target >= (op==0x8du?0x8000u:0x7f00u)){
   r.bank_write_barriers++;continue;
  }
  if(op==0x6cu){r.unknown_indirect_jumps++;continue;}
  if(op==0x20u||op==0x4cu){
   if(op==0x20u&&target==0xef85u){
    /* The dispatcher consumes an inline selector byte at JSR+3. */
    if(end>=65536u){r.truncated_instructions++;continue;}
    uint8_t sel=byte_at(img,bank,(uint16_t)end);
    r.dispatch_candidates++;r.by_selector[sel]++;
    push(0xef85u,queue,&tail,seen,&r);
    if(end+1u<=65535u)push((uint16_t)(end+1u),queue,&tail,seen,&r);
   }else{
    push(target,queue,&tail,seen,&r);
    if(op==0x20u&&end<=65535u)push(fall,queue,&tail,seen,&r);
   }
   continue;
  }
  if(op==0x10u||op==0x30u||op==0x50u||op==0x70u||op==0x90u||op==0xb0u||op==0xd0u||op==0xf0u){
   push((uint16_t)((int32_t)end+(int8_t)a),queue,&tail,seen,&r);
  }
  if(end<=65535u)push(fall,queue,&tail,seen,&r);
 }
 free(queue);free(seen);
 if(r.queue_overflow)return GK141_BAD_INPUT;
 *out=r;return GK141_OK;
}
Gk141Result gk143_scan_authenticated(const char *path,uint8_t bank,Gk143BankCfg *out){
 if(!path||!out||bank>=PRG_FIXED)return GK141_BAD_INPUT;
 FILE *f=fopen(path,"rb");if(!f)return GK141_SOURCE_UNVERIFIED;
 uint8_t *img=malloc(GK141_IMAGE_SIZE+1u);
 if(!img){fclose(f);return GK141_BAD_INPUT;}
 size_t n=fread(img,1,GK141_IMAGE_SIZE+1u,f);
 int error=ferror(f),close_error=fclose(f);
 Gk141Result result=GK141_SOURCE_UNVERIFIED;
 if(!error&&!close_error&&n==GK141_IMAGE_SIZE){
  uint8_t hash[32];gk1_sha256_bytes(img,n,hash);
  if(memcmp(hash,stock_sha,sizeof hash)==0)result=gk143_scan_bank_hypothesis(img,n,bank,out);
 }
 free(img);return result;
}
