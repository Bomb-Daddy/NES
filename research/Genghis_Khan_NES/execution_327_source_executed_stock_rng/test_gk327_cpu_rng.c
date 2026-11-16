#include "gk327_fixed6502_rng.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static unsigned assertions=0;
#define CHECK(x) do{assert(x);assertions++;}while(0)
static uint8_t *fixture(void){
 uint8_t *r=calloc(GK327_ROM_BYTES,1);
 CHECK(r!=NULL);
 memcpy(r,"NES\x1a",4);r[4]=16;r[6]=0x12;
 return r;
}
static void writeprog(uint8_t *r,uint16_t pc,const uint8_t *code,size_t n){
 CHECK((unsigned)pc>=0xc000u&&
    (unsigned)(pc-0xc000u)+n<=16384u);
 memcpy(r+GK327_PRG_BANK15_START+pc-0xc000u,code,n);
}
int main(void){
 uint8_t *rom=fixture();uint8_t seed[6]={1,2,3,4,5,6};
 Gk327RngReceipt out={0};
 /* Self-authored test program, no original stock program bytes. */
 const uint8_t program[]={
  0xa9,0xff,             /* LDA #$ff */
  0x8d,0x66,0x00,        /* STA $0066 */
  0xa9,0x01,             /* LDA #1 */
  0x8d,0x67,0x00,        /* STA $0067 */
  0x18,                  /* CLC */
  0xa9,0x80,             /* LDA #$80 */
  0x8d,0x84,0x00,        /* STA $0084 */
  0xa2,0x00,             /* LDX #0 */
  0x3e,0x84,0x00,        /* ROL $0084,X -> 0, carry1 */
  0x90,0x02,             /* BCC not taken */
  0x69,0x00,             /* ADC #0 -> A=$81 */
  0x99,0x84,0x00,        /* STA $0084,Y, Y=0 */
  0x60
 };
 writeprog(rom,0xd010,program,sizeof program);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd010,seed,100u,&out)==GK327_OK);
 CHECK(out.authentic_rom==0u&&out.seed_after[0]==0x81u);
 CHECK(out.result_66==0xffu&&out.result_67==1u);
 CHECK(out.final_pc==(uint16_t)(0xd010u+sizeof program-1u));
 CHECK(out.first_spring_generated==0&&out.player_orders_granted==0);
 CHECK(memcmp(out.seed_before,seed,6)==0);
 /* Atomic rejection: poisoned inputs cannot alter a pre-filled receipt. */
 uint8_t saved[sizeof out];memset(&out,0xa5,sizeof out);memcpy(saved,&out,sizeof out);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES-1,0xd010,seed,10u,&out)==GK327_BAD_ROM);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xbfff,seed,10u,&out)==GK327_BAD_PC);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd010,seed,0u,&out)==GK327_BAD_ARG);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 CHECK(gk327_execute_original_rng(rom,GK327_ROM_BYTES,seed,&out)==GK327_BAD_SOURCE);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 const uint8_t unsupported[]={0x02};writeprog(rom,0xd020,unsupported,sizeof unsupported);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd020,seed,10u,&out)==GK327_UNSUPPORTED);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 const uint8_t blocked[]={0xa9,0x01,0x8d,0x00,0x60,0x60};
 writeprog(rom,0xd020,blocked,sizeof blocked);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd020,seed,10u,&out)==GK327_BAD_WRITE);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 const uint8_t loop[]={0xa9,0x01,0xd0,0xfe};writeprog(rom,0xd030,loop,sizeof loop);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd030,seed,50u,&out)==GK327_STEP_LIMIT);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd030,seed,GK327_RNG_MAX_STEPS+1u,&out)==GK327_BAD_ARG);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 const uint8_t skip[]={0xa9,0x00,0xf0,0x00,0x60};/* BEQ unsupported */
 writeprog(rom,0xd040,skip,sizeof skip);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd040,seed,20u,&out)==GK327_UNSUPPORTED);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 const uint8_t branch_oob[]={0xa9,0x01,0xd0,0xfb};
 writeprog(rom,0xc000,branch_oob,sizeof branch_oob);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xc000,seed,20u,&out)==GK327_BAD_PC);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 CHECK(gk327_execute_fixture(NULL,GK327_ROM_BYTES,0xd010,seed,10u,&out)==GK327_BAD_ARG);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd010,NULL,10u,&out)==GK327_BAD_ARG);
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd010,seed,10u,NULL)==GK327_BAD_ARG);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 rom[4]=15;
 CHECK(gk327_execute_fixture(rom,GK327_ROM_BYTES,0xd010,seed,10u,&out)==GK327_BAD_ROM);
 CHECK(memcmp(&out,saved,sizeof out)==0);
 free(rom);
 printf("GK327 synthetic source-free 6502 CPU interpreter / hostile rollback: %u assertions PASS\n",assertions);
 return 0;
}
