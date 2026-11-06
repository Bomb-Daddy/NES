#include "gk1_battle_snapshot.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static unsigned passed=0;
#define CHECK(expr) do { if(!(expr)) {fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#expr);return 1;}passed++;}while(0)
static uint8_t sram[GK1_RAM_BYTES];
static uint32_t crc(const uint8_t *p,size_t n) {
    uint32_t c=UINT32_C(0xffffffff);
    for(size_t i=0;i<n;i++) {
        c^=p[i];for(unsigned k=0;k<8;k++) c=(c>>1)^((c&1u)?UINT32_C(0xedb88320):0);
    }
    return ~c;
}
static void rewrite_crc(uint8_t p[GK1_SNAPSHOT_SIZE]) {
    uint32_t c=crc(p,436);
    for(unsigned i=0;i<4;i++) p[436+i]=(uint8_t)(c>>(8u*i));
}
static int same(const Gk1BattleSnapshot *a,const Gk1BattleSnapshot *b) {
    return a->profile==b->profile && a->phase==b->phase && a->flags==b->flags &&
           !memcmp(a->rom_sha256,b->rom_sha256,32) &&
           a->vm_source_pointer==b->vm_source_pointer && a->vm_bank==b->vm_bank &&
           !memcmp(a->rng48,b->rng48,6) && a->scratch_word==b->scratch_word &&
           !memcmp(a->units,b->units,sizeof a->units) &&
           !memcmp(a->tiles,b->tiles,sizeof a->tiles);
}
static void init(Gk1BattleSnapshot *meta) {
    memset(sram,0x5a,sizeof sram);
    memset(meta,0,sizeof *meta);
    meta->profile=GK1_SNAP_APPROVED_COMBINED_MOD;
    meta->phase=GK1_PHASE_DESTINATION_SELECTED;
    meta->flags=GK1_SNAPSHOT_HAS_VM|GK1_SNAPSHOT_HAS_RNG;
    /* Real approved-ROM SHA256 from EXEC0039: metadata only; test RAM is SYNTHETIC. */
    const uint8_t id[32]={
      0x47,0x64,0xd4,0x63,0x59,0x58,0x1b,0xe1,0xe7,0xd1,0x2b,0xcb,0x85,0xbd,0x37,0x5c,
      0x84,0xcd,0x44,0x97,0xd1,0x08,0x4d,0x9d,0x30,0x6a,0x6e,0x88,0x4a,0xfb,0xe7,0xc5};
    memcpy(meta->rom_sha256,id,32);
    meta->vm_source_pointer=0xb110;meta->vm_bank=5;
    for(unsigned i=0;i<6;i++) meta->rng48[i]=(uint8_t)(0x40+i);
    for(unsigned i=0;i<220;i++) sram[0x60+i]=(uint8_t)(i*3u+1);
    for(unsigned i=0;i<160;i++) sram[0x15c+i]=(uint8_t)(i+128u);
    sram[0x13e]=0xfe;sram[0x13f]=0xff;
    /* True zero-soldier record at slot 1, same tile as moving slot 0. */
    sram[0x60]=40;sram[0x61]=0;sram[0x62]=7;sram[0x63]=7;
    sram[0x6b]=0;sram[0x6c]=0;sram[0x6d]=7;sram[0x6e]=7;
}
static int test_all(void) {
    Gk1BattleSnapshot meta,view,out,mark;
    uint8_t blob[GK1_SNAPSHOT_SIZE],bad[GK1_SNAPSHOT_SIZE],sentinel[GK1_SNAPSHOT_SIZE];
    init(&meta);memset(&view,0,sizeof view);
    CHECK(gk1_snapshot_capture(sram,sizeof sram,&meta,&view)==0);
    CHECK(view.scratch_word==65534u);
    CHECK(gk1_snapshot_encode(&view,blob,sizeof blob)==0);
    CHECK(!memcmp(blob,"GK1BTL01",8));CHECK(blob[8]==1);
    CHECK(crc(blob,436)==(uint32_t)blob[436]+((uint32_t)blob[437]<<8)+
                     ((uint32_t)blob[438]<<16)+((uint32_t)blob[439]<<24));
    CHECK(gk1_snapshot_decode(blob,sizeof blob,&out)==0);
    CHECK(same(&view,&out));
    CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,meta.profile,meta.rom_sha256,&out)==0);
    for(unsigned i=0;i<220;i++) CHECK(blob[56+i]==sram[0x60+i]);
    for(unsigned i=0;i<160;i++) CHECK(blob[276+i]==sram[0x15c+i]);
    for(unsigned i=0;i<6;i++) CHECK(blob[48+i]==meta.rng48[i]);
    Gk1NativeOccupancy occ;
    CHECK(gk1_snapshot_occupancy(&view,7,7,&occ)==0);
    CHECK((occ.at_position&3u)==3u);
    CHECK((occ.with_soldiers&3u)==1u);
    CHECK((occ.with_soldiers & ~occ.at_position)==0u);
    CHECK(gk1_snapshot_occupancy(&view,16,7,&occ)==-1);
    CHECK(gk1_snapshot_occupancy(&view,7,10,&occ)==-1);
    CHECK(gk1_snapshot_occupancy(&view,7,7,NULL)==-1);
    CHECK(gk1_snapshot_capture(NULL,8192,&meta,&out)==-1);
    CHECK(gk1_snapshot_capture(sram,8191,&meta,&out)==-1);
    CHECK(gk1_snapshot_capture(sram,8192,NULL,&out)==-1);
    CHECK(gk1_snapshot_capture(sram,8192,&meta,NULL)==-1);
    CHECK(gk1_snapshot_encode(NULL,blob,sizeof blob)==-1);
    CHECK(gk1_snapshot_encode(&view,NULL,sizeof blob)==-1);
    CHECK(gk1_snapshot_encode(&view,blob,sizeof blob-1)==-1);
    CHECK(gk1_snapshot_decode(NULL,sizeof blob,&out)==-1);
    CHECK(gk1_snapshot_decode(blob,sizeof blob,NULL)==-1);
    CHECK(gk1_snapshot_decode(blob,sizeof blob-1,&out)==-1);
    CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,meta.profile,NULL,&out)==-1);
    CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,0,meta.rom_sha256,&out)==-1);
    CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,meta.profile,meta.rom_sha256,NULL)==-1);
    CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,GK1_SNAP_STOCK_USA,meta.rom_sha256,&out)==-1);
    uint8_t wrong_hash[32];memcpy(wrong_hash,meta.rom_sha256,32);wrong_hash[10]^=1u;
    CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,meta.profile,wrong_hash,&out)==-1);
    /* Any singly flipped source byte, including unknown opaque fields, must
       fail accidental-corruption detection without a valid recomputed CRC. */
    for(unsigned i=0;i<436;i++) {
        memcpy(bad,blob,sizeof bad);bad[i]^=1u;
        memset(&mark,0xa5,sizeof mark);out=mark;
        CHECK(gk1_snapshot_decode(bad,sizeof bad,&out)==-1);
        CHECK(!memcmp(&mark,&out,sizeof out));
    }
    /* CRC re-forged malformed semantics still rejected. */
    const unsigned invalid_places[]={0,8,9,10,11,47};
    for(size_t j=0;j<sizeof invalid_places/sizeof invalid_places[0];j++) {
        unsigned at=invalid_places[j];memcpy(bad,blob,sizeof bad);
        bad[at]=at==11?0x80u:(at==9?0x42u:(at==10?0x40u:(uint8_t)(blob[at]^0xffu)));
        rewrite_crc(bad);out=mark;
        CHECK(gk1_snapshot_decode(bad,sizeof bad,&out)==-1);
        CHECK(!memcmp(&mark,&out,sizeof out));
    }
    /* A different, validly checksummed ROM identity is valid data but must
       be rejected by the ROM-pinned loader. */
    memcpy(bad,blob,sizeof bad);bad[12]^=1u;rewrite_crc(bad);
    CHECK(gk1_snapshot_decode(bad,sizeof bad,&out)==0);
    CHECK(gk1_snapshot_decode_for_rom(bad,sizeof bad,meta.profile,meta.rom_sha256,&out)==-1);
    /* Neither unknown RNG nor unknown VM pointer may be silently set. */
    memcpy(bad,blob,sizeof bad);bad[11]&=(uint8_t)~GK1_SNAPSHOT_HAS_RNG;rewrite_crc(bad);
    CHECK(gk1_snapshot_decode(bad,sizeof bad,&out)==-1);
    memcpy(bad,blob,sizeof bad);bad[11]&=(uint8_t)~GK1_SNAPSHOT_HAS_VM;rewrite_crc(bad);
    CHECK(gk1_snapshot_decode(bad,sizeof bad,&out)==-1);
    /* All 256 possible map raw byte values pass; high-bit remains opaque. */
    for(unsigned j=0;j<256;j++) {
        uint8_t raw=(uint8_t)j;
        for(unsigned i=0;i<160;i++) view.tiles[i]=raw;
        CHECK(gk1_snapshot_encode(&view,blob,sizeof blob)==0);
        CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,meta.profile,meta.rom_sha256,&out)==0);
        CHECK(!memcmp(out.tiles,view.tiles,160));
    }
    view=meta; /* metadata-only explicitly opaque/synthetic fixture */
    view.flags=0;view.vm_source_pointer=0;view.vm_bank=0;memset(view.rng48,0,6);
    CHECK(gk1_snapshot_encode(&view,blob,sizeof blob)==0);
    CHECK(gk1_snapshot_decode(blob,sizeof blob,&out)==0);
    CHECK(out.flags==0);
    view.profile=GK1_SNAP_STOCK_USA;view.phase=GK1_PHASE_UNDETERMINED;
    CHECK(gk1_snapshot_encode(&view,blob,sizeof blob)==0);
    CHECK(gk1_snapshot_decode_for_rom(blob,sizeof blob,GK1_SNAP_STOCK_USA,meta.rom_sha256,&out)==0);
    view.phase=GK1_PHASE_FINISHED_CONTROL;
    CHECK(gk1_snapshot_encode(&view,blob,sizeof blob)==0);
    CHECK(gk1_snapshot_decode(blob,sizeof blob,&out)==0);
    CHECK(out.phase==GK1_PHASE_FINISHED_CONTROL);
    view.phase=5;memset(sentinel,0x93,sizeof sentinel);memcpy(blob,sentinel,sizeof blob);
    CHECK(gk1_snapshot_encode(&view,blob,sizeof blob)==-1);
    CHECK(!memcmp(blob,sentinel,sizeof blob));
    view=meta;view.flags=0;
    CHECK(gk1_snapshot_capture(sram,sizeof sram,&view,&out)==-1);
    CHECK(gk1_snapshot_encode(&view,blob,sizeof blob)==-1);
    view=meta;memset(view.rom_sha256,0,32);
    CHECK(gk1_snapshot_capture(sram,sizeof sram,&view,&out)==-1);
    /* Flip only unrelated SRAM scratch and verify no leak to portable encoding. */
    init(&meta);CHECK(gk1_snapshot_capture(sram,sizeof sram,&meta,&view)==0);
    sram[0x500]^=0xff;
    CHECK(gk1_snapshot_capture(sram,sizeof sram,&meta,&out)==0);
    CHECK(same(&view,&out));
    /* Bit-for-bit preservation of all 20 opaque 11-byte records. */
    for(unsigned i=0;i<220;i++) CHECK(view.units[i/11][i%11]==sram[0x60+i]);
    puts("All checks passed.");
    printf("assertions=%u\n",passed);
    return 0;
}
int main(void) {return test_all();}
