#include "gk128_oam.h"
#include <string.h>

/* Source-register state only. Rendering, DMC, sprite evaluation and the
 * power-on OAMADDR latch are not inferred. */
Gk128Result gk128_set_oam_address(Gk128Oam *oam,uint8_t address){
 if(!oam)return GK128_BAD;
 oam->address=address;
 oam->address_known=1u;
 return GK128_OK;
}
Gk128Result gk128_write_oam_data(Gk128Oam *oam,uint8_t value){
 if(!oam)return GK128_BAD;
 if(oam->address_known!=1u)return GK128_UNKNOWN_ADDRESS;
 oam->data[oam->address]=value;
 oam->known[oam->address]=1u;
 oam->address++;
 oam->direct_writes++;
 return GK128_OK;
}
/* The caller must supply the exact CPU-source page AND an independently
 * known APU get/put alignment. The power-on alignment is not determined by
 * CPU cycle parity; treating CPU parity as physical alignment is incorrect.
 * A future Mesen cycle capture can license the alignment bit. This routine
 * never creates one. The DMA is committed atomically only when every one
 * of the 256 source reads and the OAM start address is known. */
Gk128Result gk128_oam_dma(Gk128Oam *oam,const uint8_t data[256],
 const uint8_t known[256],uint8_t source_page,
 uint8_t apu_alignment_known,uint8_t alignment_required,uint16_t *stall_cycles){
 if(!oam||!data||!known||!stall_cycles||
    apu_alignment_known>1u||alignment_required>1u)return GK128_BAD;
 if(oam->address_known!=1u)return GK128_UNKNOWN_ADDRESS;
 if(!apu_alignment_known)return GK128_UNKNOWN_ALIGNMENT;
 for(unsigned i=0;i<256u;i++)if(known[i]!=1u)return GK128_UNKNOWN_SOURCE;
 Gk128Oam next=*oam;
 for(unsigned i=0;i<256u;i++){
  unsigned dest=(unsigned)(uint8_t)(next.address+(uint8_t)i);
  next.data[dest]=data[i];next.known[dest]=1u;
 }
 /* DMA traverses all 256 OAM locations, leaving OAMADDR unchanged. */
 next.dma_count++;
 next.last_dma_page=source_page;
 next.dma_stall_cycles+=(uint64_t)513u+(unsigned)alignment_required;
 *oam=next;
 *stall_cycles=(uint16_t)(513u+alignment_required);
 return GK128_OK;
}
