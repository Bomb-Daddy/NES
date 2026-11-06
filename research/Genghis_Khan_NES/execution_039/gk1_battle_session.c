#include "gk1_battle_session.h"
#include <string.h>

static uint32_t read32(const uint8_t *b) {
    return (uint32_t)b[0] | ((uint32_t)b[1]<<8) | ((uint32_t)b[2]<<16) | ((uint32_t)b[3]<<24);
}
static void write32(uint8_t *b,uint32_t n) {
    for(unsigned i=0;i<4;i++) b[i]=(uint8_t)(n>>(8u*i));
}
/* Explicit little-endian polynomial CRC32 (ISO-HDLC). */
static uint32_t crc32(const uint8_t *b,size_t n) {
    uint32_t c=UINT32_C(0xffffffff);
    for(size_t i=0;i<n;i++) { c^=b[i]; for(unsigned k=0;k<8;k++) c=(c>>1) ^ ((c&1u)?UINT32_C(0xedb88320):0u); }
    return ~c;
}
/* Stable fingerprint is based on individual fields, never C struct padding. */
static uint32_t state_fingerprint(const Gk1BattleState *s) {
    uint8_t b[57]={0}; size_t p=0;
#define W8(v) do { b[p++]=(uint8_t)(v); } while(0)
#define W16(v) do { uint16_t n_=(uint16_t)(v); W8(n_); W8(n_>>8); } while(0)
#define W32(v) do { uint32_t n_=(uint32_t)(v); for(unsigned j_=0;j_<4;j_++) W8(n_>>(8u*j_)); } while(0)
#define UNIT(u) do { W16((u).soldiers); W8((u).x); W8((u).y); W8((u).mobility); } while(0)
    UNIT(s->mover); UNIT(s->friendly_slot1); UNIT(s->enemy_slot10);
    for(unsigned j=0;j<6;j++) W8(s->rng48>>(8u*j));
    W16(s->completion_word); W16(s->gold); W16(s->food);
    W8(s->tile_at_destination); W8(s->phase); W8(s->has_pending_step);
    W8(s->combined_mod_profile); W8(s->exact_checkpoint_unit_profile);
    W32(s->other_units_guard);
#undef W8
#undef W16
#undef W32
#undef UNIT
    return crc32(b,p);
}

Gk1SessionStatus gk1_session_open_fixture(uint8_t mob,uint8_t terrain,
                               Gk1Occupancy occ,Gk1BattleSession *out) {
    if(!out || (mob!=2 && mob!=4 && mob!=10)) return GK1_SESSION_BAD;
    /* Exact controlled experiments only. No assumptions for other unit types,
     * terrain values, positions, ownership, profiles or unseen states. */
    if(terrain!=3 && !(terrain==0 && mob==10 && occ==GK1_OCC_EMPTY)) return GK1_SESSION_UNSUPPORTED;
    if(occ!=GK1_OCC_EMPTY && occ!=GK1_OCC_FRIEND_SLOT1 && occ!=GK1_OCC_ENEMY_SLOT10) return GK1_SESSION_BAD;
    if(occ!=GK1_OCC_EMPTY && mob!=10) return GK1_SESSION_UNSUPPORTED;
    Gk1BattleSession s={0}; s.battle=gk1_checkpoint_fixture(mob,terrain,occ);
    s.initial_mobility=mob;s.initial_terrain=terrain;s.initial_occupant=(uint8_t)occ;
    s.first_result=-1;s.second_result=-1;
    *out=s;return GK1_SESSION_OK;
}

Gk1SessionStatus gk1_session_move(Gk1BattleSession *s,uint8_t x,uint8_t y,Gk1MoveResult *out) {
    if(!s || !out) return GK1_SESSION_BAD;
    if(s->events!=0 || x!=8 || y!=7) return GK1_SESSION_UNSUPPORTED;
    Gk1BattleState next={0}; Gk1MoveResult r=gk1_apply_observed_step(&s->battle,x,y,(Gk1Occupancy)s->initial_occupant,&next);
    if(r==GK1_UNVERIFIED) return GK1_SESSION_UNSUPPORTED;
    Gk1BattleSession copy=*s;
    copy.battle=next;copy.events=1;copy.first_x=x;copy.first_y=y;copy.first_result=(int8_t)r;
    *s=copy; *out=r; return GK1_SESSION_OK;
}
Gk1SessionStatus gk1_session_finish(Gk1BattleSession *s,Gk1MoveResult *out) {
    if(!s || !out) return GK1_SESSION_BAD;
    if(s->events!=1 || s->first_result!=GK1_STEP_PENDING) return GK1_SESSION_UNSUPPORTED;
    Gk1BattleState next={0}; Gk1MoveResult r=gk1_finish_observed_path(&s->battle,&next);
    if(r!=GK1_FINISHED) return GK1_SESSION_UNSUPPORTED;
    Gk1BattleSession copy=*s;copy.battle=next;copy.events=2;copy.second_result=(int8_t)r;
    *s=copy; *out=r; return GK1_SESSION_OK;
}

/* 0..7 magic, 8 version, 9 source profile, 10 mobility, 11 terrain,
 * 12 occupancy, 13 events, 14 x, 15 y, 16 first result, 17 second result,
 * 18..21 state CRC32, 22..43 zeros reserved, 44..47 total CRC32.
 */
static const uint8_t magic[8]={'G','K','1','R','P','L','0','1'};
Gk1SessionStatus gk1_session_encode(const Gk1BattleSession *s,uint8_t *dst,size_t cap) {
    if(!s || !dst || cap!=GK1_SESSION_SAVE_BYTES) return GK1_SESSION_BAD;
    /* Replay using the same bounded known rules and compare entire normalized
     * state by explicit field-fingerprint, not native padding. */
    Gk1BattleSession replay; Gk1MoveResult r;
    if(gk1_session_open_fixture(s->initial_mobility,s->initial_terrain,
        (Gk1Occupancy)s->initial_occupant,&replay)!=GK1_SESSION_OK) return GK1_SESSION_UNSUPPORTED;
    if(s->events>2) return GK1_SESSION_UNSUPPORTED;
    if(s->events>=1 && gk1_session_move(&replay,s->first_x,s->first_y,&r)!=GK1_SESSION_OK) return GK1_SESSION_UNSUPPORTED;
    if(s->events==2 && gk1_session_finish(&replay,&r)!=GK1_SESSION_OK) return GK1_SESSION_UNSUPPORTED;
    if(s->events!=replay.events || s->first_result!=replay.first_result ||
       s->second_result!=replay.second_result ||
       state_fingerprint(&s->battle)!=state_fingerprint(&replay.battle)) return GK1_SESSION_UNSUPPORTED;
    uint8_t b[GK1_SESSION_SAVE_BYTES]={0};
    memcpy(b,magic,sizeof magic);
    b[8]=1;b[9]=1;b[10]=s->initial_mobility;b[11]=s->initial_terrain;b[12]=s->initial_occupant;
    b[13]=s->events;b[14]=s->first_x;b[15]=s->first_y;
    b[16]=(uint8_t)s->first_result;b[17]=(uint8_t)s->second_result;
    write32(b+18,state_fingerprint(&replay.battle));
    write32(b+44,crc32(b,44));
    memcpy(dst,b,sizeof b);
    return GK1_SESSION_OK;
}
Gk1SessionStatus gk1_session_decode(const uint8_t *b,size_t n,Gk1BattleSession *out) {
    if(!b || !out || n!=GK1_SESSION_SAVE_BYTES) return GK1_SESSION_BAD;
    if(memcmp(b,magic,sizeof magic) || b[8]!=1 || b[9]!=1 || read32(b+44)!=crc32(b,44)) return GK1_SESSION_UNSUPPORTED;
    for(size_t i=22;i<44;i++) if(b[i]) return GK1_SESSION_UNSUPPORTED;
    Gk1BattleSession candidate; Gk1MoveResult r;
    if(gk1_session_open_fixture(b[10],b[11],(Gk1Occupancy)b[12],&candidate)!=GK1_SESSION_OK) return GK1_SESSION_UNSUPPORTED;
    if(b[13]>2) return GK1_SESSION_UNSUPPORTED;
    if(b[13]>=1 && gk1_session_move(&candidate,b[14],b[15],&r)!=GK1_SESSION_OK) return GK1_SESSION_UNSUPPORTED;
    if(b[13]==2 && gk1_session_finish(&candidate,&r)!=GK1_SESSION_OK) return GK1_SESSION_UNSUPPORTED;
    if(b[16]!=(uint8_t)candidate.first_result || b[17]!=(uint8_t)candidate.second_result ||
       read32(b+18)!=state_fingerprint(&candidate.battle)) return GK1_SESSION_UNSUPPORTED;
    *out=candidate;return GK1_SESSION_OK;
}
