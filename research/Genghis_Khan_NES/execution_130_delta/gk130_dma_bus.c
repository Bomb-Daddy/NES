#include "gk130_dma_bus.h"
#include "gk129_dma_source.h"
/* WorkRAM mirroring, battery PRG-RAM, and mapper-resolved cartridge PRG
 * are three DISJOINT source address regions. None may be substituted for
 * another. The 256 byte transfer commits only after every source byte has
 * passed preflight; partial OAM and partial timing advances are forbidden.
 */
Gk130DmaResult gk130_dma_cpu_bus(Gk128Oam *oam,
 const uint8_t ram[2048],const uint8_t known[2048],
 const Gk94PrgRam *save_ram,
 const uint8_t *authenticated_prg,size_t prg_bytes,
 uint8_t rom_authenticated,uint8_t control,uint8_t prg_bank,
 uint8_t control_known,uint8_t prg_known,
 uint8_t source_page,uint8_t alignment_known,uint8_t apu_phase,
 uint16_t *stalls){
 if(!oam||!stalls||alignment_known>1u||apu_phase>1u||
    control_known>1u||prg_known>1u||rom_authenticated>1u)
  return GK130_DMA_BAD;
 if(oam->address_known!=1u)return GK130_DMA_UNKNOWN_OAM;
 if(!alignment_known)return GK130_DMA_UNKNOWN_PHASE;
 if(source_page<0x20u){
  if(!ram||!known)return GK130_DMA_BAD;
  Gk129DmaResult r=gk129_dma_from_cpu_ram(oam,ram,known,
                  source_page,1u,apu_phase,stalls);
  if(r==GK129_DMA_OK)return GK130_DMA_OK;
  if(r==GK129_DMA_UNKNOWN_SOURCE)return GK130_DMA_UNKNOWN_SOURCE;
  return GK130_DMA_BAD;
 }
 if(source_page<0x60u)return GK130_DMA_IO_UNMODELLED;
 uint8_t source[256],fully_known[256];
 for(unsigned i=0u;i<256u;i++)fully_known[i]=1u;
 if(source_page<0x80u){
  if(!save_ram)return GK130_DMA_BAD;
  unsigned base=((unsigned)source_page-0x60u)*256u;
  for(unsigned i=0;i<256u;i++){
   if(save_ram->known[base+i]!=1u)return GK130_DMA_UNKNOWN_SOURCE;
   source[i]=save_ram->data[base+i];
  }
 }else{
  if(!rom_authenticated||!authenticated_prg||prg_bytes!=GK130_PRG_BYTES)
   return GK130_DMA_ROM_UNVERIFIED;
  /* The upper PRG window is fixed to bank 15 ONLY when MMC1 control
   * establishes mode 3. Unknown control cannot imply a fixed bank. */
  if(!control_known)return GK130_DMA_BANK_UNKNOWN;
  const unsigned addr=(unsigned)source_page<<8u;
  unsigned bank=15u;
  if(addr<0xc000u){
   if(!control_known||!prg_known)return GK130_DMA_BANK_UNKNOWN;
   unsigned mode=(control>>2u)&3u;
   if(mode==3u)bank=(unsigned)prg_bank&15u;
   else if(mode==2u)bank=0u;
   else bank=(unsigned)prg_bank&14u;
  }else if(control_known){
   unsigned mode=(control>>2u)&3u;
   if(mode==2u){if(!prg_known)return GK130_DMA_BANK_UNKNOWN;bank=prg_bank&15u;}
   else if(mode<2u){if(!prg_known)return GK130_DMA_BANK_UNKNOWN;bank=(prg_bank&14u)+1u;}
  }
  unsigned offset=bank*16384u+(addr&0x3fffu);
  for(unsigned i=0u;i<256u;i++)source[i]=authenticated_prg[offset+i];
 }
 return gk128_oam_dma(oam,source,fully_known,source_page,1u,apu_phase,stalls)
   ==GK128_OK?GK130_DMA_OK:GK130_DMA_BAD;
}
