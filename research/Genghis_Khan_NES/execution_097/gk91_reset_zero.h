#ifndef GK91_RESET_ZERO_H
#define GK91_RESET_ZERO_H
#include "gk83_reset_trace.h"
/* Real, pinned MesenCE settings-semantic trace: archived FRESH original
 * startup script sets RamPowerOnState=1 (AllZeros). This C11 CPU-only
 * source interpreter uses zero-initialized 2KiB CPU RAM, not guessed RNG.
 * Still stops on unsupported opcode/hardware/bank; never claims Mesen parity,
 * original hardware power-on defaults, or player-ready Spring. Invalid ROM
 * and argument refusal leave output unchanged. */
Gk83Stop gk91_trace_reset_mesen_allzeros(const char *original_private_rom,
 uint32_t max_steps,Gk83Trace *out);
#endif
