#include "gk1_snapshot_view.h"
#include <stdio.h>
#include <string.h>

static int digit(char c) {
    if(c>='0'&&c<='9')return c-'0';
    if(c>='a'&&c<='f')return c-'a'+10;
    if(c>='A'&&c<='F')return c-'A'+10;
    return -1;
}
int gk1_view_parse_hash(const char *hex, uint8_t out[32]) {
    if(!hex||!out)return -1;
    /* Never strlen() an untrusted unbounded string: inspect exactly 64+NUL. */
    uint8_t tmp[32];
    for(size_t i=0;i<64;i++)if(!hex[i]||digit(hex[i])<0)return -1;
    if(hex[64]!='\0')return -1;
    for(size_t i=0;i<32;i++)tmp[i]=(uint8_t)((digit(hex[2*i])<<4)|digit(hex[2*i+1]));
    memcpy(out,tmp,sizeof tmp);
    return 0;
}
int gk1_view_parse_profile(const char *name,uint8_t *out) {
    if(!name||!out)return -1;
    if(strcmp(name,"stock")==0){*out=GK1_SNAP_STOCK_USA;return 0;}
    if(strcmp(name,"combined")==0){*out=GK1_SNAP_APPROVED_COMBINED_MOD;return 0;}
    return -1;
}
int gk1_view_decode(const uint8_t *encoded,size_t len,uint8_t profile,
                    const uint8_t hash[32],Gk1BattleView *out) {
    if(!encoded||!out||!hash)return -1;
    Gk1BattleSnapshot snapshot;
    if(gk1_snapshot_decode_for_rom(encoded,len,profile,hash,&snapshot)!=0)return -1;
    Gk1BattleView temp={0};
    temp.capture=snapshot;
    for(unsigned i=0;i<GK1_UNIT_COUNT;i++){
        const uint8_t *u=snapshot.units[i];
        const unsigned x=u[2],y=u[3];
        const unsigned present=(u[0]!=0||u[1]!=0);
        if(present)temp.units_with_soldiers++;
        if(x>=GK1_TILE_WIDTH||y>=GK1_TILE_HEIGHT){
            temp.out_of_bounds_positions++;continue;
        }
        const unsigned at=y*GK1_TILE_WIDTH+x;
        const uint32_t bit=UINT32_C(1)<<i;
        temp.valid_positions++;
        temp.recorded_mask[at]|=bit;
        if(present)temp.populated_mask[at]|=bit;
        else temp.empty_record_positions++;
    }
    *out=temp;return 0;
}
int gk1_view_load_file(const char *path,uint8_t profile,const uint8_t hash[32],
                       Gk1BattleView *out) {
    if(!path||!*path||!hash||!out)return -1;
    FILE *f=fopen(path,"rb");
    if(!f)return -1;
    uint8_t bytes[GK1_SNAPSHOT_SIZE+1];
    const size_t size=fread(bytes,1,sizeof bytes,f);
    const int io_error=ferror(f);
    const int close_error=fclose(f);
    if(io_error||close_error||size!=GK1_SNAPSHOT_SIZE)return -1;
    return gk1_view_decode(bytes,GK1_SNAPSHOT_SIZE,profile,hash,out);
}
int gk1_view_tile(const Gk1BattleView *v,unsigned x,unsigned y,
                  uint8_t *raw,uint32_t *recorded,uint32_t *populated) {
    if(!v||!raw||!recorded||!populated||x>=GK1_TILE_WIDTH||y>=GK1_TILE_HEIGHT)return -1;
    unsigned i=y*GK1_TILE_WIDTH+x;
    *raw=v->capture.tiles[i];*recorded=v->recorded_mask[i];
    *populated=v->populated_mask[i];return 0;
}
