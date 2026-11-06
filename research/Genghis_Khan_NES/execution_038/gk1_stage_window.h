#ifndef GK1_STAGE_WINDOW_H
#define GK1_STAGE_WINDOW_H
/* EXEC0038. Native modified-ROM archived stage -> standalone Windows port
 * diagnostic acceptance. Exact isolated intervention cases ONLY. Not a
 * generalized unit-combination algorithm or a natural-gameplay replay.
 */
#include "../execution_035/gk1_native_ram_view.h"
#include "../execution_034/gk1_tactical_slice.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
    GK1_STAGE_JOIN=1, GK1_STAGE_ENEMY_BLOCK=2,
    GK1_STAGE_LOW_MOBILITY_BLOCK=3, GK1_STAGE_EXACT_MOBILITY=4
} Gk1StageCase;

typedef struct {
    const char *rom_sha256; /* caller-provided attestation, not self-authenticating */
    const char *checkpoint_sha256;
    unsigned controller_capture_attested;
    unsigned intervention_disclosed;
    unsigned phases_attested;
    uint16_t selected_vm_pc, attempt_vm_pc, idle_vm_pc;
} Gk1StageProvenance;

typedef struct {
    unsigned moved, join_like, idle_scratch_transition;
    uint16_t selected_soldiers, attempt_soldiers, idle_soldiers;
    uint16_t selected_partner, attempt_partner, idle_partner;
    uint16_t selected_completion, attempt_completion, idle_completion;
    uint8_t selected_614c, attempt_614c, idle_614c;
    uint8_t selected_614e, attempt_614e, idle_614e;
    uint8_t percent_loss_and_random_expected; /* original sourced attrition output */
} Gk1StageWindowResult;

typedef enum {
    GK1_WINDOW_UNVERIFIED=-2, GK1_WINDOW_INVALID=-1,
    GK1_WINDOW_MISMATCH=0, GK1_WINDOW_BOUNDED_MATCH=1
} Gk1StageWindowVerdict;

/* Fail-closed, read-only. On EVERY non-match, out remains byte-for-byte
 * unchanged. This is a comparison of already captured stages, not an action.
 * Provenance identity/phase claims must be independently validated by caller.
 */
Gk1StageWindowVerdict gk1_stage_check(
    Gk1StageCase kind, const Gk1StageProvenance *source,
    const uint8_t *selected, size_t ns,
    const uint8_t *attempt, size_t na,
    const uint8_t *idle, size_t ni,
    Gk1StageWindowResult *out);
#endif
