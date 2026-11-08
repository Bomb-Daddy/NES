#ifndef GK92_PPUSTATUS_HYPOTHESIS_H
#define GK92_PPUSTATUS_HYPOTHESIS_H
#include "gk83_reset_trace.h"
/* COUNTERFACTUAL ONLY: use verified original owner ROM, zero CPU RAM matching
 * the recorded fresh Mesen power-on setting, and caller-supplied PPUSTATUS
 * bytes; consume at most one per $2002 mirror read, then stop at the NEXT
 * unknown hardware/opcode/bank event. These values are NOT NES observations.
 * Hardware cycle timing, VBlank changes, PPU open bus and precise source
 * emulation are NOT implemented. 1..256 reads; bounded CPU instructions.
 * On argument/ROM failure, out is unchanged. */
Gk83Stop gk92_trace_reset_ppustatus_hypothesis(const char *original_private_rom,
 const uint8_t *hypothetical_status_reads,uint32_t read_count,
 uint32_t max_steps,Gk83Trace *out);
#endif
