/* EXEC0044: identity, phase, tile and zero-unit hostile regression tests.
 * Synthetic memory ONLY; it is not a native NES observation.
 */
#include "gk1_snapshot_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
static unsigned checks;
#define CHECK(v) do{checks++;if(!(v)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#v);exit(1);}}while(0)
static void fixture(Gk1BattleSnapshot *s, uint8_t hash[32]) {
    memset(s,0,sizeof *s);for(unsigned i=0;i<32;i++)hash[i]=(uint8_t)(i+1);
    memcpy(s->rom_sha256,hash,32);s->profile=GK1_SNAP_APPROVED_COMBINED_MOD;
    s->phase=GK1_PHASE_DESTINATION_SELECTED;s->scratch_word=2;
    s->flags=GK1_SNAPSHOT_HAS_VM|GK1_SNAPSHOT_HAS_RNG;
    s->vm_bank=3;s->vm_source_pointer=0xf1c8;
    for(unsigned i=0;i<6;i++)s->rng48[i]=(uint8_t)(i+10);
    for(unsigned i=0;i<160;i++)s->tiles[i]=(uint8_t)(i+70);
    for(unsigned i=0;i<20;i++){
       s->units[i][0]=(uint8_t)(i+1);
       s->units[i][1]=0;
       s->units[i][2]=(uint8_t)(i%16);
       s->units[i][3]=(uint8_t)(i/16);
    }
    s->units[0][2]=7;s->units[0][3]=7;
    s->units[1][0]=0;s->units[1][2]=7;s->units[1][3]=7;
    s->units[2][2]=16;s->units[2][3]=0; /* Out-of-bounds record retained. */
}
int main(void) {
    Gk1BattleSnapshot s;uint8_t hash[32],other[32],bytes[440],mut[440];
    fixture(&s,hash);memcpy(other,hash,32);other[31]^=1;
    CHECK(gk1_snapshot_encode(&s,bytes,440)==0);
    Gk1BattleView v;memset(&v,0xa9,sizeof v);
    CHECK(gk1_view_decode(bytes,439,s.profile,hash,&v)!=0);
    CHECK(((unsigned char*)&v)[0]==0xa9);
    CHECK(gk1_view_decode(bytes,440,s.profile,other,&v)!=0);
    CHECK(((unsigned char*)&v)[0]==0xa9);
    CHECK(gk1_view_decode(bytes,440,GK1_SNAP_STOCK_USA,hash,&v)!=0);
    CHECK(((unsigned char*)&v)[0]==0xa9);
    CHECK(gk1_view_decode(bytes,440,s.profile,hash,&v)==0);
    CHECK(v.capture.phase==GK1_PHASE_DESTINATION_SELECTED);
    CHECK(v.valid_positions==19 && v.out_of_bounds_positions==1);
    CHECK(v.units_with_soldiers==19 && v.empty_record_positions==1);
    CHECK((v.recorded_mask[7+7*16]&3u)==3u);
    CHECK((v.populated_mask[7+7*16]&3u)==1u);
    for(unsigned y=0;y<10;y++)for(unsigned x=0;x<16;x++){
        uint8_t raw=255;uint32_t rec=0,pop=0;
        CHECK(gk1_view_tile(&v,x,y,&raw,&rec,&pop)==0);
        CHECK(raw==s.tiles[y*16+x]);
        CHECK((rec&pop)==pop);
    }
    uint8_t raw=33;uint32_t rec=42,pop=55;
    CHECK(gk1_view_tile(&v,16,0,&raw,&rec,&pop)!=0);
    CHECK(raw==33&&rec==42&&pop==55);
    for(unsigned i=0;i<440;i++){
        memcpy(mut,bytes,440);mut[i]^=0x40;
        Gk1BattleView guard;memset(&guard,0xb7,sizeof guard);
        CHECK(gk1_view_decode(mut,440,s.profile,hash,&guard)!=0);
        CHECK(((unsigned char*)&guard)[0]==0xb7);
    }
    uint8_t parsed[32];memset(parsed,0xa5,32);
    CHECK(gk1_view_parse_hash("12",parsed)!=0&&parsed[0]==0xa5);
    CHECK(gk1_view_parse_hash("000000000000000000000000000000000000000000000000000000000000000G",parsed)!=0);
    CHECK(gk1_view_parse_hash("00000000000000000000000000000000000000000000000000000000000000000",parsed)!=0);
    const char *h="0123456789abcdef0123456789abcdef0123456789abcdef0123456789ABCDEF";
    CHECK(gk1_view_parse_hash(h,parsed)==0 && parsed[0]==0x01 && parsed[31]==0xef);
    uint8_t profile=77;
    CHECK(gk1_view_parse_profile("COMBINED",&profile)!=0&&profile==77);
    CHECK(gk1_view_parse_profile("combined",&profile)==0&&profile==GK1_SNAP_APPROVED_COMBINED_MOD);
    CHECK(gk1_view_parse_profile("stock",&profile)==0&&profile==GK1_SNAP_STOCK_USA);
    /* Disk integration: validated file exact size, phase/hash, corruption, preserve-on-error. */
    const char *file="gk1_EXEC0044_test_only.gk1b";
    FILE *f=fopen(file,"wb");CHECK(f!=NULL);
    CHECK(fwrite(bytes,1,440,f)==440);CHECK(fclose(f)==0);
    CHECK(gk1_view_load_file(file,s.profile,hash,&v)==0);
    CHECK(v.capture.scratch_word==2);
    CHECK(gk1_view_load_file(file,s.profile,other,&v)!=0);
    CHECK(v.capture.scratch_word==2);
    f=fopen(file,"ab");CHECK(f!=NULL);CHECK(fputc(0,f)!=EOF);CHECK(fclose(f)==0);
    CHECK(gk1_view_load_file(file,s.profile,hash,&v)!=0);
    CHECK(v.capture.scratch_word==2);
    CHECK(remove(file)==0);
    CHECK(gk1_view_load_file(file,s.profile,hash,&v)!=0);
    CHECK(v.capture.scratch_word==2);
    printf("EXEC0044 snapshot-view strict assertions: %u PASS (synthetic only)\n",checks);
    return 0;
}
