/* Developer-only example of the portable tactical-state codec.
 * The generated file is explicitly synthetic and cannot run an NES game.
 * Does not expose or bundle ROM, SRAM, artwork, or sounds.
 */
#include "gk1_battle_snapshot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int parse_hex(const char *str,uint8_t out[32]) {
    if(!str || strlen(str)!=64) return -1;
    for(unsigned i=0;i<32;i++) {
        char c=str[i*2],d=str[i*2+1];
        int hi=(c>='0'&&c<='9')?c-'0':(c>='a'&&c<='f')?c-'a'+10:(c>='A'&&c<='F')?c-'A'+10:-1;
        int lo=(d>='0'&&d<='9')?d-'0':(d>='a'&&d<='f')?d-'a'+10:(d>='A'&&d<='F')?d-'A'+10:-1;
        if(hi<0 || lo<0) return -1;
        out[i]=(uint8_t)((unsigned)hi*16u+(unsigned)lo);
    }
    return 0;
}
static int profile_arg(const char *s) {
    if(!strcmp(s,"stock")) return GK1_SNAP_STOCK_USA;
    if(!strcmp(s,"combined")) return GK1_SNAP_APPROVED_COMBINED_MOD;
    return -1;
}
static int write_file(const char *path,const uint8_t *b,size_t n) {
    FILE *f=fopen(path,"wbx");
    if(!f) {perror("refusing to overwrite or open");return -1;}
    int ok=fwrite(b,1,n,f)==n && fflush(f)==0;
    if(fclose(f)!=0) ok=0;
    if(!ok) {remove(path);return -1;}
    return 0;
}
static int read_file(const char *path,uint8_t *b,size_t n) {
    FILE *f=fopen(path,"rb");if(!f) return -1;
    int ok=fread(b,1,n,f)==n && fgetc(f)==EOF && !ferror(f);
    if(fclose(f)!=0) ok=0;
    return ok?0:-1;
}
int main(int argc,char **argv) {
    if(argc!=5) {
        fprintf(stderr,"usage: %s synthetic-write|inspect PATH stock|combined ROM_SHA256_HEX\n",argv[0]);
        return 2;
    }
    uint8_t id[32];int profile=profile_arg(argv[3]);
    if(profile<0 || parse_hex(argv[4],id)!=0) {fputs("bad identity argument\n",stderr);return 2;}
    uint8_t b[GK1_SNAPSHOT_SIZE];
    if(!strcmp(argv[1],"synthetic-write")) {
        /* Create an obvious non-game synthetic test fixture. The supplied
           hash is an identity label ONLY; not a proof of actual ROM bytes. */
        uint8_t ram[GK1_RAM_BYTES]={0};
        ram[0x60]=40;ram[0x62]=7;ram[0x63]=7;ram[0x65]=10;
        ram[0x13e]=2;
        for(unsigned y=0;y<10;y++) for(unsigned x=0;x<16;x++)
            ram[0x15c+y*16+x]=(uint8_t)((x+y)%5);
        Gk1BattleSnapshot metadata={0},view;
        metadata.profile=(uint8_t)profile;
        metadata.phase=GK1_PHASE_UNDETERMINED;
        memcpy(metadata.rom_sha256,id,32);
        if(gk1_snapshot_capture(ram,sizeof ram,&metadata,&view) ||
           gk1_snapshot_encode(&view,b,sizeof b) ||
           write_file(argv[2],b,sizeof b)) {
            fputs("synthetic fixture write FAILED\n",stderr);return 1;
        }
        puts("SYNTHETIC battle research snapshot written (440 bytes). Not a playable save.");
        return 0;
    }
    if(!strcmp(argv[1],"inspect")) {
        Gk1BattleSnapshot view;
        if(read_file(argv[2],b,sizeof b)!=0 ||
           gk1_snapshot_decode_for_rom(b,sizeof b,(uint8_t)profile,id,&view)!=0) {
            fputs("snapshot verification FAILED\n",stderr);return 1;
        }
        printf("verified profile=%u phase=%u scratch_word=%u unit0_soldiers=%u tile(8,7)=%u\n",
              (unsigned)view.profile,(unsigned)view.phase,(unsigned)view.scratch_word,
              (unsigned)view.units[0][0]+((unsigned)view.units[0][1]<<8),
              (unsigned)view.tiles[7*16+8]);
        puts("Partial tactical research snapshot only; not full NES or Windows save.");
        return 0;
    }
    fputs("unknown action\n",stderr);return 2;
}
