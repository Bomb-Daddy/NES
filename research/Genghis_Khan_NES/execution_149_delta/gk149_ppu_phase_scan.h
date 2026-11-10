#ifndef GK149_PPU_PHASE_SCAN_H
#define GK149_PPU_PHASE_SCAN_H
#include <stdint.h>
/* Source-authenticated 6502/Ppu reset research. 35 EXPLICIT phase-origin
 * hypotheses; none is an observed original Mesen clock origin. No ROM bytes,
 * initial 14/27-country records or licensed assets are shipped or generated. */
#define GK149_PHASE_COUNT 35u
typedef enum {
 GK149_INVALID=0, GK149_SOURCE_REFUSED=1,
 GK149_BOUNDED=2, GK149_UNEXPECTED_GATE=3
} Gk149Status;
typedef struct {
 uint8_t origins_checked,ppu_race_stops,render_enable_stops,other_stops;
 uint8_t first_render_origin,first_render_mask;
 uint16_t first_render_pc,first_race_pc;
 uint32_t vblank_set_reads,ppu_status_reads;
 uint64_t first_render_cycles;
 uint8_t spring_created; /* hard zero: this is a source trace, not game init */
} Gk149PhaseScan;
/* Only authenticated original USA ROM hash is accepted. A rejected source
 * leaves output entirely unchanged. Does not mutate campaign state. */
Gk149Status gk149_scan_original_reset_phases(const char *private_rom,
                                             Gk149PhaseScan *out);
#endif
