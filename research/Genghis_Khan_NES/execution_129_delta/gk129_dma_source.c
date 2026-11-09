#include "gk129_dma_source.h"
#include <string.h>
Gk129DmaResult gk129_dma_from_cpu_ram(Gk128Oam *oam,
 const uint8_t ram[2048],const uint8_t known[2048],
 uint8_t page,uint8_t alignment_known,uint8_t phase,uint16_t *stalls){
 if(!oam||!ram||!known||!stalls||alignment_known>1u||phase>1u)
  return GK129_DMA_BAD;
 /* Reject unknown peripheral/mapper source pages before touching OAM. */
 if(page>=0x20u)return GK129_DMA_UNMODELLED_PAGE;
 if(oam->address_known!=1u)return GK129_DMA_UNKNOWN_OAM;
 if(!alignment_known)return GK129_DMA_UNKNOWN_PHASE;
 uint8_t bytes[256],known_page[256];
 unsigned base=((unsigned)page<<8u)&0x7ffu;
 for(unsigned i=0;i<256;i++){
  unsigned at=(base+i)&0x7ffu;
  if(known[at]!=1u)return GK129_DMA_UNKNOWN_SOURCE;
  bytes[i]=ram[at];known_page[i]=1u;
 }
 Gk128Result r=gk128_oam_dma(oam,bytes,known_page,page,1u,phase,stalls);
 return r==GK128_OK?GK129_DMA_OK:GK129_DMA_BAD;
}
