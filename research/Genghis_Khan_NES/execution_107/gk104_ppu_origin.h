#ifndef GK104_PPU_ORIGIN_H
#define GK104_PPU_ORIGIN_H
#include "gk83_reset_trace.h"
#include <stdint.h>
/* Full-frame NTSC PPU reset-origin *hypothesis* (0..89341 dots).
 * Source ROM checksum mandatory. No original-Mesen parity inference. */
Gk83Stop gk104_trace_original_ppu_origin(const char *private_stock_rom,
 uint32_t origin_dots,uint32_t max_steps,Gk83Trace *out);
#endif
