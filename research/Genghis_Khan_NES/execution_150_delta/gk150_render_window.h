#ifndef GK150_RENDER_WINDOW_H
#define GK150_RENDER_WINDOW_H
#include "gk83_reset_trace.h"
/* Every origin represents a hypothesis, NOT authenticated Mesen timing.
 * Results are original-ROM source-execution bounds only, not original gameplay. */
#define GK150_ORIGINS 35u
typedef enum {GK150_INVALID=0,GK150_SOURCE=1,GK150_BOUNDED=2} Gk150Status;
typedef struct {
 unsigned origins_checked,render_accepted,legacy_race_gates,other_gates;
 unsigned hardware_gates,unknown_ram_gates,opcode_gates,budget_gates;
 uint16_t first_render_pc,first_after_render_stop_pc;
 uint8_t first_render_mask,first_render_origin;
 uint64_t first_render_cycle,first_render_limit_dot,first_after_render_stop_cycles;
 uint8_t reached_spring; /* always false until an independent verified transition */
} Gk150Report;
/* Atomic on source refusal. No state alteration or synthetic first Spring. */
Gk150Status gk150_scan_render_window(const char *original_stock_rom,Gk150Report *out);
#endif
