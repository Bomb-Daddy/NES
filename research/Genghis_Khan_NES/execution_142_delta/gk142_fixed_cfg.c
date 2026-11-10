#include "gk142_fixed_cfg.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BASE (16u + 15u * 16384u)
#define OFF(x) ((uint16_t)((x)-0xc000u))
/* The table contains only documented NMOS 6502 opcodes. Invalid and
 * undocumented operations terminate exploration rather than guessing. */
static const uint8_t lengths[256]={
 [0x00]=1,[0x01]=2,[0x05]=2,[0x06]=2,[0x08]=1,[0x09]=2,[0x0a]=1,[0x0d]=3,[0x0e]=3,
 [0x10]=2,[0x11]=2,[0x15]=2,[0x16]=2,[0x18]=1,[0x19]=3,[0x1d]=3,[0x1e]=3,
 [0x20]=3,[0x21]=2,[0x24]=2,[0x25]=2,[0x26]=2,[0x28]=1,[0x29]=2,[0x2a]=1,[0x2c]=3,[0x2d]=3,[0x2e]=3,
 [0x30]=2,[0x31]=2,[0x35]=2,[0x36]=2,[0x38]=1,[0x39]=3,[0x3d]=3,[0x3e]=3,
 [0x40]=1,[0x41]=2,[0x45]=2,[0x46]=2,[0x48]=1,[0x49]=2,[0x4a]=1,[0x4c]=3,[0x4d]=3,[0x4e]=3,
 [0x50]=2,[0x51]=2,[0x55]=2,[0x56]=2,[0x58]=1,[0x59]=3,[0x5d]=3,[0x5e]=3,
 [0x60]=1,[0x61]=2,[0x65]=2,[0x66]=2,[0x68]=1,[0x69]=2,[0x6a]=1,[0x6c]=3,[0x6d]=3,[0x6e]=3,
 [0x70]=2,[0x71]=2,[0x75]=2,[0x76]=2,[0x78]=1,[0x79]=3,[0x7d]=3,[0x7e]=3,
 [0x81]=2,[0x84]=2,[0x85]=2,[0x86]=2,[0x88]=1,[0x8a]=1,[0x8c]=3,[0x8d]=3,[0x8e]=3,
 [0x90]=2,[0x91]=2,[0x94]=2,[0x95]=2,[0x96]=2,[0x98]=1,[0x99]=3,[0x9a]=1,[0x9d]=3,
 [0xa0]=2,[0xa1]=2,[0xa2]=2,[0xa4]=2,[0xa5]=2,[0xa6]=2,[0xa8]=1,[0xa9]=2,[0xaa]=1,[0xac]=3,[0xad]=3,[0xae]=3,
 [0xb0]=2,[0xb1]=2,[0xb4]=2,[0xb5]=2,[0xb6]=2,[0xb8]=1,[0xb9]=3,[0xba]=1,[0xbc]=3,[0xbd]=3,[0xbe]=3,
 [0xc0]=2,[0xc1]=2,[0xc4]=2,[0xc5]=2,[0xc6]=2,[0xc8]=1,[0xc9]=2,[0xca]=1,[0xcc]=3,[0xcd]=3,[0xce]=3,
 [0xd0]=2,[0xd1]=2,[0xd5]=2,[0xd6]=2,[0xd8]=1,[0xd9]=3,[0xdd]=3,[0xde]=3,
 [0xe0]=2,[0xe1]=2,[0xe4]=2,[0xe5]=2,[0xe6]=2,[0xe8]=1,[0xe9]=2,[0xea]=1,[0xec]=3,[0xed]=3,[0xee]=3,
 [0xf0]=2,[0xf1]=2,[0xf5]=2,[0xf6]=2,[0xf8]=1,[0xf9]=3,[0xfd]=3,[0xfe]=3
};
/* No stack recursion, maximum 16KB enqueues; each address is enqueued once. */
static void enqueue(uint16_t pc,uint16_t *queue,size_t *tail,uint8_t *seen,Gk142FixedCfg *s){
 if(pc<0xc000u){s->switchable_targets++;return;}
 uint16_t i=OFF(pc);
 if(!seen[i]){seen[i]=1;queue[(*tail)++]=pc;}
}
Gk141Result gk142_scan_fixed_cfg(const uint8_t *img,size_t bytes,Gk142FixedCfg *out){
 if(!img||!out)return GK141_BAD_INPUT;
 Gk141DispatchMap sanity;
 if(gk141_scan_image(img,bytes,&sanity)!=GK141_OK)return GK141_BAD_INPUT;
 Gk142FixedCfg next={0};
 const uint8_t *p=img+BASE;
 const uint16_t *unused=NULL;(void)unused;
 next.vector_nmi=(uint16_t)(p[0x3ffa]|(p[0x3ffb]<<8u));
 next.vector_reset=(uint16_t)(p[0x3ffc]|(p[0x3ffd]<<8u));
 next.vector_irq=(uint16_t)(p[0x3ffe]|(p[0x3fff]<<8u));
 uint16_t queue[16384];uint8_t seen[16384]={0};size_t head=0,tail=0;
 uint16_t vectors[3]={next.vector_nmi,next.vector_reset,next.vector_irq};
 for(int i=0;i<3;i++){
  if(vectors[i]>=0xc000u){next.fixed_entrypoints++;enqueue(vectors[i],queue,&tail,seen,&next);}
  else next.dynamic_entries++;
 }
 while(head<tail){
  const uint16_t pc=queue[head++];const uint16_t pos=OFF(pc);
  const uint8_t op=p[pos],len=lengths[op];
  if(!len){next.unsupported_opcodes++;continue;}
  if(pos+len>16384u){next.truncated_instructions++;continue;}
  next.decoded_instructions++;next.examined_fixed_bytes+=len;
  if(op==0u||op==0x40u||op==0x60u)continue; /* BRK, RTI, RTS */
  if(op==0x6cu){next.indirect_jumps++;continue;}
  uint16_t fall=(uint16_t)(pc+len);
  if(op==0x20u ||op==0x4cu){
   uint16_t dest=(uint16_t)(p[pos+1u]|(p[pos+2u]<<8u));
   if(op==0x20u&&dest==0xef85u){
    if(pos+3u>=16384u){next.truncated_instructions++;continue;}
    const uint8_t selector=p[pos+3u];
    next.reachable_dispatch_calls++;next.reachable_by_selector[selector]++;
    enqueue(0xef85u,queue,&tail,seen,&next);
    if(pc<=0xfffbu)enqueue((uint16_t)(pc+4u),queue,&tail,seen,&next);
    continue;
   }
   enqueue(dest,queue,&tail,seen,&next);
   if(op==0x20u&&pc<=0xfffcu)enqueue(fall,queue,&tail,seen,&next);
   continue;
  }
  if(op==0x10u||op==0x30u||op==0x50u||op==0x70u||op==0x90u||op==0xb0u||op==0xd0u||op==0xf0u){
   const int32_t tgt=(int32_t)(uint32_t)fall+(int8_t)p[pos+1u];
   enqueue((uint16_t)tgt,queue,&tail,seen,&next);
  }
  if(pc <= (uint16_t)(0xffffu-len))enqueue(fall,queue,&tail,seen,&next);
 }
 *out=next;return GK141_OK;
}
static const uint8_t stock_hash[32]={0xad,0xa7,0x06,0x6d,0x9e,0x71,0xa8,0x13,0x10,0xff,0xf7,0x8b,0xc7,0x03,0x71,0x26,0x79,0x80,0x18,0xa6,0x9d,0xc2,0xb9,0x23,0x82,0xc3,0xcb,0xdc,0xd9,0xb2,0x43,0xb3};
Gk141Result gk142_scan_fixed_cfg_authenticated(const char *path,Gk142FixedCfg *out){
 if(!path||!out)return GK141_BAD_INPUT;
 FILE *f=fopen(path,"rb");if(!f)return GK141_SOURCE_UNVERIFIED;
 uint8_t *buf=(uint8_t*)malloc(GK141_IMAGE_SIZE+1u);
 if(!buf){fclose(f);return GK141_BAD_INPUT;}
 size_t n=fread(buf,1,GK141_IMAGE_SIZE+1u,f);
 int bad=ferror(f),closed=fclose(f);Gk141Result st=GK141_SOURCE_UNVERIFIED;
 if(!bad&&!closed&&n==GK141_IMAGE_SIZE){
  uint8_t sha[32];gk1_sha256_bytes(buf,n,sha);
  if(memcmp(sha,stock_hash,32u)==0)st=gk142_scan_fixed_cfg(buf,n,out);
 }
 free(buf);return st;
}
