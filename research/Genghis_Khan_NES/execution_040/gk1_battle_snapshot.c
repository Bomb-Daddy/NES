#include "gk1_battle_snapshot.h"
#include <string.h>

static const uint8_t sig[8]={'G','K','1','B','T','L','0','1'};
static uint16_t load16(const uint8_t *p) {return (uint16_t)((uint16_t)p[0]|((uint16_t)p[1]<<8));}
static void put16(uint8_t *p,uint16_t v) {p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static uint32_t load32(const uint8_t *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}
static void put32(uint8_t *p,uint32_t v) {
    for(unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(i*8u));
}
static uint32_t crc32c_iso(const uint8_t *p,size_t n) {
    uint32_t c=UINT32_C(0xffffffff);
    for(size_t i=0;i<n;i++) {
        c^=p[i];
        for(unsigned k=0;k<8;k++) c=(c>>1)^((c&1u)?UINT32_C(0xedb88320):0);
    }
    return ~c;
}
static int valid(const Gk1BattleSnapshot *s) {
    if(!s || (s->profile!=GK1_SNAP_STOCK_USA &&
              s->profile!=GK1_SNAP_APPROVED_COMBINED_MOD) ||
       s->phase>GK1_PHASE_FINISHED_CONTROL || (s->flags & ~GK1_SNAPSHOT_FLAG_MASK)) return 0;
    uint8_t z=0;
    for(unsigned i=0;i<32;i++) z|=s->rom_sha256[i];
    if(!z) return 0;
    if(!(s->flags&GK1_SNAPSHOT_HAS_VM) &&
        (s->vm_source_pointer || s->vm_bank)) return 0;
    if(!(s->flags&GK1_SNAPSHOT_HAS_RNG)) {
        for(unsigned i=0;i<6;i++) if(s->rng48[i]) return 0;
    }
    return 1;
}
int gk1_snapshot_capture(const uint8_t *ram,size_t len,
                         const Gk1BattleSnapshot *metadata,Gk1BattleSnapshot *out) {
    if(!ram || len!=GK1_RAM_BYTES || !out || !valid(metadata)) return -1;
    Gk1BattleSnapshot copy=*metadata;
    for(unsigned i=0;i<GK1_UNIT_COUNT;i++) {
        Gk1NativeUnit u;
        if(gk1_native_read_unit(ram,len,i,&u)!=0) return -1;
        memcpy(copy.units[i],u.raw,GK1_UNIT_STRIDE);
    }
    for(unsigned y=0;y<GK1_TILE_HEIGHT;y++) for(unsigned x=0;x<GK1_TILE_WIDTH;x++) {
        Gk1NativeTile t;
        if(gk1_native_read_tile(ram,len,x,y,&t)!=0) return -1;
        copy.tiles[y*GK1_TILE_WIDTH+x]=t.raw;
    }
    if(gk1_native_scratch_word(ram,len,&copy.scratch_word)!=0) return -1;
    *out=copy;return 0;
}
int gk1_snapshot_encode(const Gk1BattleSnapshot *s,uint8_t *dst,size_t n) {
    if(!dst || n!=GK1_SNAPSHOT_SIZE || !valid(s)) return -1;
    uint8_t b[GK1_SNAPSHOT_SIZE]={0};
    memcpy(b,sig,8);b[8]=GK1_SNAPSHOT_VERSION;b[9]=s->profile;
    b[10]=s->phase;b[11]=s->flags;
    memcpy(b+12,s->rom_sha256,32);
    put16(b+44,s->vm_source_pointer);b[46]=s->vm_bank;
    memcpy(b+48,s->rng48,6);
    put16(b+54,s->scratch_word);
    memcpy(b+56,s->units,220);
    memcpy(b+276,s->tiles,160);
    put32(b+436,crc32c_iso(b,436));
    memcpy(dst,b,GK1_SNAPSHOT_SIZE);return 0;
}
int gk1_snapshot_decode(const uint8_t *p,size_t n,Gk1BattleSnapshot *out) {
    if(!p || !out || n!=GK1_SNAPSHOT_SIZE) return -1;
    if(memcmp(p,sig,8) || p[8]!=GK1_SNAPSHOT_VERSION || p[47]!=0 ||
       crc32c_iso(p,436)!=load32(p+436)) return -1;
    Gk1BattleSnapshot copy={0};
    copy.profile=p[9];copy.phase=p[10];copy.flags=p[11];
    memcpy(copy.rom_sha256,p+12,32);
    copy.vm_source_pointer=load16(p+44);copy.vm_bank=p[46];
    memcpy(copy.rng48,p+48,6);
    copy.scratch_word=load16(p+54);
    memcpy(copy.units,p+56,220);
    memcpy(copy.tiles,p+276,160);
    if(!valid(&copy)) return -1;
    *out=copy;return 0;
}
int gk1_snapshot_decode_for_rom(const uint8_t *p,size_t n,uint8_t profile,
        const uint8_t hash[32],Gk1BattleSnapshot *out) {
    if(!hash || !out || (profile!=GK1_SNAP_STOCK_USA &&
         profile!=GK1_SNAP_APPROVED_COMBINED_MOD)) return -1;
    Gk1BattleSnapshot s;
    if(gk1_snapshot_decode(p,n,&s)!=0 || s.profile!=profile ||
       memcmp(hash,s.rom_sha256,32)!=0) return -1;
    *out=s;return 0;
}
int gk1_snapshot_occupancy(const Gk1BattleSnapshot *s,
                           unsigned x,unsigned y,Gk1NativeOccupancy *out) {
    if(!s || !out || x>=GK1_TILE_WIDTH || y>=GK1_TILE_HEIGHT || !valid(s)) return -1;
    Gk1NativeOccupancy v={0,0};
    for(unsigned i=0;i<GK1_UNIT_COUNT;i++) {
        const uint8_t *u=s->units[i];
        if(u[2]==x && u[3]==y) {
            uint32_t bit=UINT32_C(1)<<i;
            v.at_position|=bit;
            if(u[0] || u[1]) v.with_soldiers|=bit;
        }
    }
    *out=v;return 0;
}
