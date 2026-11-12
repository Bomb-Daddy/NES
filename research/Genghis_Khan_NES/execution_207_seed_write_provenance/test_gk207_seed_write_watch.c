#include "gk207_seed_write_watch.h"
#include "gk83_reset_trace.h"
#include "gk91_reset_zero.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(8);}checks++;}while(0)
static void observer_controls(void){
 Gk207SeedWatch w={0}, old;
 CHECK(!gk207_source_written_complete(&w));
 CHECK(gk207_note_completed_seed_store(&w,0xc101,0x83,42,1,15,10)==GK207_OUTSIDE);
 CHECK(gk207_note_completed_seed_store(&w,0xc101,0x8a,42,1,15,10)==GK207_OUTSIDE);
 CHECK(gk207_note_completed_seed_store(&w,0xc101,0x2084,42,1,15,10)==GK207_OUTSIDE);
 CHECK(gk207_note_completed_seed_store(&w,0xc101,0x6084,42,1,15,10)==GK207_OUTSIDE);
 CHECK(w.completed_writes==0 && !w.written_mask);
 for(unsigned i=0;i<6;i++){
  Gk207Status r=gk207_note_completed_seed_store(&w,0xc100+(uint16_t)i,
      (uint16_t)(0x1884u+i),(uint8_t)(0x10u+i),1,15,30+i);
  CHECK(r==GK207_RECORDED);
  CHECK(w.written_mask==(uint8_t)((1u<<(i+1u))-1u));
  CHECK(w.first[i].byte_index==i&&w.first[i].cpu_address==0x1884u+i);
  CHECK(w.first[i].cpu_cycle==30u+i && w.first[i].bank==15u);
  CHECK(w.latest_written[i]==0x10u+i);
 }
 CHECK(gk207_source_written_complete(&w));
 CHECK(gk207_note_completed_seed_store(&w,0x9000,0x0084,0xaa,0,0,99)==GK207_RECORDED);
 CHECK(w.latest_written[0]==0xaau && w.first[6].bank==0xffu);
 CHECK(w.written_mask==GK207_FULL_SIX_WRITES);
 memcpy(&old,&w,sizeof w);
 CHECK(gk207_note_completed_seed_store(&w,0x7fff,0x84,0,1,15,100)==GK207_INVALID);
 CHECK(!memcmp(&w,&old,sizeof w));
 CHECK(gk207_note_completed_seed_store(&w,0x8000,0x84,0,2,15,100)==GK207_INVALID);
 CHECK(!memcmp(&w,&old,sizeof w));
 CHECK(gk207_note_completed_seed_store(&w,0x8000,0x84,0,1,16,100)==GK207_INVALID);
 CHECK(!memcmp(&w,&old,sizeof w));
 CHECK(gk207_note_completed_seed_store(&w,0x8000,0x84,0,1,15,0)==GK207_INVALID);
 CHECK(!memcmp(&w,&old,sizeof w));
 CHECK(gk207_note_completed_seed_store(NULL,0x8000,0x84,0,1,15,100)==GK207_INVALID);
 for(unsigned i=7;i<37;i++)
  CHECK(gk207_note_completed_seed_store(&w,0x9000,0x84,0x11,1,14,100+i)==GK207_RECORDED);
 CHECK(w.completed_writes==37 && w.dropped_details==5);
 CHECK(w.first[0].value==0x10 && w.last.value==0x11);
 w.completed_writes=UINT32_MAX;old=w;
 CHECK(gk207_note_completed_seed_store(&w,0x9000,0x84,0,1,14,1000)==GK207_INVALID);
 CHECK(!memcmp(&w,&old,sizeof w));
}
static void stock_controls(void){
 const char *rom=getenv("GK1_ORIGINAL_STOCK_ROM");
 if(!rom||!*rom){puts("GK207 original ROM not supplied: observer negative controls only");return;}
 Gk83Trace strict={0},zero={0};
 Gk83Stop a=gk83_trace_original_reset(rom,20000,&strict);
 Gk83Stop b=gk91_trace_reset_mesen_allzeros(rom,20000,&zero);
 CHECK(a==GK83_UNKNOWN_RAM_GATE);
 CHECK(strict.instructions==70 && strict.stop_pc==0x8060u);
 CHECK(b==GK83_HARDWARE_GATE);
 CHECK(zero.instructions==142 && zero.stop_pc==0xc5beu);
 /* THIS DOES NOT SAY THE GAME NEVER WRITES RNG: only first 70/142
  * authenticated completed original-source instructions were executed. */
 CHECK(strict.source_seed_writes.completed_writes==0u);
 CHECK(zero.source_seed_writes.completed_writes==0u);
 CHECK(strict.source_seed_writes.written_mask==0u);
 CHECK(zero.source_seed_writes.written_mask==0u);
 CHECK(!gk207_source_written_complete(&strict.source_seed_writes));
 CHECK(!gk207_source_written_complete(&zero.source_seed_writes));
}
int main(void){observer_controls();stock_controls();printf("GK207 PASS %u checks; executable source writes only; first Spring NOT generated\n",checks);return 0;}
