#ifndef GK102_CONTROLLER_FRAMES_H
#define GK102_CONTROLLER_FRAMES_H
#include "gk83_reset_trace.h"
#include <stdint.h>
/* Research-only, counterfactual injection: first NMI at CPU wait $C460;
 * subsequent ones at CPU wait $C486, separated by >=64 SOURCE instructions.
 * Port1 sequence uses NES bits A,B,Select,Start,Up,Down,Left,Right.
 * Original ROM hash is mandatory, writes remain private, fail closed on any
 * unmodeled PPU/APU register. This does NOT schedule an NES frame or infer
 * stock game first-Spring RNG. Returns source stop gate and accounting. */
Gk83Stop gk102_trace_scripted_controller_frames(const char *private_stock_rom,
 const uint8_t *hypothetical_ppustatus_reads,uint32_t ppustatus_count,
 const uint8_t *port1_frames,uint32_t frame_count,
 uint32_t max_instructions,Gk83Trace *output);
#endif
