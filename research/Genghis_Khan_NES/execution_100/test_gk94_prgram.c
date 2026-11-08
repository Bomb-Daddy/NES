#include "gk94_prgram.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"GK94 FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){
 Gk94PrgRam s;memset(&s,0xa5,sizeof s);gk94_prgram_reset_unknown(&s);
 /* Fresh stock WorkRam observed at physical offset 0x0DCD does not
  * establish *anything* about separately mapped battery SaveRam. */
 uint8_t out=0xb7;
 CHECK(gk94_prgram_read(&s,0x6dcdu,&out)==GK94_RAM_UNKNOWN);
 CHECK(out==0xb7);
 CHECK(gk94_prgram_read(&s,0x0dcdu,&out)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_read(&s,0x8000u,&out)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_write(&s,0x0dcdu,99)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_write(&s,0x8000u,99)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_write(&s,0x6dcdu,0x4d)==GK94_RAM_OK);
 CHECK(gk94_prgram_read(&s,0x6dcdu,&out)==GK94_RAM_OK&&out==0x4d);
 CHECK(gk94_prgram_read(&s,0x6dceu,&out)==GK94_RAM_UNKNOWN);
 CHECK(out==0x4d);
 uint8_t payload[]={1,2,3,4,5};
 CHECK(gk94_prgram_seed_physical(&s,0x0dcd,payload,sizeof payload)==GK94_RAM_OK);
 for(unsigned i=0;i<5;i++){
  CHECK(gk94_prgram_read(&s,(uint16_t)(0x6dcdu+i),&out)==GK94_RAM_OK);
  CHECK(out==i+1);
 }
 CHECK(gk94_prgram_read(&s,0x6000u,&out)==GK94_RAM_UNKNOWN);
 CHECK(gk94_prgram_read(&s,0x7fffu,&out)==GK94_RAM_UNKNOWN);
 CHECK(gk94_prgram_seed_physical(&s,8191,payload,5)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_seed_physical(&s,8192,payload,1)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_seed_physical(&s,0,payload,0)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_seed_physical(&s,0,0,5)==GK94_RAM_INVALID);
 CHECK(gk94_prgram_write(&s,0x7fffu,0xee)==GK94_RAM_OK);
 CHECK(gk94_prgram_read(&s,0x7fffu,&out)==GK94_RAM_OK&&out==0xee);
 CHECK(gk94_prgram_write(&s,0x6000u,0x12)==GK94_RAM_OK);
 CHECK(gk94_prgram_read(&s,0x6000u,&out)==GK94_RAM_OK&&out==0x12);
 gk94_prgram_reset_unknown(&s);
 CHECK(gk94_prgram_read(&s,0x6dcdu,&out)==GK94_RAM_UNKNOWN);
 CHECK(out==0x12);
 puts("GK94 battery SaveRam/work RAM bank isolation, rollback and physical offset PASS");
 return 0;
}
