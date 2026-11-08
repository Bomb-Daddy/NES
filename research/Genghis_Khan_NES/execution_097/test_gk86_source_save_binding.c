#include "gk1_app_state.h"
#include "gk80_full_checkpoint.h"
#include "gk1_source_identity.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Public synthetic source records. None of this test's 83-byte data is from
 * the original cartridge. The real source has to pass gk77_load SHA separately. */
static void synth(const Gk1LoadedScenario *pre,Gk77Profile profile,Gk77StockSpring *s){
 memset(s,0,sizeof *s);s->scenario=(uint8_t)pre->scenario_id;
 s->profile=profile;s->country_count=pre->country_count;
 s->orders=3;s->year=(uint16_t)(s->scenario==1?1175:1206);s->rate=137;
 for(unsigned i=0;i<s->country_count;i++){
  const Gk1CountryTemplate *c=&pre->countries[i];Gk77Country *r=&s->countries[i];
  r->arms=c->arms;r->skill=c->skill;r->troops=c->troops;
  r->economy=c->economy;r->town=c->town;r->defense=c->defense;
  r->masons=c->masons;r->artisans=c->artisans;r->food_workers=c->food_workers;
  r->gold=c->gold;r->morale=c->morale;r->tax=c->tax;r->food=c->goods[10];
  r->undrafted=c->undrafted_reserve;
  for(unsigned j=0;j<10u;j++)r->goods[j]=c->goods[j];
  for(unsigned j=0;j<83u;j++)r->raw[j]=(unsigned char)(i*13u+j*17u+19u);
 }
}
static void check(const Gk1LoadedScenario *pre,Gk77Profile profile,unsigned player){
 Gk77StockSpring *s=calloc(1,sizeof *s);assert(s);synth(pre,profile,s);
 Gk79Checkpoint *initial=malloc(sizeof *initial),*other=malloc(sizeof *other);
 Gk79Checkpoint *before=malloc(sizeof *before);assert(initial&&other&&before);
 assert(gk79_from_decoded(pre,s,(uint8_t)player,initial)==GK79_OK);
 assert(gk79_from_decoded(pre,s,(uint8_t)player,other)==GK79_OK);
 const char *v2="gk86_test_v2.gk8",*v1="gk86_test_legacy.gk8";
 (void)remove(v2);(void)remove(v1);
 assert(gk80_save_new(v2,initial)==GK80_IO_OK);
 assert(gk80_save_new(v2,initial)==GK80_IO_EXISTS);
 FILE *f=fopen(v2,"rb");assert(f);
 unsigned char all[16u+27u*62u+64u];size_t n=fread(all,1,sizeof all,f);
 assert(fclose(f)==0);
 const size_t body=16u+pre->country_count*62u;
 assert(n==body+64u&&all[4]==2u);
 assert(memcmp(all+body,initial->raw_sha256,32)==0);
 /* A separately validated 83-byte source can decode to the exact same
  * 31 numeric fields but carry a different unused original byte. V1 could
  * not discriminate; V2 MUST reject it without modifying caller state. */
 s->countries[s->country_count-1].raw[82]^=0x80u;
 assert(gk79_from_decoded(pre,s,(uint8_t)player,other)==GK79_OK);
 assert(memcmp(other->world.countries,initial->world.countries,
               sizeof initial->world.countries)==0);
 assert(memcmp(other->raw_sha256,initial->raw_sha256,32)!=0);
 *before=*other;
 assert(gk80_load(v2,other)==GK80_IO_SOURCE_MISMATCH);
 assert(memcmp(before,other,sizeof *other)==0);
 /* Even forging the V2 source tag and recalculating the unkeyed digest
  * cannot move the authorization boundary to a new unverified source. */
 unsigned char edited[sizeof all+1u];memcpy(edited,all,n);
 edited[body+31u]^=1u;
 gk1_sha256_bytes(edited,n-32u,edited+n-32u);
 const char *forged="gk86_forged.gk8";(void)remove(forged);
 f=fopen(forged,"wb");assert(f);
 assert(fwrite(edited,1,n,f)==n&&fclose(f)==0);
 assert(gk80_load(forged,other)==GK80_IO_SOURCE_MISMATCH);
 assert(memcmp(before,other,sizeof *other)==0);
 assert(remove(forged)==0);
 /* Unmodified owner-reattached source reopens source-bound V2 in cold load. */
 assert(gk79_from_decoded(pre,s,(uint8_t)player,other)==GK79_OK);
 /* s still has changed raw data; restore the original from saved first. */
 s->countries[s->country_count-1].raw[82]^=0x80u;
 assert(gk79_from_decoded(pre,s,(uint8_t)player,other)==GK79_OK);
 assert(gk80_load(v2,other)==GK80_IO_OK);
 assert(other->orders.remaining==3u&&other->orders.ended==0u);
 /* Exact prior GK80 version 1 format remains loadable, but must report
  * source-binding weakness; never call it a V2 authenticated load. */
 edited[0]='G';edited[1]='K';edited[2]='8';edited[3]='0';
 memcpy(edited,all,body);
 edited[4]=1u;
 gk1_sha256_bytes(edited,body,edited+body);
 f=fopen(v1,"wb");assert(f);
 assert(fwrite(edited,1,body+32u,f)==body+32u&&fclose(f)==0);
 assert(gk80_load(v1,other)==GK80_IO_LEGACY_SOURCE_UNBOUND);
 assert(gk80_load(v2,other)==GK80_IO_OK);
 /* Old V1 cannot differentiate a new source with same numeric projection;
  * explicit status warns the user to save a V2 in a different slot. */
 s->countries[s->country_count-1].raw[82]^=0x80u;
 assert(gk79_from_decoded(pre,s,(uint8_t)player,other)==GK79_OK);
 assert(gk80_load(v1,other)==GK80_IO_LEGACY_SOURCE_UNBOUND);
 /* Corrupt, truncated, appended and version-confused V2 files roll back. */
 for(int mode=0;mode<4;mode++){
  const char *bad="gk86_corrupt.gk8";(void)remove(bad);
  memcpy(edited,all,n);size_t len=n;
  if(mode==0)edited[n-1u]^=0x1u;
  if(mode==1)len=n-1u;
  if(mode==2){edited[n]=0x42u;len=n+1u;}
  if(mode==3)edited[4]=3u;
  f=fopen(bad,"wb");assert(f);
  assert(fwrite(edited,1,len,f)==len&&fclose(f)==0);
  *before=*other;
  assert(gk80_load(bad,other)==GK80_IO_CORRUPT);
  assert(memcmp(before,other,sizeof *other)==0);
  assert(remove(bad)==0);
 }
 assert(remove(v2)==0&&remove(v1)==0);
 free(before);free(other);free(initial);free(s);
}
int main(int argc,char **argv){
 assert(argc==2);
 Gk1TemplateDataset *d=malloc(sizeof *d);assert(d);
 Gk1LoadedScenario *mongol=malloc(sizeof *mongol),*world=malloc(sizeof *world);
 assert(mongol&&world);
 assert(gk1_template_load(argv[1],d)==GK1_LOAD_OK);
 assert(gk1_open_loaded_scenario(d,GK1_MONGOL_CONQUEST,mongol)==GK1_LOAD_OK);
 assert(gk1_open_loaded_scenario(d,GK1_WORLD_CONQUEST,world)==GK1_LOAD_OK);
 check(mongol,GK77_FRESH_MONGOL_WORKRAM,0);
 check(mongol,GK77_ARCHIVED_MONGOL_SAVERAM,0);
 check(world,GK77_ARCHIVED_WORLD_SAVERAM,18);
 free(world);free(mongol);free(d);
 puts("EXEC0086 PASS: V2 full-original-83-byte SHA-bound saves 14/27; legacy V1 explicit warning; forged SHA, wrong source, corrupt/truncated/appended/invalid version fail closed with rollback (synthetic data)");
 return 0;
}
