#ifndef GK131_BOOT_GATE_H
#define GK131_BOOT_GATE_H
#include "gk83_reset_trace.h"
#include <stdint.h>
/* Authenticated original cartridge source / documented MesenCE configuration
 * contrast. A bounded diagnostic, NEVER an initialized game world or RNG.
 * The same ROM may take different paths under different initial RAM states.
 */
typedef enum {
 GK131_AUDIT_OK=0, GK131_AUDIT_BAD_ARGUMENT=1,
 GK131_AUDIT_ROM_REFUSED=2, GK131_AUDIT_SOURCE_DIVERGED=3
} Gk131AuditResult;
typedef struct {
 Gk83Trace unknown_power_on;
 Gk83Trace archived_mesen_allzeros;
 uint8_t stock_verified;
 uint8_t allzeros_is_explicit_archived_config;
 uint8_t first_spring_playable; /* ALWAYS zero: no inferred startup/RNG */
} Gk131BootAudit;
/* Failure is atomic: caller's output retains byte-identical contents.
 * Never pass raw cartridge bytes back to callers; filename is private input. */
Gk131AuditResult gk131_compare_boot_memory_profiles(const char *private_rom,
 Gk131BootAudit *out);
#endif
