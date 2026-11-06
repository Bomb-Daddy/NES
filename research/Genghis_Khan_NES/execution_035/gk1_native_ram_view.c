#include "gk1_native_ram_view.h"
#include <string.h>

static int valid(const uint8_t *ram, size_t len, const void *out) {
    return ram != NULL && len == GK1_RAM_BYTES && out != NULL;
}

int gk1_native_read_unit(const uint8_t *ram, size_t len, unsigned slot, Gk1NativeUnit *out) {
    if (!valid(ram, len, out) || slot >= GK1_UNIT_COUNT) return -1;
    const size_t at = GK1_UNIT_BASE-GK1_RAM_CPU_BASE + slot*GK1_UNIT_STRIDE;
    Gk1NativeUnit v;
    memcpy(v.raw, ram+at, sizeof v.raw);
    v.soldiers = (uint16_t)((uint16_t)v.raw[0] | ((uint16_t)v.raw[1]<<8));
    v.x = v.raw[2];
    v.y = v.raw[3];
    v.type = v.raw[4];
    v.mobility = v.raw[5];
    *out = v;
    return 0;
}

int gk1_native_read_tile(const uint8_t *ram, size_t len, unsigned x, unsigned y, Gk1NativeTile *out) {
    if (!valid(ram, len, out) || x>=GK1_TILE_WIDTH || y>=GK1_TILE_HEIGHT) return -1;
    Gk1NativeTile v;
    v.cpu_address = (uint16_t)(GK1_TILE_BASE + GK1_TILE_WIDTH*y + x);
    v.raw = ram[(size_t)(v.cpu_address-GK1_RAM_CPU_BASE)];
    *out=v;
    return 0;
}

int gk1_native_scratch_word(const uint8_t *ram, size_t len, uint16_t *out) {
    if (!valid(ram, len, out)) return -1;
    const size_t i=GK1_SCRATCH_WORD-GK1_RAM_CPU_BASE;
    const uint16_t w=(uint16_t)((uint16_t)ram[i] | ((uint16_t)ram[i+1]<<8));
    *out=w;
    return 0;
}

int gk1_native_occupancy(const uint8_t *ram, size_t len, unsigned x, unsigned y, Gk1NativeOccupancy *out) {
    if (!valid(ram, len, out) || x>=GK1_TILE_WIDTH || y>=GK1_TILE_HEIGHT) return -1;
    Gk1NativeOccupancy v={0,0};
    for(unsigned i=0;i<GK1_UNIT_COUNT;i++) {
        const size_t at=GK1_UNIT_BASE-GK1_RAM_CPU_BASE+i*GK1_UNIT_STRIDE;
        if(ram[at+2]==x && ram[at+3]==y) {
            const uint32_t bit=UINT32_C(1)<<i;
            v.at_position |= bit;
            if(ram[at]!=0 || ram[at+1]!=0) v.with_soldiers |= bit;
        }
    }
    *out=v;
    return 0;
}
