#ifndef GK97_NMI_TRACE_H
#define GK97_NMI_TRACE_H
#include "gk83_reset_trace.h"
/* Exact private stock ROM, source-driven single NMI request at a known
 * wait loop. This is NOT a full NES PPU/NMI timing model. It is an
 * explicit controlled hypothesis, never a fresh Mesen observation. */
Gk83Stop gk97_trace_reset_one_hypothetical_nmi(const char *rom,
 const uint8_t *hypothetical_ppustatus,uint32_t status_count,
 uint32_t max_steps,Gk83Trace *out);
#endif
