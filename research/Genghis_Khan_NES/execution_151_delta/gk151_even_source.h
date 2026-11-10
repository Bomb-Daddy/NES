#ifndef GK151_EVEN_SOURCE_H
#define GK151_EVEN_SOURCE_H
#include "gk83_reset_trace.h"
#define GK151_ORIGINS 35u
typedef enum { GK151_INVALID=0,GK151_SOURCE=1,GK151_BOUNDED=2 } Gk151Status;
typedef struct {
 unsigned origins,even_crossed,legacy_render_gated,ppu_race_gated,other_gated;
 uint16_t first_cross_stop_pc;
 uint64_t first_cross_stop_cycles;
 uint8_t source_advanced_after_legacy,first_cross_origin;
 uint8_t generated_spring; /* Never asserted by this research scanner. */
} Gk151Report;
/* Original-stock SHA required. Hypothesis is EVEN parity, never a
 * verified fact about original boot or real first Spring. Atomic output. */
Gk151Status gk151_scan_even_source(const char *rom,Gk151Report *out);
#endif
