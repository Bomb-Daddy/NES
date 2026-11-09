#ifndef GK128_OAM_H
#define GK128_OAM_H
#include <stdint.h>
/* NTSC NES OAM source-verified CPU write model. No original licensed bytes.
 * Not full PPU, sprites, background, DMC DMA, or NES first Spring.
 * All optional DMA effects require caller-supplied known CPU source bytes
 * AND independent APU get/put phase; no fabricated initial RAM or phase. */
typedef enum {GK128_OK=0,GK128_BAD=1,GK128_UNKNOWN_ADDRESS=2,
 GK128_UNKNOWN_ALIGNMENT=3,GK128_UNKNOWN_SOURCE=4} Gk128Result;
typedef struct {
 uint8_t data[256],known[256],address,address_known,last_dma_page;
 uint32_t direct_writes,dma_count;
 uint64_t dma_stall_cycles;
} Gk128Oam;
Gk128Result gk128_set_oam_address(Gk128Oam *oam,uint8_t address);
Gk128Result gk128_write_oam_data(Gk128Oam *oam,uint8_t value);
Gk128Result gk128_oam_dma(Gk128Oam *oam,const uint8_t data[256],
 const uint8_t known[256],uint8_t source_page,
 uint8_t apu_alignment_known,uint8_t alignment_required,uint16_t *stall_cycles);
#endif
