#ifndef GK103_PPU_SCHEDULE_H
#define GK103_PPU_SCHEDULE_H
#include "gk83_reset_trace.h"
/* Source-bound reset trace with an explicit ASSUMED NTSC PPU phase. This is
 * NOT genuine Mesen playback or authentic RNG/first Spring parity. */
Gk83Stop gk103_trace_original_ppu_schedule(const char *private_stock_rom,
 uint8_t phase_ppu_dots,uint32_t max_steps,Gk83Trace *out);
#endif
