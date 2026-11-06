#include "gk1_capture_pair.h"
#include <string.h>

#define UNIT_START (GK1_UNIT_BASE-GK1_RAM_CPU_BASE)
#define TILE_START (GK1_TILE_BASE-GK1_RAM_CPU_BASE)
static const uint8_t *unit_at(const uint8_t *ram, unsigned slot) {
    return ram+UNIT_START+slot*GK1_UNIT_STRIDE;
}
static int valid_pair(const uint8_t *b, size_t nb, const uint8_t *a, size_t na) {
    return b!=NULL && a!=NULL && nb==GK1_RAM_BYTES && na==GK1_RAM_BYTES;
}
int gk1_capture_delta(const uint8_t *before, size_t n_before,
                      const uint8_t *after, size_t n_after,
                      unsigned dest_x, unsigned dest_y,
                      Gk1CaptureDelta *out) {
    if(!valid_pair(before,n_before,after,n_after) || out==NULL ||
       dest_x>=GK1_TILE_WIDTH || dest_y>=GK1_TILE_HEIGHT) return -1;
    Gk1CaptureDelta d={0};
    for(unsigned i=0;i<GK1_UNIT_COUNT;i++) {
        const uint8_t *b=unit_at(before,i), *a=unit_at(after,i);
        const uint32_t bit=UINT32_C(1)<<i;
        if(memcmp(b,a,GK1_UNIT_STRIDE)) d.changed_unit_records|=bit;
        if(memcmp(b,a,2)) d.changed_soldier_words|=bit;
        if(memcmp(b+2,a+2,2)) d.changed_coordinates|=bit;
        if(b[5]!=a[5]) d.changed_mobility|=bit;
        if(b[4]!=a[4] || memcmp(b+6,a+6,5)) d.changed_opaque_fields|=bit;
    }
    for(unsigned i=0;i<GK1_TILE_WIDTH*GK1_TILE_HEIGHT;i++)
        if(before[TILE_START+i]!=after[TILE_START+i]) d.changed_terrain_tiles++;
    if(gk1_native_scratch_word(before,n_before,&d.scratch_before) ||
       gk1_native_scratch_word(after,n_after,&d.scratch_after) ||
       gk1_native_occupancy(before,n_before,dest_x,dest_y,&d.destination_before) ||
       gk1_native_occupancy(after,n_after,dest_x,dest_y,&d.destination_after)) return -1;
    Gk1NativeTile t;
    if(gk1_native_read_tile(before,n_before,dest_x,dest_y,&t)) return -1;
    d.destination_raw_before=t.raw;
    if(gk1_native_read_tile(after,n_after,dest_x,dest_y,&t)) return -1;
    d.destination_raw_after=t.raw;
    *out=d;
    return 0;
}
static int matches_unit(const uint8_t *ram, unsigned slot, Gk1Unit u) {
    Gk1NativeUnit n;
    if(gk1_native_read_unit(ram,GK1_RAM_BYTES,slot,&n)) return 0;
    return n.soldiers==u.soldiers && n.x==u.x && n.y==u.y &&
           n.mobility==u.mobility;
}
static int exact_hashes(const Gk1CaptureWitness *w) {
    return w && w->rom_sha256 && w->checkpoint_sha256 &&
           strcmp(w->rom_sha256,GK1_SOURCE_ROM_SHA256)==0 &&
           strcmp(w->checkpoint_sha256,GK1_SOURCE_CHECKPOINT_SHA256)==0 &&
           w->command_witnessed==1 && w->comparable_phase==1;
}
Gk1PairVerdict gk1_capture_compare_fixture(const uint8_t *before, size_t n_before,
                                           const uint8_t *after, size_t n_after,
                                           const Gk1CaptureWitness *w,
                                           Gk1CaptureDelta *out_delta) {
    if(!valid_pair(before,n_before,after,n_after) || !exact_hashes(w) ||
       out_delta==NULL) return GK1_PAIR_UNVERIFIED;
    Gk1CaptureDelta d;
    if(gk1_capture_delta(before,n_before,after,n_after,8,7,&d)) return GK1_PAIR_UNVERIFIED;
    Gk1NativeUnit mover, friend_unit, enemy;
    if(gk1_native_read_unit(before,n_before,0,&mover) ||
       gk1_native_read_unit(before,n_before,1,&friend_unit) ||
       gk1_native_read_unit(before,n_before,10,&enemy)) return GK1_PAIR_UNVERIFIED;
    if(mover.soldiers!=40 || mover.x!=7 || mover.y!=7 ||
       friend_unit.soldiers!=40 || enemy.soldiers!=70 || d.scratch_before!=0 ||
       d.destination_raw_before>=7) return GK1_PAIR_UNVERIFIED;
    const uint32_t friend_bit=UINT32_C(1)<<1, enemy_bit=UINT32_C(1)<<10;
    Gk1Occupancy occ=GK1_OCC_EMPTY;
    if(d.destination_before.at_position==0) {
        if(friend_unit.x!=6 || friend_unit.y!=9) return GK1_PAIR_UNVERIFIED;
    } else if(d.destination_before.at_position==friend_bit &&
              d.destination_before.with_soldiers==friend_bit) {
        occ=GK1_OCC_FRIEND_SLOT1;
    } else if(d.destination_before.at_position==enemy_bit &&
              d.destination_before.with_soldiers==enemy_bit) {
        if(friend_unit.x!=6 || friend_unit.y!=9) return GK1_PAIR_UNVERIFIED;
        occ=GK1_OCC_ENEMY_SLOT10;
    } else return GK1_PAIR_UNVERIFIED;
    Gk1BattleState fixture=gk1_checkpoint_fixture(mover.mobility,d.destination_raw_before,occ);
    Gk1BattleState projected;
    const Gk1MoveResult result=gk1_apply_observed_step(&fixture,8,7,occ,&projected);
    if(result==GK1_UNVERIFIED || result!=w->expected_result) return GK1_PAIR_UNVERIFIED;
    /* Contradiction checks, deliberately narrow: only authoritative fixture
     * projected unit fields, full unchanged terrain, unchanged unrelated
     * raw unit records, and the scratch word. No phase/RNG/AI inference. */
    if(d.changed_terrain_tiles!=0 || d.scratch_after!=projected.completion_word ||
       !matches_unit(after,0,projected.mover) ||
       !matches_unit(after,1,projected.friendly_slot1) ||
       (occ==GK1_OCC_ENEMY_SLOT10 && !matches_unit(after,10,projected.enemy_slot10)))
        return GK1_PAIR_MISMATCH;
    for(unsigned i=2;i<GK1_UNIT_COUNT;i++) {
        if(i==10) continue;
        if(memcmp(unit_at(before,i),unit_at(after,i),GK1_UNIT_STRIDE)!=0)
            return GK1_PAIR_MISMATCH;
    }
    if(memcmp(unit_at(before,10),unit_at(after,10),GK1_UNIT_STRIDE)!=0)
        return GK1_PAIR_MISMATCH;
    /* The fixture projection has never established that unit-type and
     * opaque bytes are unmodified, so expose them in delta, not a rule. */
    *out_delta=d;
    return GK1_PAIR_SUBSET_MATCH;
}
