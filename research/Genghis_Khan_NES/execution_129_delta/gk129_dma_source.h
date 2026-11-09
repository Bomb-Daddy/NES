#ifndef GK129_DMA_SOURCE_H
#define GK129_DMA_SOURCE_H
#include <stdint.h>
#include "gk128_oam.h"
/* This is a source-validated $4014 CPU RAM read path for the INHERITED native
 * Genghis NES interpreter, not an original stock capture. The 6502 $0000-
 * $1FFF RAM window mirrors 2KB. Source pages $20-$FF may include PPU/APU,
 * cartridge-mapped RAM, or PRG and are deliberately NOT presumed readable.
 * The APU get/put alignment bit MUST come from independent evidence.
 * Success is atomic and returns exact 513/514 DMA CPU-stall cycles. */
typedef enum { GK129_DMA_OK=0, GK129_DMA_BAD=1,
 GK129_DMA_UNMODELLED_PAGE=2, GK129_DMA_UNKNOWN_SOURCE=3,
 GK129_DMA_UNKNOWN_OAM=4, GK129_DMA_UNKNOWN_PHASE=5 } Gk129DmaResult;
Gk129DmaResult gk129_dma_from_cpu_ram(Gk128Oam *oam,
 const uint8_t ram[2048],const uint8_t known[2048],
 uint8_t source_page,uint8_t apu_alignment_is_known,uint8_t apu_phase,
 uint16_t *cpu_stall_cycles);
#endif
