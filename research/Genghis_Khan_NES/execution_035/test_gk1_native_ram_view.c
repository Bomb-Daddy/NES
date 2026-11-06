#include "gk1_native_ram_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static int assertions=0;
#define CHECK(expr) do {++assertions;if(!(expr)) {fprintf(stderr,"FAIL %s:%d: %s\n", __FILE__,__LINE__,#expr); exit(1);}}while(0)
static uint8_t ram[GK1_RAM_BYTES];
static void put_unit(unsigned slot,uint16_t count,uint8_t x,uint8_t y,uint8_t ty,uint8_t mp) {
    size_t i=GK1_UNIT_BASE-GK1_RAM_CPU_BASE+slot*GK1_UNIT_STRIDE;
    ram[i]=(uint8_t)count;ram[i+1]=(uint8_t)(count>>8);
    ram[i+2]=x;ram[i+3]=y;ram[i+4]=ty;ram[i+5]=mp;
}
static void init(void){
    memset(ram,0xff,sizeof ram); /* Off-map by default, no inferred unit absence. */
    for(unsigned i=0;i<GK1_UNIT_COUNT;i++)put_unit(i,0,255,255,255,255);
    put_unit(0,40,7,7,0,10);
    put_unit(1,40,8,7,2,10);
    put_unit(10,70,9,8,0,10);
    ram[GK1_SCRATCH_WORD-GK1_RAM_CPU_BASE]=0x34;
    ram[GK1_SCRATCH_WORD-GK1_RAM_CPU_BASE+1]=0x12;
    /* Synthetic terrain values, not a dumped copyrighted board. */
    ram[GK1_TILE_BASE-GK1_RAM_CPU_BASE+16*7+7]=0x82;
    ram[GK1_TILE_BASE-GK1_RAM_CPU_BASE+16*7+8]=3;
}
static void positive(void){
    init();
    uint8_t before[GK1_RAM_BYTES];memcpy(before,ram,sizeof ram);
    Gk1NativeUnit u;
    CHECK(gk1_native_read_unit(ram,sizeof ram,0,&u)==0);
    CHECK(u.soldiers==40 && u.x==7 && u.y==7 && u.type==0 && u.mobility==10);
    CHECK(memcmp(u.raw,ram+GK1_UNIT_BASE-GK1_RAM_CPU_BASE,11)==0);
    CHECK(gk1_native_read_unit(ram,sizeof ram,10,&u)==0);
    CHECK(u.soldiers==70 && u.x==9 && u.y==8);
    Gk1NativeTile t;
    CHECK(gk1_native_read_tile(ram,sizeof ram,7,7,&t)==0);
    CHECK(t.cpu_address==0x61D3 && t.raw==0x82);
    CHECK(gk1_native_read_tile(ram,sizeof ram,8,7,&t)==0);
    CHECK(t.cpu_address==0x61D4 && t.raw==3);
    CHECK(gk1_native_read_tile(ram,sizeof ram,0,0,&t)==0 && t.cpu_address==0x615C);
    CHECK(gk1_native_read_tile(ram,sizeof ram,15,9,&t)==0 && t.cpu_address==0x61FB);
    uint16_t word;
    CHECK(gk1_native_scratch_word(ram,sizeof ram,&word)==0 && word==0x1234);
    Gk1NativeOccupancy o;
    CHECK(gk1_native_occupancy(ram,sizeof ram,8,7,&o)==0);
    CHECK(o.at_position==(1u<<1) && o.with_soldiers==(1u<<1));
    CHECK(gk1_native_occupancy(ram,sizeof ram,7,7,&o)==0 && o.at_position==1);
    /* A zero-soldier slot still has a position in a captured intermediate state.
       Its physical record is reported, not interpreted as a legal blockade. */
    put_unit(2,0,8,7,0,0);
    CHECK(gk1_native_occupancy(ram,sizeof ram,8,7,&o)==0);
    CHECK(o.at_position==((1u<<1)|(1u<<2)) && o.with_soldiers==(1u<<1));
    put_unit(1,0,8,7,2,10);
    CHECK(gk1_native_occupancy(ram,sizeof ram,8,7,&o)==0 && o.with_soldiers==0);
    put_unit(1,40,8,7,2,10);put_unit(2,0,255,255,255,255);
    CHECK(memcmp(before,ram,sizeof ram)==0);
}
static void property_boundaries(void){
    init();
    Gk1NativeTile t;
    /* Every grid coordinate has one unique row-major SRAM address. */
    for(unsigned y=0;y<10;y++) for(unsigned x=0;x<16;x++) {
        const unsigned offset=GK1_TILE_BASE-GK1_RAM_CPU_BASE+16u*y+x;
        ram[offset]=(uint8_t)((x+17u*y)&255u);
        CHECK(gk1_native_read_tile(ram,sizeof ram,x,y,&t)==0);
        CHECK(t.cpu_address==0x615cu+16u*y+x);
        CHECK(t.raw==(uint8_t)((x+17u*y)&255u));
    }
    /* Every slot has an independent 11-byte record, including high-byte counts. */
    for(unsigned i=0;i<20;i++){
        put_unit(i,(uint16_t)(256u+i), (uint8_t)(i%16u), (uint8_t)(i%10u),
                 (uint8_t)(i+40u),(uint8_t)i);
    }
    for(unsigned i=0;i<20;i++) {
        Gk1NativeUnit u;
        CHECK(gk1_native_read_unit(ram,sizeof ram,i,&u)==0);
        CHECK(u.soldiers==256u+i);
        CHECK(u.x==i%16u && u.y==i%10u);
        CHECK(u.type==i+40u && u.mobility==i);
    }
    /* Input is completely read-only: no command state can mutate captures. */
    uint8_t before[GK1_RAM_BYTES];memcpy(before,ram,sizeof ram);
    Gk1NativeOccupancy o;
    CHECK(gk1_native_occupancy(ram,sizeof ram,0,0,&o)==0);
    CHECK(memcmp(before,ram,sizeof ram)==0);
    /* Raw high-bit tile IDs are never masked into purported terrain IDs. */
    ram[GK1_TILE_BASE-GK1_RAM_CPU_BASE]=0x84;
    CHECK(gk1_native_read_tile(ram,sizeof ram,0,0,&t)==0 && t.raw==0x84);
}
static void negatives(void){
    init();
    Gk1NativeUnit unit;memset(&unit,0xa5,sizeof unit);Gk1NativeUnit us=unit;
    CHECK(gk1_native_read_unit(NULL,sizeof ram,0,&unit)==-1);
    CHECK(gk1_native_read_unit(ram,sizeof ram-1,0,&unit)==-1);
    CHECK(gk1_native_read_unit(ram,sizeof ram,20,&unit)==-1);
    CHECK(memcmp(&unit,&us,sizeof unit)==0);
    Gk1NativeTile t={0x77,0xabcd},ts=t;
    CHECK(gk1_native_read_tile(ram,sizeof ram,16,0,&t)==-1);
    CHECK(gk1_native_read_tile(ram,sizeof ram,0,10,&t)==-1);
    CHECK(gk1_native_read_tile(ram,sizeof ram-1,0,0,&t)==-1);
    CHECK(gk1_native_read_tile(NULL,sizeof ram,0,0,&t)==-1);
    CHECK(memcmp(&t,&ts,sizeof t)==0);
    uint16_t w=0x5432;
    CHECK(gk1_native_scratch_word(NULL,sizeof ram,&w)==-1 && w==0x5432);
    Gk1NativeOccupancy o={0x12345678u,0x87654321u}, os=o;
    CHECK(gk1_native_occupancy(ram,sizeof ram,16,0,&o)==-1);
    CHECK(gk1_native_occupancy(ram,sizeof ram,0,10,&o)==-1);
    CHECK(gk1_native_occupancy(ram,sizeof ram-1,8,7,&o)==-1);
    CHECK(gk1_native_occupancy(NULL,sizeof ram,8,7,&o)==-1);
    CHECK(memcmp(&o,&os,sizeof o)==0);
    CHECK(gk1_native_read_unit(ram,sizeof ram,0,NULL)==-1);
    CHECK(gk1_native_read_tile(ram,sizeof ram,0,0,NULL)==-1);
    CHECK(gk1_native_scratch_word(ram,sizeof ram,NULL)==-1);
    CHECK(gk1_native_occupancy(ram,sizeof ram,0,0,NULL)==-1);
}
int main(void){positive();property_boundaries();negatives();printf("EXEC0035 SRAM read-only adapter PASS: %d assertions\n",assertions);return 0;}
