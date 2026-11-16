#include "gk323_original_spring.h"
#include <stdio.h>
#include <string.h>
static int n;
#define CHECK(x) do{++n;if(!(x)){fprintf(stderr,"FAILED line %d %s\n",__LINE__,#x);return 1;}}while(0)
static void put(uint8_t *p,size_t off,unsigned v){p[off]=(uint8_t)v;p[off+1]=(uint8_t)(v>>8);}
static void make(uint8_t *buf,unsigned count){
 memset(buf,0,GK323_RECORD_BYTES*count);
 for(unsigned i=0;i<count;i++){
  uint8_t *p=buf+i*GK323_RECORD_BYTES;
  put(p,0x14,101+i);put(p,0x16,111+i);put(p,0x18,51+i);
  put(p,0x1a,150+i);put(p,0x1c,4+i);put(p,0x1e,116+i);
  put(p,0x20,3+i);for(unsigned j=0;j<10;j++)put(p,0x22+2*j,20+i+j);
  put(p,0x36,200+i);put(p,0x3a,6+i);put(p,0x3e,27+i);
  put(p,0x40,430+i);put(p,0x42,118+i);put(p,0x44,35);
  put(p,0x46,0);put(p,0x49,34+i);
 }
}
int main(void){
 uint8_t raw[GK323_MAX_COUNTRIES*GK323_RECORD_BYTES];Gk323Spring s={0},old;
 make(raw,14);
 CHECK(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&s)==GK323_OK);
 CHECK(s.world==1 && s.country_count==14 && s.year==1175);
 CHECK(s.source_orders==3 && !s.port_orders_granted && !s.generated_by_port);
 CHECK(s.countries[0].gold==430&&s.countries[0].food==200);
 CHECK(s.countries[0].goods[0]==20&&s.countries[0].goods[1]==21);
 CHECK(s.countries[13].gold==443&&s.countries[13].goods[9]==42);
 old=s;
 #define REJECT(expr,expected) do{CHECK((expr)==(expected));CHECK(memcmp(&s,&old,sizeof s)==0);}while(0)
 REJECT(gk323_decode(NULL,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&s),GK323_ARGUMENT);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,NULL,14*83,&s),GK323_ARGUMENT);
 REJECT(gk323_decode("bad",GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&s),GK323_SOURCE_MISMATCH);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_SAVE_RAM,1175,0,3,raw,14*83,&s),GK323_SOURCE_MISMATCH);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_ARCHIVED_RESTORED,GK323_WORK_RAM,1175,0,3,raw,14*83,&s),GK323_SOURCE_MISMATCH);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_WORLD,GK323_FRESH_RESET,GK323_WORK_RAM,1206,0,3,raw,14*83,&s),GK323_SOURCE_MISMATCH);
 REJECT(gk323_decode(GK323_ROM_SHA256,0,GK323_ARCHIVED_RESTORED,GK323_SAVE_RAM,1175,0,3,raw,14*83,&s),GK323_UNSUPPORTED);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83-1,&s),GK323_LENGTH);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,1,3,raw,14*83,&s),GK323_SOURCE_MISMATCH);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,2,raw,14*83,&s),GK323_SOURCE_MISMATCH);
 put(raw,0x22,1000);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&s),GK323_FIELD);
 make(raw,14);put(raw,83*13+0x44,101);
 REJECT(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_FRESH_RESET,GK323_WORK_RAM,1175,0,3,raw,14*83,&s),GK323_FIELD);
 make(raw,27);
 CHECK(gk323_decode(GK323_ROM_SHA256,GK323_WORLD,GK323_ARCHIVED_RESTORED,GK323_SAVE_RAM,1206,0,3,raw,27*83,&s)==GK323_OK);
 CHECK(s.country_count==27&&s.countries[26].gold==456&&s.countries[26].goods[9]==55);
 CHECK(!s.generated_by_port&&!s.port_orders_granted);
 CHECK(gk323_decode(GK323_ROM_SHA256,GK323_MONGOL,GK323_ARCHIVED_RESTORED,GK323_SAVE_RAM,1175,0,3,raw,27*83,&s)==GK323_LENGTH);
 printf("GK323 PASS %d assertions: authentic stock 83-byte field parser; 14/27, fresh/archive bank gates, rollback, no gameplay unlock\n",n);
 return 0;
}
