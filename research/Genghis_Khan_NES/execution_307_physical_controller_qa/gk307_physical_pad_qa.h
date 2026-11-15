#ifndef GK307_PHYSICAL_PAD_QA_H
#define GK307_PHYSICAL_PAD_QA_H
#include <stdint.h>
#include "gk99_xinput_bridge.h"
/* A real-device-only Windows diagnostic in the existing C11 WinMain shell.
 * These steps are transport observations, not an original NES test, gameplay
 * progress, or proof that a human played a campaign. Hardware NOT RUN in CI.
 * The state machine has no clocks or synthetic-gameplay grant. */
#define GK307_PHYSICAL_STEPS 8u
typedef struct {
 uint8_t active_slot; /* 0-3, or 0xff when none */
 uint8_t step;        /* 0..8: neutral, UP, DOWN, A, B, START, BACK, LX LEFT, LX RIGHT */
 uint8_t wait_release;
 uint8_t complete;
 uint32_t accepted_edges;
} Gk307PhysicalQa;
void gk307_physical_init(Gk307PhysicalQa *state);
void gk307_physical_disconnect(Gk307PhysicalQa *state);
/* Pass ONLY the live successful XInputGetState sample, not scripted smoke.
 * Slot change is treated as disconnection, requiring new neutral. Returns
 * 1 for accepted input, 0 for invalid API contract. Atomic on bad pointers. */
int gk307_physical_step(Gk307PhysicalQa *state,uint8_t slot,
 const Gk99XinputRaw *actual_sample);
const char *gk307_physical_instruction(const Gk307PhysicalQa *state);
#endif
