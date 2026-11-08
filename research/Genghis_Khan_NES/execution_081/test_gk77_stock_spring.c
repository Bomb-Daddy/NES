#include "gk77_stock_spring.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void put16(uint8_t b[83],unsigned p,uint16_t v){b[p]=(uint8_t)v;b[p+1]=(uint8_t)(v>>8);}
int main(int argc,char **argv){
 uint8_t b[83]={0};Gk77Country c;
 memset(&c,0x7d,sizeof c);
 assert(gk77_decode(NULL,83,&c)==GK77_INVALID);
 assert(gk77_decode(b,82,&c)==GK77_INVALID);
 for(unsigned i=0;i<sizeof c;i++)assert(((uint8_t*)&c)[i]==0x7d);
 put16(b,0x14,109);put16(b,0x16,114);put16(b,0x18,61);
 put16(b,0x1a,186);put16(b,0x1c,5);put16(b,0x1e,115);
 put16(b,0x20,4);for(unsigned i=0;i<10;i++)put16(b,0x22+2*i,(uint16_t)(321+i));
 put16(b,0x36,208);put16(b,0x3a,7);put16(b,0x3e,3);
 put16(b,0x40,430);put16(b,0x42,118);put16(b,0x44,20);
 put16(b,0x46,6);put16(b,0x49,34);
 assert(gk77_decode(b,83,&c)==GK77_OK);
 assert(c.arms==109&&c.skill==114&&c.troops==61&&c.economy==186);
 assert(c.town==5&&c.defense==115&&c.masons==4&&c.food==208);
 assert(c.artisans==7&&c.food_workers==3&&c.gold==430&&c.morale==118);
 assert(c.tax==20&&c.extra_tax==6&&c.undrafted==34);
 for(unsigned i=0;i<10;i++)assert(c.goods[i]==321+i);
 /* Regression: +0x24 is Silk NOT Fur, and +0x36 is Food not goods. */
 assert(c.goods[0]==321 && c.goods[1]==322 && c.food==208);
 assert(gk77_profile_parse("fresh")==GK77_FRESH_MONGOL_WORKRAM);
 assert(gk77_profile_parse("mongol")==GK77_ARCHIVED_MONGOL_SAVERAM);
 assert(gk77_profile_parse("world")==GK77_ARCHIVED_WORLD_SAVERAM);
 assert(gk77_profile_parse("stock")==0&&gk77_profile_parse(NULL)==0);
 Gk77StockSpring out;memset(&out,0xa6,sizeof out);
 assert(gk77_load("nonexistent",GK77_FRESH_MONGOL_WORKRAM,&out)==GK77_IO);
 assert(gk77_load("nonexistent",(Gk77Profile)9,&out)==GK77_INVALID);
 assert(gk77_load(NULL,GK77_FRESH_MONGOL_WORKRAM,&out)==GK77_INVALID);
 for(unsigned i=0;i<sizeof out;i++)assert(((const uint8_t*)&out)[i]==0xa6);
 if(argc==4){
  const Gk77Profile profiles[]={GK77_FRESH_MONGOL_WORKRAM,GK77_ARCHIVED_MONGOL_SAVERAM,GK77_ARCHIVED_WORLD_SAVERAM};
  const unsigned gold[]={430,439,193},food[]={208,208,332},troops[]={61,61,116},rate[]={121,101,114};
  for(unsigned j=0;j<3;j++){
   memset(&out,0xa6,sizeof out);
   assert(gk77_load(argv[1+j],profiles[j],&out)==GK77_OK);
   assert(out.countries[0].gold==gold[j]);assert(out.countries[0].food==food[j]);
   assert(out.countries[0].troops==troops[j]&&out.rate==rate[j]);
   assert(out.orders==3&&out.country_count==(j==2?27:14));
   printf("VERIFIED PRIVATE STOCK PROFILE %s: %u COUNTRY RECORDS PASS\n",gk77_profile_name(profiles[j]),out.country_count);
  }
  uint8_t *p=(uint8_t*)malloc(2241u);assert(p);
  FILE *in=fopen(argv[3],"rb");assert(in);
  assert(fread(p,1,2241,in)==2241);assert(fclose(in)==0);
  p[0x22]^=1; /* altered actual source traded item must fail SHA. */
  FILE *bad=fopen("gk77_corrupt_fixture.bin","wb");assert(bad);
  assert(fwrite(p,1,2241,bad)==2241);assert(fclose(bad)==0);
  Gk77StockSpring before=out;
  assert(gk77_load("gk77_corrupt_fixture.bin",GK77_ARCHIVED_WORLD_SAVERAM,&out)==GK77_SHA);
  assert(memcmp(&before,&out,sizeof out)==0);remove("gk77_corrupt_fixture.bin");free(p);
  assert(gk77_load(argv[1],GK77_ARCHIVED_WORLD_SAVERAM,&out)==GK77_SIZE);
  assert(gk77_load(argv[3],GK77_ARCHIVED_MONGOL_SAVERAM,&out)==GK77_SIZE);
  assert(memcmp(&before,&out,sizeof out)==0);
 }
 puts("EXEC0077 C11 original stock record offset, profile SHA/bank, read-only rollback PASS");return 0;
}
