#ifndef GK174_SETUP_BOUNDARY_H
#define GK174_SETUP_BOUNDARY_H
#include <stdint.h>
#include "gk83_reset_trace.h"
/* Two ORIGINAL-SOURCE CPU-only traces with different, explicit assumptions.
 * STRICT: unspecified power-on work RAM -> first unknown read.
 * MESEN_ALL_ZEROS: owner archived fresh-reset MesenCE setting
 * RamPowerOnState=1, independently documented in GK91. Neither trace models
 * actual Mesen frames, PPUSTATUS, controller/NMI, game setup or turn actions.
 * Both outputs are transactional: no mutation on any invalid provenance.
 */
typedef struct {
 Gk83Stop strict_gate, mesen_zero_gate;
 Gk83Trace strict, mesen_zero;
 uint32_t completed_instruction_delta;
 uint64_t completed_cycle_delta;
} Gk174SetupBoundary;
/* Return 1 only if both traces terminate at the source-proven first gates
 * with the authenticated exact original ROM. Valid output never contains
 * original ROM bytes; it is a bounded source/assumption comparison.
 * Failures leave `out` byte-for-byte unchanged. */
int gk174_trace_setup_boundary(const char *private_original_rom,
 uint32_t instruction_budget,Gk174SetupBoundary *out);
#endif
