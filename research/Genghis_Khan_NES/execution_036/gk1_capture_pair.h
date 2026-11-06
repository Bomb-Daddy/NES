#ifndef GK1_CAPTURE_PAIR_H
#define GK1_CAPTURE_PAIR_H
/* EXEC0036. New copyright-clean capture-to-port consistency layer.
 * Diffing SRAM does NOT prove completion, legal moves or unobserved mechanics.
 * witness is EXTERNAL EVIDENCE provided by a real controller trace, not
 * proof invented by this module. Synthetic tests exercise integration only.
 */
#include "../execution_034/gk1_tactical_slice.h"
#include "../execution_035/gk1_native_ram_view.h"

typedef struct {
    uint32_t changed_unit_records;
    uint32_t changed_soldier_words;
    uint32_t changed_coordinates;
    uint32_t changed_mobility;
    uint32_t changed_opaque_fields;
    unsigned changed_terrain_tiles;
    uint16_t scratch_before, scratch_after;
    Gk1NativeOccupancy destination_before, destination_after;
    uint8_t destination_raw_before, destination_raw_after;
} Gk1CaptureDelta;

typedef struct {
    const char *rom_sha256;        /* Must match the combined approved ROM. */
    const char *checkpoint_sha256; /* Must match the approved battle checkpoint. */
    uint8_t command_witnessed;    /* Attested externally; NOT inferred from SRAM. */
    uint8_t comparable_phase;     /* Attested same source-command boundary. */
    Gk1MoveResult expected_result;
} Gk1CaptureWitness;

typedef enum {
    GK1_PAIR_UNVERIFIED = -1, /* unknown fixture or missing witness */
    GK1_PAIR_MISMATCH = 0,    /* a supported witnessed fixture contradicts projection */
    GK1_PAIR_SUBSET_MATCH = 1 /* exact tested observable fields only, NOT full parity */
} Gk1PairVerdict;

/* No output modifications on failure. This is a descriptive pairwise diff,
 * even when snapshots are at different VM phases. */
int gk1_capture_delta(const uint8_t *before, size_t n_before,
                      const uint8_t *after, size_t n_after,
                      unsigned dest_x, unsigned dest_y,
                      Gk1CaptureDelta *out);

/* Strictly compares the formerly native-observed adjacent (7,7)->(8,7)
 * controlled combined-ROM fixture subset against executable EXEC0034.
 * Requires external provenance and phase witness; cannot infer them.
 * Uses EXEC0035 to decode RAM and excludes legal/Divide/AI generalization.
 * out_delta remains untouched on UNVERIFIED/MISMATCH. */
Gk1PairVerdict gk1_capture_compare_fixture(const uint8_t *before, size_t n_before,
                                           const uint8_t *after, size_t n_after,
                                           const Gk1CaptureWitness *witness,
                                           Gk1CaptureDelta *out_delta);
#endif
