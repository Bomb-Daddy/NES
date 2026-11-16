#include "gk326_cpu_country_store.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static int checks;
#define CHECK(x) do {checks++;if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static uint8_t rom[GK326_ORIGINAL_ROM_SIZE],ram[GK326_RAM_BYTES],before[GK326_RAM_BYTES];
static Gk326CpuSource cpu;
static Gk326StoreReceipt receipt;
static void boot(void){
 memset(rom,0,sizeof rom);memcpy(rom,"NES\x1a",4);rom[4]=16;rom[6]=0x10;
 memset(ram,0x55,sizeof ram);memset(&cpu,0,sizeof cpu);
 cpu.pc=0xc100;cpu.bank=15;cpu.condition=GK326_FRESH_WORKRAM;cpu.country_count=14;
 memset(&receipt,0xa5,sizeof receipt);
}
static void instr(uint8_t op,uint8_t low,uint8_t high){
 unsigned off=16u+15u*16384u+0x100u;rom[off]=op;rom[off+1]=low;rom[off+2]=high;
}
static void rejected(Gk326Status expected){
 memcpy(before,ram,sizeof ram);Gk326StoreReceipt old=receipt;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==expected);
 CHECK(memcmp(before,ram,sizeof ram)==0);
 CHECK(memcmp(&old,&receipt,sizeof receipt)==0);
}
int main(void){
 boot();instr(0x91,0xff,0);cpu.zp[0xff]=0xcd;cpu.zp[0]=0x6d;cpu.y=0x22;cpu.a=0x7b;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.cpu_address==0x6def&&receipt.physical_offset==0x0def);
 CHECK(receipt.offset_in_country==0x22&&receipt.country_one_based==1);
 CHECK(receipt.opcode==0x91&&receipt.old_value==0x55&&receipt.new_value==0x7b);
 CHECK(ram[0x0def]==0x7b&&receipt.backing==GK326_FRESH_WORKRAM);
 CHECK(receipt.source_address_decoded==1&&!receipt.first_spring_generated&&!receipt.player_orders_granted);
 /* Switch to archived saved state: same virtual address but separately supplied backing. */
 boot();cpu.condition=GK326_ARCHIVED_SAVERAM;cpu.country_count=27;
 instr(0x91,0xfe,0);cpu.zp[0xfe]=0xcd;cpu.zp[0xff]=0x6d;
 cpu.y=82;cpu.a=0x92;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.offset_in_country==82&&receipt.country_one_based==1);
 CHECK(receipt.backing==GK326_ARCHIVED_SAVERAM&&ram[0xe1f]==0x92); /* different offset */
 /* Indexed indirect pointer wraps at $FF; X indexes pointer, Y does not. */
 boot();instr(0x81,0xfd,0);cpu.x=2;cpu.zp[0xff]=0xd0;cpu.zp[0]=0x6d;cpu.a=9;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.cpu_address==0x6dd0&&receipt.offset_in_country==3);
 /* Absolute indexed addressing and country 14/27 boundaries. */
 boot();instr(0x9d,0xcd,0x6d);cpu.x=(uint8_t)82;cpu.a=4;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.country_one_based==1&&receipt.offset_in_country==82);
 boot();instr(0x8e,0xcd,0x6d);cpu.x=99;cpu.a=8;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.new_value==99&&ram[0xdcd]==99);
 boot();instr(0x8c,0xcd,0x6d);cpu.y=33;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.new_value==33&&ram[0xdcd]==33);
 boot();instr(0x9e,0xcc,0x6d);cpu.x=5;cpu.y=1;
 rejected(GK326_UNSUPPORTED_OPCODE); /* 0x9E is unofficial SHX, not STX */
 boot();instr(0x99,0xcc,0x6d);cpu.a=42;cpu.y=1;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.new_value==42);
 /* Refuse unsupported 6502 instruction and ungrounded source bank. */
 boot();instr(0x20,0xcd,0x6d);rejected(GK326_UNSUPPORTED_OPCODE);
 boot();instr(0x91,0x10,0);cpu.zp[0x10]=0x03;cpu.zp[0x11]=0x6e;cpu.y=0;
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.country_one_based==1&&receipt.offset_in_country==0x36);
 boot();instr(0x8d,0x36,0x7b);rejected(GK326_NOT_COUNTRY);
 boot();instr(0x8d,0xcd,0x6d);cpu.bank=14;rejected(GK326_WRONG_BANK);
 boot();instr(0x8d,0xcd,0x6d);cpu.pc=0x7fff;rejected(GK326_WRONG_BANK);
 boot();instr(0x8d,0xcd,0x6d);cpu.country_count=15;rejected(GK326_BAD_CONTEXT);
 boot();instr(0x8d,0xcd,0x6d);cpu.condition=0;rejected(GK326_BAD_CONTEXT);
 boot();instr(0x8d,0xcd,0x6d);rom[6]=0x20;rejected(GK326_BAD_ROM);
 boot();instr(0x8d,0xcd,0x6d);rom[0]='X';rejected(GK326_BAD_ROM);
 boot();instr(0x8d,0xcd,0x6d);
 CHECK(gk326_apply_country_store(rom,sizeof rom-1,&cpu,ram,&receipt)==GK326_BAD_ROM);
 boot();cpu.pc=0xffff;cpu.bank=15;rejected(GK326_BAD_CONTEXT);
 boot();cpu.pc=0xfffe;cpu.bank=15;instr(0x8d,0xcd,0x6d);rom[sizeof rom-2]=0x8d;rom[sizeof rom-1]=0xcd;
 rejected(GK326_BAD_CONTEXT);
 /* Genuine world country 27 last byte is within 8 KB SRAM. */
 boot();cpu.country_count=27;
 unsigned end=GK326_COUNTRY_BASE+27*GK326_COUNTRY_BYTES-1u;
 instr(0x8d,(uint8_t)end,(uint8_t)(end>>8));
 CHECK(gk326_apply_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_OK);
 CHECK(receipt.country_one_based==27&&receipt.offset_in_country==82);
 CHECK(receipt.physical_offset==(uint16_t)(end-0x6000));
 end++;boot();cpu.country_count=27;instr(0x8d,(uint8_t)end,(uint8_t)(end>>8));
 rejected(GK326_NOT_COUNTRY);
 /* Fake but structurally plausible NES ROM must never pass stock SHA gate. */
 boot();instr(0x8d,0xcd,0x6d);
 memcpy(before,ram,sizeof ram);Gk326StoreReceipt old=receipt;
 CHECK(gk326_apply_stock_country_store(rom,sizeof rom,&cpu,ram,&receipt)==GK326_BAD_ROM);
 CHECK(memcmp(before,ram,sizeof ram)==0);
 CHECK(memcmp(&old,&receipt,sizeof receipt)==0);
 puts("PASS GK326 exact bank-bound 6502 country-store decoder");
 printf("Checked %d source-store and fail-atomic assertions\n",checks);
 return 0;
}
