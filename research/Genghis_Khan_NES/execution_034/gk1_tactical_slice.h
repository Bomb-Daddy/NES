#ifndef GK1_TACTICAL_SLICE_H
#define GK1_TACTICAL_SLICE_H

/* GK1 English NES -> standalone Windows tactical port, EXEC0034.
 * Copyright-clean numerical C11 integration slice; NOT a complete game.
 * Source oracles: EXEC0030 (phase/cost), EXEC0032 (casualties),
 * EXEC0033 (low mobility + occupied tile), approved combined modified ROM.
 * Natural source/executable Windows parity is NOT established.
 */
#include <stdint.h>

#define GK1_SOURCE_ROM_SHA256 "4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5"
#define GK1_SOURCE_CHECKPOINT_SHA256 "e1b6266c963a48d736627812d024910b6d600c2ad8571813eb3b0999e0f2938b"
#define GK1_SEED_INPUT UINT64_C(0xc7c3f28f2363)
#define GK1_SEED_AFTER_HAZARD UINT64_C(0xa31e4ecd8fd1)

typedef enum { GK1_PHASE_MOVE_WHERE=1, GK1_PHASE_ORDERS=2 } Gk1Phase;
typedef enum { GK1_OCC_EMPTY=0, GK1_OCC_FRIEND_SLOT1=1, GK1_OCC_ENEMY_SLOT10=2 } Gk1Occupancy;
typedef enum { GK1_UNVERIFIED=-1, GK1_REJECTED=0, GK1_STEP_PENDING=1, GK1_FINISHED=2 } Gk1MoveResult;

typedef struct { uint16_t soldiers; uint8_t x,y,mobility; } Gk1Unit;
typedef struct {
    Gk1Unit mover;
    Gk1Unit friendly_slot1;
    Gk1Unit enemy_slot10;
    uint64_t rng48;
    uint16_t completion_word;
    uint16_t gold,food;
    uint8_t tile_at_destination;
    Gk1Phase phase;
    uint8_t has_pending_step;
    uint8_t combined_mod_profile; /* Must be 1; other games are distinct oracles. */
    uint8_t exact_checkpoint_unit_profile; /* Must be 1: approved cavalry/unit identity, not a unit-type inference. */
    uint32_t other_units_guard;   /* opaque digest passed through unchanged */
} Gk1BattleState;

typedef struct {
    uint16_t after_percent;
    uint16_t after_random;
    uint8_t percent;
    uint8_t has_random;
    uint8_t rng_remainder;
    uint64_t next_seed;
} Gk1CasualtyResult;

/* Exact source tables; outside 0..6 returns -1. 255 hazard means no draw. */
int gk1_mobility_cost(unsigned terrain);
int gk1_hazard_percent(unsigned terrain);
int gk1_casualties(uint16_t soldiers, unsigned terrain, uint64_t seed48,
                   Gk1CasualtyResult *out);

/* Fully typed fixture state. Use only in tests or an explicit source-profile
 * checkpoint replay, not as a synthesized 'typical' campaign state. */
Gk1BattleState gk1_checkpoint_fixture(uint8_t mobility, uint8_t terrain,
                                        Gk1Occupancy occupant);

/* Atomic: unknown/invalid inputs leave output untouched, no guessed moves.
 * Destination is intentionally the one independently tested adjacent cell.
 * Never generalize this to multi-tile/path/Entire/Divide/battle termination. */
Gk1MoveResult gk1_apply_observed_step(const Gk1BattleState *before,
                                      uint8_t dest_x, uint8_t dest_y,
                                      Gk1Occupancy occupant,
                                      Gk1BattleState *after);
/* B finishes a STEP_PENDING path, rather than undoing it. Other B cases are
 * still not proved by these traces and fail closed. */
Gk1MoveResult gk1_finish_observed_path(const Gk1BattleState *before,
                                       Gk1BattleState *after);
#endif
