#ifndef GK1_NATIVE_RAM_VIEW_H
#define GK1_NATIVE_RAM_VIEW_H
/* EXEC0035. Copyright-clean, read-only adapter for archived NES SRAM captures.
 * Evidence: EXEC0022 20x11-byte unit layout, EXEC0026 16x10 tile layout,
 * EXEC0028 u16 scratch $613E:$613F, EXEC0033 controlled occupancy.
 * This decoder does NOT infer legal movement, ownership, terrain semantics,
 * end-of-turn completion, NES CPU PC or Windows battle-engine parity.
 */
#include <stddef.h>
#include <stdint.h>
#define GK1_RAM_BYTES 8192u
#define GK1_RAM_CPU_BASE 0x6000u
#define GK1_UNIT_BASE 0x6060u
#define GK1_UNIT_STRIDE 11u
#define GK1_UNIT_COUNT 20u
#define GK1_TILE_BASE 0x615Cu
#define GK1_TILE_WIDTH 16u
#define GK1_TILE_HEIGHT 10u
#define GK1_SCRATCH_WORD 0x613Eu

typedef struct {
    uint16_t soldiers;
    uint8_t x;
    uint8_t y;
    uint8_t type;
    uint8_t mobility;
    uint8_t raw[GK1_UNIT_STRIDE]; /* Opaque unit fields must survive readout. */
} Gk1NativeUnit;

typedef struct {
    uint8_t raw;
    uint16_t cpu_address; /* SRAM logical CPU address, NOT a 6502 PC. */
} Gk1NativeTile;

typedef struct {
    uint32_t at_position; /* bits 0..19, including zero-soldier records */
    uint32_t with_soldiers; /* subset, descriptive only, not legality */
} Gk1NativeOccupancy;

/* All routines return 0 on success, -1 on invalid input, and leave output
 * untouched on error. Input buffer must contain exactly 8192 SRAM bytes.
 */
int gk1_native_read_unit(const uint8_t *ram, size_t len, unsigned slot, Gk1NativeUnit *out);
int gk1_native_read_tile(const uint8_t *ram, size_t len, unsigned x, unsigned y, Gk1NativeTile *out);
int gk1_native_scratch_word(const uint8_t *ram, size_t len, uint16_t *out);
int gk1_native_occupancy(const uint8_t *ram, size_t len, unsigned x, unsigned y,
                         Gk1NativeOccupancy *out);
#endif
