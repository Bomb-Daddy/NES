#ifndef GK105_OPENBUS_H
#define GK105_OPENBUS_H
#include "gk83_reset_trace.h"
#include <stdint.h>
typedef struct {
 Gk83Trace baseline;
 uint32_t frame_origin_dots,step_limit;
 uint16_t control_divergences,register_divergences;
 uint8_t trials;
} Gk105Summary;
/* Exhaust all 128 LOWER bits, compare against case0. These are constant
 * counterfactual lower status values, not simulated open-bus evolution.
 * A rejected source/argument leaves caller output byte-for-byte unchanged. */
Gk83Stop gk105_scan_lower7(const char *private_rom,uint32_t origin,
 uint32_t max_steps,Gk105Summary *out);
#endif
