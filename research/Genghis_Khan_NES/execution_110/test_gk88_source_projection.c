#include "gk1_app_state.h"
#include "gk80_full_checkpoint.h"
#include "gk1_source_identity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"GK88 FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static void put16(uint8_t *b,unsigned offset,unsigned v){b[offset]=(uint8_t)v;b[offset+1u]=(uint8_t)(v>>8);}
static void encode(Gk77Country *r){
 uint8_t *b=r->raw;memset(b,0x5a,GK77_RECORD_BYTES);
 put16(b,0x14,r->arms);put16(b,0x16,r->skill);put16(b,0x18,r->troops);
 put16(b,0x1a,r->economy);put16(b,0x1c,r->town);put16(b,0x1e,r->defense);
 put16(b,0x20,r->masons);for(unsigned j=0;j<10u;j++)put16(b,0x22u+2u*j,r->goods[j]);
 put16(b,0x36,r->food);put16(b,0x3a,r->artisans);put16(b,0x3e,r->food_workers);
 put16(b,0x40,r->gold);put16(b,0x42,r->morale);put16(b,0x44,r->tax);
 put16(b,0x46,r->extra_tax);put16(b,0x49,r->undrafted);
}
static int case_world(const Gk1LoadedScenario *pre,Gk77Profile profile){
 Gk77StockSpring *src=calloc(1,sizeof *src);CHECK(src);
 src->scenario=(uint8_t)pre->scenario_id;src->country_count=pre->country_count;
 src->profile=profile;src->year=src->scenario==1?1175:1206;src->orders=3;src->rate=121;
 for(unsigned i=0;i<src->country_count;i++){
  const Gk1CountryTemplate *c=&pre->countries[i];Gk77Country *r=&src->countries[i];
  r->arms=c->arms;r->skill=c->skill;r->troops=c->troops;r->economy=c->economy;
  r->town=c->town;r->defense=c->defense;r->masons=c->masons;r->artisans=c->artisans;
  r->food_workers=c->food_workers;r->gold=c->gold;r->morale=c->morale;
  r->tax=c->tax;r->food=c->goods[10];r->undrafted=c->undrafted_reserve;
  for(unsigned k=0;k<10u;k++)r->goods[k]=c->goods[k];
  encode(r);
 }
 Gk79Checkpoint *base=calloc(1,sizeof *base);CHECK(base);
 CHECK(gk79_from_decoded(pre,src,0,base)==GK79_OK);
 CHECK(gk85_raw_source_intact(base)); /* legacy synthetic path remains usable */
 base->source_verified=1u;
 CHECK(gk88_stock_projection_intact(base));
 /* Synthetic test bytes are consistent but ARE NOT one of the three exact
  * original stock SHA profiles. Original authenticity must be refused. */
 CHECK(!gk85_raw_source_intact(base));
 CHECK(!gk77_known_source_digest(profile,base->raw_sha256));
 Gk79Checkpoint *bad=malloc(sizeof *bad);CHECK(bad);
 #define BAD_MUTATE(field) do{*bad=*base;bad->world.countries[0].field^=1u;\
 CHECK(!gk88_stock_projection_intact(bad));CHECK(!gk85_raw_source_intact(bad));CHECK(gk79_pass(bad)==GK79_BAD);\
 CHECK(bad->orders.remaining==3&&bad->orders.ended==0);\
 CHECK(gk80_save_new("/no-such-path/blocked.gk",bad)==GK80_IO_BAD);}while(0)
 BAD_MUTATE(gold);BAD_MUTATE(food_workers);BAD_MUTATE(troops);
 BAD_MUTATE(goods[0]);BAD_MUTATE(goods[10]);BAD_MUTATE(combined_population);
 BAD_MUTATE(eligible_draft_count);BAD_MUTATE(arms);BAD_MUTATE(skill);
 #undef BAD_MUTATE
 /* Independent raw record corruption, including one unmodelled byte, fails. */
 *bad=*base;bad->stock_raw[0][0]^=1;CHECK(!gk85_raw_source_intact(bad));
 /* A forged digest of the mutated record is still defeated by decoded-source comparison. */
 *bad=*base;bad->stock_raw[0][0x40]^=1;
 gk1_sha256_bytes((const uint8_t*)bad->stock_raw,
   (size_t)bad->world.country_count*GK77_RECORD_BYTES,bad->raw_sha256);
 CHECK(!gk85_raw_source_intact(bad));CHECK(gk79_pass(bad)==GK79_BAD);
 /* Already verified source supports real manual-proven Pass, without world mutation. */
 const Gk1LoadedScenario exact=base->world;
 CHECK(gk79_pass(base)==GK79_BAD); /* false authentic-source claim denied */
 base->source_verified=0u; /* source-neutral synthetic fixture only */
 CHECK(gk79_pass(base)==GK79_OK);
 CHECK(base->orders.remaining==0&&base->orders.ended==1);
 CHECK(memcmp(&exact,&base->world,sizeof exact)==0);
 CHECK(!gk88_stock_projection_intact(base)); /* authenticity bit now off */
 /* The original code-path for source-neutral tests remains unchanged. */
 base->source_verified=0u;
 CHECK(gk85_raw_source_intact(base));
 free(bad);free(base);free(src);return 0;
}
int main(int argc,char **argv){
 CHECK(argc==2);Gk1TemplateDataset *data=malloc(sizeof *data);CHECK(data);
 Gk1LoadedScenario *pre=malloc(sizeof *pre);CHECK(pre);
 CHECK(gk1_template_load(argv[1],data)==GK1_LOAD_OK);
 CHECK(gk1_open_loaded_scenario(data,GK1_MONGOL_CONQUEST,pre)==GK1_LOAD_OK);
 CHECK(case_world(pre,GK77_FRESH_MONGOL_WORKRAM)==0);
 CHECK(gk1_open_loaded_scenario(data,GK1_WORLD_CONQUEST,pre)==GK1_LOAD_OK);
 CHECK(case_world(pre,GK77_ARCHIVED_WORLD_SAVERAM)==0);
 free(pre);free(data);
 puts("GK88: 14/27-country original-record projection consistency, numeric tamper & forged hash refusal, Pass atomicity PASS");return 0;
}
