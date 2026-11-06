#include "gk1_battle_session.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static unsigned checks=0;
#define CHECK(x) do { checks++; if(!(x)) { fprintf(stderr,"FAIL line %u: %s\n",(unsigned)__LINE__,#x);return 1; } } while(0)
static void fix_crc(uint8_t *b) {
    uint32_t c=UINT32_C(0xffffffff);
    for(unsigned i=0;i<44;i++) {c^=b[i];for(unsigned k=0;k<8;k++)c=(c>>1)^((c&1)?UINT32_C(0xedb88320):0);}
    c=~c; for(unsigned i=0;i<4;i++)b[44+i]=(uint8_t)(c>>(8u*i));
}
static int run(void) {
    Gk1BattleSession s={0}; Gk1MoveResult r=GK1_UNVERIFIED;
    uint8_t bytes[GK1_SESSION_SAVE_BYTES]={0},backup[GK1_SESSION_SAVE_BYTES];
    /* Forest, optional completion B, then reconstruct from on-disk wire format. */
    CHECK(gk1_session_open_fixture(10,3,GK1_OCC_EMPTY,&s)==GK1_SESSION_OK);
    CHECK(s.battle.mover.soldiers==40 && s.battle.mover.x==7 && s.battle.mover.y==7);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes)==GK1_SESSION_OK);
    CHECK(gk1_session_move(&s,8,7,&r)==GK1_SESSION_OK && r==GK1_STEP_PENDING);
    CHECK(s.battle.mover.soldiers==34 && s.battle.mover.mobility==6);
    CHECK(s.battle.has_pending_step==1 && s.battle.completion_word==0);
    CHECK(gk1_session_finish(&s,&r)==GK1_SESSION_OK && r==GK1_FINISHED);
    CHECK(s.battle.mover.mobility==10 && s.battle.completion_word==1);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes)==GK1_SESSION_OK);
    Gk1BattleSession recovered={0};
    CHECK(gk1_session_decode(bytes,sizeof bytes,&recovered)==GK1_SESSION_OK);
    CHECK(recovered.events==2 && recovered.battle.mover.soldiers==34);
    CHECK(recovered.battle.completion_word==1 && recovered.battle.phase==GK1_PHASE_ORDERS);
    CHECK(gk1_session_finish(&recovered,&r)==GK1_SESSION_UNSUPPORTED);
    CHECK(gk1_session_move(&recovered,8,7,&r)==GK1_SESSION_UNSUPPORTED);
    CHECK(gk1_session_encode(&recovered,backup,sizeof backup)==GK1_SESSION_OK);
    CHECK(memcmp(bytes,backup,sizeof bytes)==0); /* deterministic canonical bytes */

    /* Exact native bounded source-profile interventions, not generalized paths. */
    CHECK(gk1_session_open_fixture(4,3,GK1_OCC_EMPTY,&s)==GK1_SESSION_OK);
    CHECK(gk1_session_move(&s,8,7,&r)==GK1_SESSION_OK && r==GK1_FINISHED);
    CHECK(s.battle.mover.soldiers==34 && s.battle.mover.mobility==10);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes)==GK1_SESSION_OK);
    CHECK(gk1_session_decode(bytes,sizeof bytes,&recovered)==GK1_SESSION_OK);
    CHECK(recovered.battle.completion_word==1);
    CHECK(gk1_session_open_fixture(2,3,GK1_OCC_EMPTY,&s)==GK1_SESSION_OK);
    CHECK(gk1_session_move(&s,8,7,&r)==GK1_SESSION_OK && r==GK1_REJECTED);
    CHECK(s.battle.mover.soldiers==40 && s.battle.completion_word==0);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes)==GK1_SESSION_OK);
    CHECK(gk1_session_decode(bytes,sizeof bytes,&recovered)==GK1_SESSION_OK);
    CHECK(recovered.battle.mover.mobility==2);
    CHECK(gk1_session_open_fixture(10,3,GK1_OCC_ENEMY_SLOT10,&s)==GK1_SESSION_OK);
    CHECK(gk1_session_move(&s,8,7,&r)==GK1_SESSION_OK && r==GK1_REJECTED);
    CHECK(s.battle.rng48==GK1_SEED_INPUT && s.battle.completion_word==0);
    CHECK(gk1_session_open_fixture(10,3,GK1_OCC_FRIEND_SLOT1,&s)==GK1_SESSION_OK);
    CHECK(gk1_session_move(&s,8,7,&r)==GK1_SESSION_OK && r==GK1_FINISHED);
    CHECK(s.battle.mover.soldiers==74 && s.battle.friendly_slot1.soldiers==0);
    CHECK(s.battle.completion_word==2);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes)==GK1_SESSION_OK);
    CHECK(gk1_session_decode(bytes,sizeof bytes,&recovered)==GK1_SESSION_OK);
    CHECK(recovered.battle.mover.soldiers==74 && recovered.battle.friendly_slot1.soldiers==0);
    CHECK(gk1_session_open_fixture(10,0,GK1_OCC_EMPTY,&s)==GK1_SESSION_OK);
    CHECK(gk1_session_move(&s,8,7,&r)==GK1_SESSION_OK && r==GK1_FINISHED);
    CHECK(s.battle.mover.soldiers==32 && s.battle.completion_word==1);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes)==GK1_SESSION_OK);
    CHECK(gk1_session_decode(bytes,sizeof bytes,&recovered)==GK1_SESSION_OK);

    /* Hostile codec and input controls including valid-CRC forgery. */
    Gk1BattleSession sentinel;memset(&sentinel,0xA5,sizeof sentinel);
    Gk1BattleSession before=sentinel;
    CHECK(gk1_session_decode(NULL,sizeof bytes,&sentinel)==GK1_SESSION_BAD);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    CHECK(gk1_session_decode(bytes,sizeof bytes-1,&sentinel)==GK1_SESSION_BAD);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    memcpy(backup,bytes,sizeof backup);backup[10]^=0x80;
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    memcpy(backup,bytes,sizeof backup);backup[8]=2;fix_crc(backup);
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    memcpy(backup,bytes,sizeof backup);backup[9]=2;fix_crc(backup);
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    memcpy(backup,bytes,sizeof backup);backup[13]=2;fix_crc(backup);
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    memcpy(backup,bytes,sizeof backup);backup[14]=9;fix_crc(backup);
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    memcpy(backup,bytes,sizeof backup);backup[17]=0;fix_crc(backup);
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    memcpy(backup,bytes,sizeof backup);backup[18]^=1;fix_crc(backup);
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    memcpy(backup,bytes,sizeof backup);backup[22]=1;fix_crc(backup);
    CHECK(gk1_session_decode(backup,sizeof backup,&sentinel)==GK1_SESSION_UNSUPPORTED);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    CHECK(gk1_session_open_fixture(10,7,GK1_OCC_EMPTY,&sentinel)==GK1_SESSION_UNSUPPORTED);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    CHECK(gk1_session_open_fixture(10,0,GK1_OCC_FRIEND_SLOT1,&sentinel)==GK1_SESSION_UNSUPPORTED);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    CHECK(gk1_session_open_fixture(9,3,GK1_OCC_EMPTY,&sentinel)==GK1_SESSION_BAD);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    CHECK(gk1_session_open_fixture(10,3,(Gk1Occupancy)77,&sentinel)==GK1_SESSION_BAD);
    CHECK(memcmp(&sentinel,&before,sizeof before)==0);
    CHECK(gk1_session_open_fixture(10,3,GK1_OCC_EMPTY,&s)==GK1_SESSION_OK);
    Gk1BattleSession original=s;
    CHECK(gk1_session_move(&s,9,7,&r)==GK1_SESSION_UNSUPPORTED);
    CHECK(memcmp(&original,&s,sizeof s)==0);
    CHECK(gk1_session_finish(&s,&r)==GK1_SESSION_UNSUPPORTED);
    CHECK(memcmp(&original,&s,sizeof s)==0);
    CHECK(gk1_session_move(&s,8,7,&r)==GK1_SESSION_OK);
    s.battle.mover.soldiers++;
    memset(backup,0x5A,sizeof backup);memcpy(bytes,backup,sizeof backup);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes)==GK1_SESSION_UNSUPPORTED);
    CHECK(memcmp(bytes,backup,sizeof bytes)==0);
    CHECK(gk1_session_encode(NULL,bytes,sizeof bytes)==GK1_SESSION_BAD);
    CHECK(gk1_session_encode(&s,bytes,sizeof bytes-1)==GK1_SESSION_BAD);
    CHECK(gk1_session_decode(bytes,sizeof bytes,&sentinel)==GK1_SESSION_UNSUPPORTED);
    printf("PASS %u assertions (bounded modified-ROM replay/session/wire-contract only)\n",checks);
    return 0;
}
int main(void){return run();}
