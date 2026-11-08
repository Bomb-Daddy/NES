#ifndef GK98_CONTROLLER_TRACE_H
#define GK98_CONTROLLER_TRACE_H
#include "gk83_reset_trace.h"
/* Controlled, explicitly HYPOTHETICAL input sequence. The full original
 * private cartridge hash is verified before any trace. One synthetic NMI at
 * $C460 (already isolated in GK97), caller-supplied $2002 return series,
 * and one neutral or NES-button-mask $4016 controller. Upper read bits must
 * be provided explicitly and are not derived from open-bus/PPU timing.
 * This does NOT independently complete the original NES boot or Spring. */
Gk83Stop gk98_trace_reset_controller_hypothesis(const char *private_stock_rom,
 const uint8_t *hypothetical_ppustatus,uint32_t ppustatus_count,
 uint8_t port1_buttons,uint8_t port2_buttons,uint8_t hypothetical_upper_bus_bits,
 uint32_t max_steps,Gk83Trace *result);
#endif
