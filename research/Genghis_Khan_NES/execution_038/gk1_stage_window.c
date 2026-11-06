#include "gk1_stage_window.h"
#include <string.h>
#define R(a, addr) ((a)[(addr)-GK1_RAM_CPU_BASE])

/* Observed as VM DATA pointers, NEVER native 6502 PC addresses. */
static const uint16_t VM_EXPECTED[4][3]={
    {0x945b,0xd917,0xcb55}, /* friendly-occupied, 40+40 intervention */
    {0x945b,0xe6c6,0xcb53}, /* enemy-occupied */
    {0xcb5f,0xe6c4,0x8ff1}, /* Mobility 2 */
    {0xcb8f,0xcc2b,0xcb60}  /* Mobility 4 */
};
static int prov_ok(Gk1StageCase kind,const Gk1StageProvenance *s) {
    if(!s || kind<GK1_STAGE_JOIN || kind>GK1_STAGE_EXACT_MOBILITY ||
       !s->rom_sha256 || !s->checkpoint_sha256 ||
       strcmp(s->rom_sha256,GK1_SOURCE_ROM_SHA256) ||
       strcmp(s->checkpoint_sha256,GK1_SOURCE_CHECKPOINT_SHA256) ||
       s->controller_capture_attested!=1 || s->intervention_disclosed!=1 ||
       s->phases_attested!=1) return 0;
    const uint16_t *vm=VM_EXPECTED[kind-1];
    return s->selected_vm_pc==vm[0] && s->attempt_vm_pc==vm[1] && s->idle_vm_pc==vm[2];
}
static int read_units(const uint8_t *buf,Gk1NativeUnit *a,Gk1NativeUnit *b) {
    return !gk1_native_read_unit(buf,GK1_RAM_BYTES,0,a) &&
           !gk1_native_read_unit(buf,GK1_RAM_BYTES,1,b);
}
static int terrain_preserved(const uint8_t *a,const uint8_t *b) {
    for(unsigned i=0;i<GK1_TILE_HEIGHT*GK1_TILE_WIDTH;++i)
        if(R(a,GK1_TILE_BASE+i)!=R(b,GK1_TILE_BASE+i)) return 0;
    return 1;
}
static int other_units_preserved(const uint8_t *a,const uint8_t *b) {
    /* Bytes are opaque. Do not normalize zero-soldier or ownership fields. */
    for(unsigned i=2;i<GK1_UNIT_COUNT;i++)
        if(memcmp(a+(GK1_UNIT_BASE-GK1_RAM_CPU_BASE)+GK1_UNIT_STRIDE*i,
                  b+(GK1_UNIT_BASE-GK1_RAM_CPU_BASE)+GK1_UNIT_STRIDE*i,
                  GK1_UNIT_STRIDE)!=0) return 0;
    return 1;
}
static int raw_full_equal(const uint8_t *a,const uint8_t *b,unsigned slot) {
    const size_t base=(size_t)(GK1_UNIT_BASE-GK1_RAM_CPU_BASE)+GK1_UNIT_STRIDE*slot;
    return memcmp(a+base,b+base,GK1_UNIT_STRIDE)==0;
}
Gk1StageWindowVerdict gk1_stage_check(
    Gk1StageCase kind,const Gk1StageProvenance *source,
    const uint8_t *selected,size_t ns,const uint8_t *attempt,size_t na,
    const uint8_t *idle,size_t ni,Gk1StageWindowResult *out) {
    if(!out || !selected || !attempt || !idle || ns!=GK1_RAM_BYTES ||
       na!=GK1_RAM_BYTES || ni!=GK1_RAM_BYTES)
        return GK1_WINDOW_INVALID;
    if(!prov_ok(kind,source)) return GK1_WINDOW_UNVERIFIED;
    Gk1NativeUnit m0,m1,m2,p0,p1,p2;
    if(!read_units(selected,&m0,&p0)||!read_units(attempt,&m1,&p1)||
       !read_units(idle,&m2,&p2))return GK1_WINDOW_INVALID;
    if(!terrain_preserved(selected,attempt) || !terrain_preserved(attempt,idle) ||
       !other_units_preserved(selected,attempt) || !other_units_preserved(attempt,idle))
        return GK1_WINDOW_MISMATCH;
    uint16_t w0=0,w1=0,w2=0;
    if(gk1_native_scratch_word(selected,ns,&w0) ||
       gk1_native_scratch_word(attempt,na,&w1) ||
       gk1_native_scratch_word(idle,ni,&w2)) return GK1_WINDOW_INVALID;
    if(m0.soldiers!=40 || m0.x!=7 || m0.y!=7 || p0.soldiers!=40 || w0!=0 ||
       R(selected,0x61d4)!=3 || m1.soldiers!=m2.soldiers ||
       m1.x!=m2.x || m1.y!=m2.y || m1.mobility!=m2.mobility ||
       p1.soldiers!=p2.soldiers || p1.x!=p2.x || p1.y!=p2.y || w1!=w2)
        return GK1_WINDOW_MISMATCH;
    int match=0;
    Gk1CasualtyResult casualty;
    if(gk1_casualties(40,3,GK1_SEED_INPUT,&casualty)!=0 ||
       casualty.after_random!=34) return GK1_WINDOW_INVALID;
    switch(kind) {
    case GK1_STAGE_JOIN:
        /* 74 = native forest 40->34 + 40; numerical decomposition is
         * consistent, not proof of join instruction order or generality. */
        match=m0.mobility==10 && p0.x==8 && p0.y==7 &&
              m1.soldiers==casualty.after_random+p0.soldiers &&
              m1.soldiers==74 && m1.x==8 && m1.y==7 && m1.mobility==10 &&
              p1.soldiers==0 && p1.x==8 && p1.y==7 && w1==2 &&
              R(selected,0x614c)==14 && R(attempt,0x614c)==14 &&
              R(idle,0x614c)==22 && R(selected,0x614e)==5 &&
              R(attempt,0x614e)==3 && R(idle,0x614e)==3;
        break;
    case GK1_STAGE_ENEMY_BLOCK:
    case GK1_STAGE_LOW_MOBILITY_BLOCK:
        match=m0.mobility==(kind==GK1_STAGE_ENEMY_BLOCK?10:2) &&
              raw_full_equal(selected,attempt,0) &&
              raw_full_equal(selected,attempt,1) &&
              m1.soldiers==40 && m1.x==7 && m1.y==7 &&
              p1.soldiers==40 && w1==0 &&
              R(attempt,0x614e)==5;
        break;
    case GK1_STAGE_EXACT_MOBILITY:
        match=m0.mobility==4 && p0.x==6 && p0.y==9 &&
              m1.soldiers==casualty.after_random &&
              m1.x==8 && m1.y==7 && m1.mobility==10 &&
              raw_full_equal(selected,attempt,1) &&
              w1==1 && R(attempt,0x614e)==5;
        break;
    default: return GK1_WINDOW_UNVERIFIED;
    }
    if(!match) return GK1_WINDOW_MISMATCH;
    Gk1StageWindowResult v={0};
    v.moved=(m0.x!=m1.x || m0.y!=m1.y);
    v.join_like=(kind==GK1_STAGE_JOIN);
    v.idle_scratch_transition=R(attempt,0x614c)!=R(idle,0x614c);
    v.selected_soldiers=m0.soldiers;v.attempt_soldiers=m1.soldiers;v.idle_soldiers=m2.soldiers;
    v.selected_partner=p0.soldiers;v.attempt_partner=p1.soldiers;v.idle_partner=p2.soldiers;
    v.selected_completion=w0;v.attempt_completion=w1;v.idle_completion=w2;
    v.selected_614c=R(selected,0x614c);v.attempt_614c=R(attempt,0x614c);
    v.idle_614c=R(idle,0x614c);v.selected_614e=R(selected,0x614e);
    v.attempt_614e=R(attempt,0x614e);v.idle_614e=R(idle,0x614e);
    v.percent_loss_and_random_expected=(uint8_t)casualty.after_random;
    *out=v;
    return GK1_WINDOW_BOUNDED_MATCH;
}
