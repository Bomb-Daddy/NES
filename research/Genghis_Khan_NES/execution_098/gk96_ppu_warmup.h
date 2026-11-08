#ifndef GK96_PPU_WARMUP_H
#define GK96_PPU_WARMUP_H
#include "gk92_ppustatus_hypothesis.h"
/* RESEARCH ONLY: inspect owner-authenticated original NES bytes while
 * injecting hypothetical PPUSTATUS reads.  This explicitly DOES NOT model
 * CPU cycle timing, NMI, interrupts, open-bus, or initial stat/season RNG.
 * It detects the first source-derived CPU loop which needs NES hardware to
 * progress, so later development cannot mislabel it a playable Spring.
 * All output fields are copied atomically only after authenticated tracing.
 */
typedef enum {GK96_INVALID=0,GK96_SOURCE_REFUSED=1,GK96_NEEDS_NMI=2,
              GK96_OTHER_GATE=3} Gk96Result;
typedef struct {
 Gk83Trace immediate_vblank;
 Gk83Trace delayed_vblank;
 uint8_t scenarios_checked,required_nmi;
} Gk96Warmup;
Gk96Result gk96_original_reset_warmup(const char *private_rom,Gk96Warmup *out);
#endif
