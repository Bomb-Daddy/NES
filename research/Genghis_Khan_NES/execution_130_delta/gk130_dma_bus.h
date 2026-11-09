#ifndef GK130_DMA_BUS_H
#define GK130_DMA_BUS_H
#include "gk128_oam.h"
#include "gk94_prgram.h"
#include <stddef.h>
#include <stdint.h>
/* Source-checked 2A03 OAM DMA from the CPU address bus. This is NOT a
 * determination of original Mesen power-on APU phase or PPU rendering.
 * Original cartridge bytes must already be authenticated by the caller;
 * never pass a claimed file name or a screenshot as authentication.
 * $2000-$5FFF requires peripheral reads not represented here and fails closed.
 */
#define GK130_PRG_BYTES 262144u
typedef enum {
 GK130_DMA_OK=0, GK130_DMA_BAD=1, GK130_DMA_UNKNOWN_OAM=2,
 GK130_DMA_UNKNOWN_PHASE=3, GK130_DMA_UNKNOWN_SOURCE=4,
 GK130_DMA_IO_UNMODELLED=5, GK130_DMA_BANK_UNKNOWN=6,
 GK130_DMA_ROM_UNVERIFIED=7
} Gk130DmaResult;
Gk130DmaResult gk130_dma_cpu_bus(Gk128Oam *oam,
 const uint8_t ram[2048],const uint8_t known[2048],
 const Gk94PrgRam *save_ram,
 const uint8_t *authenticated_prg,size_t prg_bytes,
 uint8_t rom_authenticated,uint8_t control,uint8_t prg_bank,
 uint8_t control_known,uint8_t prg_known,
 uint8_t source_page,uint8_t alignment_known,uint8_t apu_phase,
 uint16_t *stalls);
#endif
