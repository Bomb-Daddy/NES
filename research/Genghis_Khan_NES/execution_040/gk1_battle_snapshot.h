#ifndef GK1_BATTLE_SNAPSHOT_H
#define GK1_BATTLE_SNAPSHOT_H
/* EXEC0040: source-profile-locked portable battle *research snapshot*.
 * Not a full NES savestate, full SRAM, player campaign save, or a
 * claim that an observed VM phase is command-complete.
 * No game art, copyrighted ROM, captured SRAM, or save data is embedded.
 */
#include <stddef.h>
#include <stdint.h>
#include "../execution_035/gk1_native_ram_view.h"
#define GK1_SNAPSHOT_SIZE 440u
#define GK1_SNAPSHOT_CRC_OFFSET 436u
#define GK1_SNAPSHOT_VERSION 1u
#define GK1_SNAPSHOT_HAS_VM 1u
#define GK1_SNAPSHOT_HAS_RNG 2u
#define GK1_SNAPSHOT_FLAG_MASK 3u

typedef enum {
    GK1_SNAP_STOCK_USA=1,
    GK1_SNAP_APPROVED_COMBINED_MOD=2
} Gk1SnapProfile;
typedef enum {
    GK1_PHASE_UNDETERMINED=0,
    GK1_PHASE_DESTINATION_SELECTED=1,
    GK1_PHASE_AFTER_ATTEMPT=2,
    GK1_PHASE_AFTER_IDLE=3,
    GK1_PHASE_FINISHED_CONTROL=4
} Gk1SnapPhase;
typedef struct {
    uint8_t profile;               /* ROM identity enum, NOT proof by itself */
    uint8_t phase;                 /* observed phase, never inferred */
    uint8_t flags;                 /* VM and RNG presence gates */
    uint8_t rom_sha256[32];        /* hash supplied by the caller */
    uint16_t vm_source_pointer;    /* original VM data pointer, NOT 6502 PC */
    uint8_t vm_bank;
    uint8_t rng48[6];
    uint16_t scratch_word;         /* RAM $613E:$613F u16 */
    uint8_t units[GK1_UNIT_COUNT][GK1_UNIT_STRIDE];
    uint8_t tiles[GK1_TILE_WIDTH*GK1_TILE_HEIGHT];
} Gk1BattleSnapshot;

/* Zero is success, -1 invalid input. All failures leave output untouched.
 * No memory layout of native C structs is used on disk.
 */
int gk1_snapshot_capture(const uint8_t *sram, size_t len,
                         const Gk1BattleSnapshot *metadata,
                         Gk1BattleSnapshot *out);
int gk1_snapshot_encode(const Gk1BattleSnapshot *view, uint8_t *dst, size_t n);
int gk1_snapshot_decode(const uint8_t *src, size_t n,
                        Gk1BattleSnapshot *out);
/* Fail closed when expected ROM identity does not match. */
int gk1_snapshot_decode_for_rom(const uint8_t *src, size_t n,
                                uint8_t expected_profile,
                                const uint8_t expected_rom_sha256[32],
                                Gk1BattleSnapshot *out);
/* Purely descriptive occupancy. Includes zero-soldier records in at_position. */
int gk1_snapshot_occupancy(const Gk1BattleSnapshot *view,
                           unsigned x, unsigned y, Gk1NativeOccupancy *out);
#endif
