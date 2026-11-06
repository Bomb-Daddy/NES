#include "gk1_capture_pair.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned checks=0;
#define CHECK(cond) do { checks++; if(!(cond)) {fprintf(stderr,"FAIL %s:%d %s\n",__FILE__,__LINE__,#cond);exit(1);} } while(0)
#define S (GK1_RAM_BYTES)
static uint8_t before[S],after[S];
static const Gk1CaptureWitness witness_template={
    GK1_SOURCE_ROM_SHA256,GK1_SOURCE_CHECKPOINT_SHA256,1,1,GK1_FINISHED};
static void set_word(uint8_t *ram, unsigned addr, unsigned v) {
    ram[addr-GK1_RAM_CPU_BASE]=(uint8_t)v;
    ram[addr-GK1_RAM_CPU_BASE+1]=(uint8_t)(v>>8);
}
static void set_unit(uint8_t *ram,unsigned slot,Gk1Unit u) {
    const unsigned i=GK1_UNIT_BASE-GK1_RAM_CPU_BASE+GK1_UNIT_STRIDE*slot;
    ram[i]=(uint8_t)u.soldiers;ram[i+1]=(uint8_t)(u.soldiers>>8);
    ram[i+2]=u.x;ram[i+3]=u.y;ram[i+5]=u.mobility;
}
static void make_case(uint8_t terrain,uint8_t mobility,Gk1Occupancy occ,Gk1MoveResult *result) {
    memset(before,0,sizeof before);
    for(unsigned i=0;i<GK1_UNIT_COUNT;i++) {
        unsigned off=GK1_UNIT_BASE-GK1_RAM_CPU_BASE+GK1_UNIT_STRIDE*i;
        before[off+2]=255;before[off+3]=255;
        before[off+4]=(uint8_t)(i+11); /* opaque type remains opaque */
        before[off+6]=(uint8_t)(i+50);
    }
    Gk1BattleState s=gk1_checkpoint_fixture(mobility,terrain,occ), projected;
    set_unit(before,0,s.mover);
    set_unit(before,1,s.friendly_slot1);
    set_unit(before,10,s.enemy_slot10);
    before[GK1_TILE_BASE-GK1_RAM_CPU_BASE+7*16+8]=terrain;
    memcpy(after,before,sizeof before);
    *result=gk1_apply_observed_step(&s,8,7,occ,&projected);
    CHECK(*result!=GK1_UNVERIFIED);
    set_unit(after,0,projected.mover);
    set_unit(after,1,projected.friendly_slot1);
    set_unit(after,10,projected.enemy_slot10);
    set_word(after,GK1_SCRATCH_WORD,projected.completion_word);
}
int main(void) {
    const struct {uint8_t t,m;Gk1Occupancy occ;Gk1MoveResult result;uint16_t soldiers,word;}
      cases[]={
        {3,10,GK1_OCC_EMPTY,GK1_STEP_PENDING,34,0},
        {3,2,GK1_OCC_EMPTY,GK1_REJECTED,40,0},
        {3,4,GK1_OCC_EMPTY,GK1_FINISHED,34,1},
        {3,10,GK1_OCC_FRIEND_SLOT1,GK1_FINISHED,74,2},
        {3,10,GK1_OCC_ENEMY_SLOT10,GK1_REJECTED,40,0},
        {0,10,GK1_OCC_EMPTY,GK1_FINISHED,32,1}
      };
    for(unsigned k=0;k<sizeof cases/sizeof cases[0];k++) {
        Gk1MoveResult r;
        make_case(cases[k].t,cases[k].m,cases[k].occ,&r);
        CHECK(r==cases[k].result);
        Gk1CaptureWitness w=witness_template;w.expected_result=r;
        Gk1CaptureDelta delta={0};
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&delta)==GK1_PAIR_SUBSET_MATCH);
        CHECK((delta.changed_unit_records & 1u)==(r==GK1_REJECTED?0u:1u));
        CHECK(delta.changed_terrain_tiles==0);
        CHECK(delta.scratch_after==cases[k].word);
        Gk1NativeUnit n;
        CHECK(gk1_native_read_unit(after,S,0,&n)==0);
        CHECK(n.soldiers==cases[k].soldiers);
        CHECK(delta.destination_raw_before==cases[k].t);
        CHECK(delta.destination_raw_after==cases[k].t);
        CHECK(delta.destination_after.with_soldiers ==
              (cases[k].occ==GK1_OCC_ENEMY_SLOT10? (1u<<10):
               r==GK1_REJECTED?0u:1u));
        if(cases[k].occ==GK1_OCC_FRIEND_SLOT1) {
            CHECK(delta.destination_after.at_position==3);
            CHECK(delta.destination_after.with_soldiers==1);
            CHECK(delta.changed_unit_records==3);
        }
        if(cases[k].occ==GK1_OCC_ENEMY_SLOT10) {
            CHECK(delta.destination_before.with_soldiers==(1u<<10));
            CHECK(delta.changed_unit_records==0);
        }
        if(r==GK1_REJECTED){CHECK(delta.changed_unit_records==0);CHECK(delta.scratch_after==0);}
        /* A wrong external phase/source attribution must not be quietly accepted. */
        Gk1CaptureDelta guard={0};guard.scratch_after=0xbeef;
        w.command_witnessed=0;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_UNVERIFIED);
        CHECK(guard.scratch_after==0xbeef);
        w.command_witnessed=1; w.comparable_phase=0;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_UNVERIFIED);
        w.comparable_phase=1;w.rom_sha256=GK1_SOURCE_CHECKPOINT_SHA256;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_UNVERIFIED);
        w.rom_sha256=GK1_SOURCE_ROM_SHA256;w.checkpoint_sha256="wrong";
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_UNVERIFIED);
        w.checkpoint_sha256=GK1_SOURCE_CHECKPOINT_SHA256;
        w.expected_result=(r==GK1_REJECTED?GK1_FINISHED:GK1_REJECTED);
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_UNVERIFIED);
        CHECK(guard.scratch_after==0xbeef);
        /* Mutate a real observable output; verdict MUST be mismatch. */
        w.expected_result=r;
        after[GK1_UNIT_BASE-GK1_RAM_CPU_BASE] ^=1;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_MISMATCH);
        after[GK1_UNIT_BASE-GK1_RAM_CPU_BASE]^=1;
        CHECK(guard.scratch_after==0xbeef);
        after[GK1_TILE_BASE-GK1_RAM_CPU_BASE+7*16+8]^=1;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_MISMATCH);
        after[GK1_TILE_BASE-GK1_RAM_CPU_BASE+7*16+8]^=1;
        after[GK1_UNIT_BASE-GK1_RAM_CPU_BASE+4*GK1_UNIT_STRIDE+8]^=1;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_MISMATCH);
        after[GK1_UNIT_BASE-GK1_RAM_CPU_BASE+4*GK1_UNIT_STRIDE+8]^=1;
        after[GK1_SCRATCH_WORD-GK1_RAM_CPU_BASE]^=1;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_MISMATCH);
        after[GK1_SCRATCH_WORD-GK1_RAM_CPU_BASE]^=1;
        CHECK(gk1_capture_compare_fixture(before,S,after,S,&w,&guard)==GK1_PAIR_SUBSET_MATCH);
    }
    Gk1MoveResult r;
    make_case(3,10,GK1_OCC_FRIEND_SLOT1,&r);
    Gk1CaptureDelta original={0}; original.scratch_after=0xbeef;
    CHECK(gk1_capture_delta(before,S,after,S,8,7,&original)==0);
    CHECK(original.changed_soldier_words==3);
    CHECK(original.changed_coordinates==1);
    CHECK(original.changed_mobility==0);
    CHECK(original.changed_opaque_fields==0);
    CHECK(original.destination_before.at_position==(1u<<1));
    CHECK(original.destination_after.at_position==3);
    CHECK(original.destination_after.with_soldiers==1);
    /* Phase-independent diff preserves high-bit tile bytes and doesn't decide legality. */
    before[GK1_TILE_BASE-GK1_RAM_CPU_BASE+5]=0x84;
    memcpy(after,before,S);
    CHECK(gk1_capture_delta(before,S,after,S,5,0,&original)==0);
    CHECK(original.destination_raw_before==0x84);
    CHECK(original.destination_raw_after==0x84);
    CHECK(original.changed_unit_records==0);
    CHECK(original.changed_terrain_tiles==0);
    after[GK1_TILE_BASE-GK1_RAM_CPU_BASE+5]=0x82;
    CHECK(gk1_capture_delta(before,S,after,S,5,0,&original)==0);
    CHECK(original.changed_terrain_tiles==1);
    CHECK(original.destination_raw_before==0x84);
    CHECK(original.destination_raw_after==0x82);
    /* One zero-soldier unit can remain physically co-located. */
    set_unit(before,1,(Gk1Unit){0,8,7,9});
    CHECK(gk1_capture_delta(before,S,before,S,8,7,&original)==0);
    CHECK((original.destination_before.at_position&(1u<<1))!=0);
    CHECK((original.destination_before.with_soldiers&(1u<<1))==0);
    Gk1CaptureDelta sentinel;memset(&sentinel,0xa5,sizeof sentinel);
    uint8_t copy[sizeof sentinel];memcpy(copy,&sentinel,sizeof copy);
    CHECK(gk1_capture_delta(NULL,S,after,S,8,7,&sentinel)==-1);
    CHECK(gk1_capture_delta(before,S-1,after,S,8,7,&sentinel)==-1);
    CHECK(gk1_capture_delta(before,S,after,S-1,8,7,&sentinel)==-1);
    CHECK(gk1_capture_delta(before,S,after,S,16,7,&sentinel)==-1);
    CHECK(gk1_capture_delta(before,S,after,S,8,10,&sentinel)==-1);
    CHECK(memcmp(copy,&sentinel,sizeof copy)==0);
    CHECK(gk1_capture_compare_fixture(before,S,after,S,NULL,&sentinel)==GK1_PAIR_UNVERIFIED);
    CHECK(gk1_capture_compare_fixture(before,S,after,S,&witness_template,NULL)==GK1_PAIR_UNVERIFIED);
    CHECK(memcmp(copy,&sentinel,sizeof copy)==0);
    puts("EXEC0036 C11 snapshot-pair integration PASS");
    printf("assertions=%u\n",checks);
    return 0;
}
