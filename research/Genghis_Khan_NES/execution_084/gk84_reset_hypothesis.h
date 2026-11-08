#ifndef GK84_RESET_HYPOTHESIS_H
#define GK84_RESET_HYPOTHESIS_H
#include "gk83_reset_trace.h"
/* Hypothetical counterfactual static source trace only.
 * Sets exactly ONE selected CPU power-on RAM address to the proposed byte;
 * every other uninitialized byte stays unknown and gates execution.
 * NO claim the byte was observed in Mesen, original hardware, or cartridge.
 * Accepts only CPU RAM $0000-$07FF; output untouched upon invalid argument
 * or absent/altered ROM. Use the original gk83 function for proven path. */
Gk83Stop gk84_trace_reset_single_ram_hypothesis(const char *original_private_rom,
 uint16_t ram_address,uint8_t hypothetical_value,uint32_t max_steps,
 Gk83Trace *out);
#endif
