#include "gk74_spring_archive.h"
#include "gk1_app_state.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL %s:%d: %s\n",__FILE__,__LINE__,#x);return 1;}checks++;}while(0)
static unsigned checks;
static void put16(uint8_t *b,unsigned at,unsigned v){b[at]=(uint8_t)v;b[at+1]=(uint8_t)(v>>8);}
int main(int argc,char **argv){
 Gk74SpringCountry row={555,666,777,888,999,1111};
 Gk74SpringCountry unchanged=row;
 CHECK(gk74_decode_record(NULL,83,&row)==GK74_BAD_INPUT);
 CHECK(gk74_decode_record((const uint8_t*)"",0,&row)==GK74_BAD_INPUT);
 CHECK(memcmp(&unchanged,&row,sizeof row)==0);
 uint8_t fake[83]={0};put16(fake,20,110);put16(fake,22,124);put16(fake,24,61);
 put16(fake,26,185);put16(fake,54,208);put16(fake,64,439);
 CHECK(gk74_decode_record(fake,sizeof fake,&row)==GK74_OK);
 CHECK(row.gold==439&&row.food==208&&row.troops==61);
 CHECK(row.arms==110&&row.skill==124&&row.economy==185);
 Gk74SpringArchive a;memset(&a,0xA5,sizeof a);
 Gk74SpringArchive sentinel=a;
 CHECK(gk74_load_file(NULL,&a)==GK74_BAD_INPUT);
 CHECK(gk74_load_file("__gk74_absent_file__.sram",&a)==GK74_IO);
 CHECK(memcmp(&a,&sentinel,sizeof a)==0);
 char path[256];snprintf(path,sizeof path,"gk74_negative_%u.bin",(unsigned)1234);
 FILE *f=fopen(path,"wb");CHECK(f!=NULL);CHECK(fwrite(fake,1,83,f)==83);CHECK(fclose(f)==0);
 CHECK(gk74_load_file(path,&a)==GK74_SIZE);CHECK(memcmp(&a,&sentinel,sizeof a)==0);
 f=fopen(path,"wb");CHECK(f!=NULL);uint8_t zero[GK74_RAM_BYTES]={0};
 CHECK(fwrite(zero,1,sizeof zero,f)==sizeof zero);CHECK(fclose(f)==0);
 CHECK(gk74_load_file(path,&a)==GK74_SHA);CHECK(memcmp(&a,&sentinel,sizeof a)==0);
 f=fopen(path,"ab");CHECK(f!=NULL);CHECK(fwrite(fake,1,1,f)==1);CHECK(fclose(f)==0);
 CHECK(gk74_load_file(path,&a)==GK74_SIZE);CHECK(memcmp(&a,&sentinel,sizeof a)==0);
 CHECK(remove(path)==0);
 if(argc==2){
  CHECK(gk74_load_file(argv[1],&a)==GK74_OK);
  CHECK(a.countries[0].gold==439&&a.countries[0].food==208);
  CHECK(a.countries[0].troops==61&&a.countries[0].arms==110);
  CHECK(a.countries[0].skill==124&&a.countries[0].economy==185);
  CHECK(gk74_load_file("__gk74_absent_file__.sram",&a)==GK74_IO);
  CHECK(a.countries[0].gold==439);
 }
 puts("EXEC0074 source-only private-optional SHA-gated archive decoder PASS");
 printf("Assertions: %u\n",checks);return 0;
}
