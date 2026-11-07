#ifndef GK1_SNAPSHOT_VIEW_H
#define GK1_SNAPSHOT_VIEW_H
/* EXEC0044 - read-only, identity-gated battle-snapshot viewer input.
 * No terrain classification, movement legality, unit allegiance, phase
 * completion, or genuine-gameplay authentication is inferred here.
 */
#include <stddef.h>
#include <stdint.h>
#include "../execution_040/gk1_battle_snapshot.h"

typedef struct {
    Gk1BattleSnapshot capture;
    uint32_t recorded_mask[GK1_TILE_WIDTH * GK1_TILE_HEIGHT];
    uint32_t populated_mask[GK1_TILE_WIDTH * GK1_TILE_HEIGHT];
    unsigned valid_positions;
    unsigned out_of_bounds_positions;
    unsigned units_with_soldiers;
    unsigned empty_record_positions;
} Gk1BattleView;

/* Requires exact matching caller-specified ROM profile and 32-byte SHA.
 * The caller must separately verify that the original NES ROM has that hash.
 * On failure, output remains unmodified. No disk writes are performed.
 */
int gk1_view_decode(const uint8_t *encoded, size_t len, uint8_t profile,
                    const uint8_t expected_rom_sha256[32], Gk1BattleView *out);
int gk1_view_load_file(const char *path, uint8_t profile,
                       const uint8_t expected_rom_sha256[32], Gk1BattleView *out);
/* Exactly 64 hex digits, no prefix/whitespace; leaves out unchanged on error. */
int gk1_view_parse_hash(const char *hex, uint8_t out[32]);
/* Exact names `stock` and `combined` only. */
int gk1_view_parse_profile(const char *name, uint8_t *out);
/* Read-only tile/occupancy query, no move legality or alliance inference. */
int gk1_view_tile(const Gk1BattleView *view, unsigned x, unsigned y,
                  uint8_t *raw, uint32_t *recorded, uint32_t *populated);
#endif
