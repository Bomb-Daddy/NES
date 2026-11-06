/* Real native CLI executable exercising deterministic sourced tactical
 * integration. This is a developer replay command-line, not Xbox game UI.
 */
#include "gk1_battle_session.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv) {
    if(argc<2 || argc>3) {fputs("usage: gk1_battle_replay forest|low|exact|friendly|enemy|terrain0 [new_save_file] OR load save_file\n",stderr);return 2;}
    if(argc==3 && !strcmp(argv[1],"load")) {
        FILE *f=fopen(argv[2],"rb"); if(!f) return 7;
        uint8_t payload[GK1_SESSION_SAVE_BYTES];
        const size_t n=fread(payload,1,sizeof payload,f);
        const int extra=fgetc(f), error=ferror(f), closed=fclose(f);
        if(n!=sizeof payload || extra!=EOF || error!=0 || closed!=0) return 8;
        Gk1BattleSession restored;
        if(gk1_session_decode(payload,n,&restored)!=GK1_SESSION_OK) return 9;
        printf("REPLAY_LOAD_VERIFIED bytes=%u events=%u soldiers=%u completed_word=%u\n",
            (unsigned)n,(unsigned)restored.events,(unsigned)restored.battle.mover.soldiers,
            (unsigned)restored.battle.completion_word);
        return 0;
    }
    uint8_t mob=10,t=3;Gk1Occupancy occ=GK1_OCC_EMPTY;
    if(!strcmp(argv[1],"low"))mob=2;
    else if(!strcmp(argv[1],"exact"))mob=4;
    else if(!strcmp(argv[1],"friendly"))occ=GK1_OCC_FRIEND_SLOT1;
    else if(!strcmp(argv[1],"enemy"))occ=GK1_OCC_ENEMY_SLOT10;
    else if(!strcmp(argv[1],"terrain0"))t=0;
    else if(strcmp(argv[1],"forest")) {fputs("unknown fixture\n",stderr);return 2;}
    Gk1BattleSession s={0};Gk1MoveResult r;
    if(gk1_session_open_fixture(mob,t,occ,&s)!=GK1_SESSION_OK) return 3;
    if(gk1_session_move(&s,8,7,&r)!=GK1_SESSION_OK) return 4;
    printf("ATTEMPT result=%d soldiers=%u partner=%u mobility=%u word=%u rng=%012llx\n",(int)r,
        (unsigned)s.battle.mover.soldiers,(unsigned)s.battle.friendly_slot1.soldiers,
        (unsigned)s.battle.mover.mobility,(unsigned)s.battle.completion_word,
        (unsigned long long)s.battle.rng48);
    if(r==GK1_STEP_PENDING) {
        if(gk1_session_finish(&s,&r)!=GK1_SESSION_OK) return 5;
        printf("B_FINISH result=%d soldiers=%u mobility=%u word=%u\n",(int)r,
            (unsigned)s.battle.mover.soldiers,(unsigned)s.battle.mover.mobility,(unsigned)s.battle.completion_word);
    }
    uint8_t b[GK1_SESSION_SAVE_BYTES];Gk1BattleSession restored;
    if(gk1_session_encode(&s,b,sizeof b)!=GK1_SESSION_OK ||
       gk1_session_decode(b,sizeof b,&restored)!=GK1_SESSION_OK) return 6;
    if(argc==3) {
        FILE *exists=fopen(argv[2],"rb");
        if(exists) {fclose(exists);fputs("refusing to overwrite existing save\n",stderr);return 7;}
        FILE *f=fopen(argv[2],"wb");if(!f) return 7;
        const size_t written=fwrite(b,1,sizeof b,f);
        const int closed=fclose(f);
        if(written!=sizeof b || closed!=0) return 8;
    }
    printf("REPLAY_SAVE_VERIFIED bytes=%u events=%u completed_word=%u\n",
        (unsigned)sizeof b,(unsigned)restored.events,(unsigned)restored.battle.completion_word);
    return 0;
}
