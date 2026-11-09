#ifndef GK69_MESEN_RAM_MAP_H
#define GK69_MESEN_RAM_MAP_H
/* Copyright-clean native adapter for ONE exact observed approved-modified NES
 * Mesen WorkRAM capture (MESEN-GAP-0001, November 7 2026). This is NOT a
 * Classic stock map, playable battle, movement-cost table or water mapping.
 * The original raw WorkRAM, ROM and graphics remain private, externally loaded.
 */
#include <stddef.h>
#include <stdint.h>
#define GK69_RAM_LENGTH 8192u
#define GK69_MAP_OFFSET 0x015cu
#define GK69_MAP_WIDTH 16u
#define GK69_MAP_HEIGHT 10u
#define GK69_MAP_CELLS 160u
typedef struct {
 uint8_t raw[GK69_MAP_CELLS];
 uint8_t x,y;
 uint16_t source_plains,source_forest,source_mountain,unknown;
} Gk69MesenMap;
typedef enum {
 GK69_MAP_OK=0,GK69_MAP_BAD_INPUT=1,GK69_MAP_NOT_EXACT_CAPTURE=2,
 GK69_MAP_IO=3
} Gk69MapStatus;
/* Read exactly one previously authenticated 8192-byte nesWorkRam capture.
 * Exact full-memory SHA protects provenance; it is not a ROM hash check.
 * On all failure statuses, the entire output is byte-identical to before.
 */
Gk69MapStatus gk69_map_from_ram(const uint8_t *ram,size_t length,Gk69MesenMap *out);
Gk69MapStatus gk69_map_from_file(const char *path,Gk69MesenMap *out);
/* Controller cursor confined to the original 16x10 grid. */
int gk69_map_move(Gk69MesenMap *map,int dx,int dy);
/* Descriptive labels ONLY for raw2/3/4 whose appearance was captured;
 * high-bit edge bytes retain their exact raw values and are unclassified. */
const char *gk69_map_label(uint8_t raw);
#endif
