#include "gk80_full_checkpoint.h"
#include "gk1_source_identity.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

/* Header 16B, 31 explicitly serialized 16-bit values per country, SHA256 32B.
 * The additional authenticated source check is essential: a digest alone
 * cannot establish that the checkpoint came from the original NES. */
#define GK80_HEAD 16u
#define GK80_WORDS 31u
#define GK80_COUNTRY_BYTES (GK80_WORDS*2u)
#define GK80_DIGEST 32u
#define GK80_MAX (GK80_HEAD+27u*GK80_COUNTRY_BYTES+GK80_DIGEST)
static void put16(unsigned char *p,unsigned n){p[0]=(unsigned char)n;p[1]=(unsigned char)(n>>8);}
static int eligible(const Gk79Checkpoint *s){
 if(!s||s->valid!=1||s->profile<1||s->profile>3||
    s->orders.governor!=GK71_BASE||s->orders.season!=GK71_SPRING||
    s->orders.playable!=1||s->orders.allowance!=3||s->orders.completed!=0||
    s->orders.scenario!=s->world.scenario_id||
    s->orders.country_id!=s->player_index+1||
    s->world.phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE||
    s->year!=(s->world.scenario_id==1?1175u:1206u)||
    s->rate<1||s->rate>999||s->pending_pass>1||
    ((s->profile==GK77_ARCHIVED_WORLD_SAVERAM)!=(s->world.scenario_id==2))||
    s->world.country_count!=(s->world.scenario_id==1?14u:27u)||
    s->view_index>=s->world.country_count||
    !((s->orders.remaining==3&&s->orders.ended==0)||
      (s->orders.remaining==0&&s->orders.ended==1)))return 0;
 if(s->world.scenario_id==1){if(s->player_index!=0)return 0;}
 else if(s->player_index!=0&&s->player_index!=6&&
         s->player_index!=10&&s->player_index!=18)return 0;
 for(unsigned i=0;i<s->world.country_count;i++){
  const Gk1CountryTemplate *c=&s->world.countries[i];
  if(c->scenario_id!=s->world.scenario_id||c->country_id!=i+1u||c->index!=i)
   return 0;
 }
 return 1;
}
static void country(const Gk1CountryTemplate *c,unsigned char *p){
 const unsigned short v[GK80_WORDS]={
  c->goods[0],c->goods[1],c->goods[2],c->goods[3],c->goods[4],
  c->goods[5],c->goods[6],c->goods[7],c->goods[8],c->goods[9],c->goods[10],
  c->arms,c->skill,c->troops,c->economy,c->town,c->defense,c->masons,
  c->commodity_history,c->artisans,c->food_history,c->food_workers,
  c->gold,c->morale,c->tax,c->extra_tax_used,c->undrafted_reserve,
  c->eligibility_percent,c->drafted_population,c->combined_population,
  c->eligible_draft_count
 };
 for(unsigned i=0;i<GK80_WORDS;i++)put16(p+i*2u,v[i]);
}
static unsigned serialize(const Gk79Checkpoint *s,unsigned char b[GK80_MAX]){
 const unsigned n=GK80_HEAD+s->world.country_count*GK80_COUNTRY_BYTES;
 memset(b,0,GK80_MAX);
 memcpy(b,"GK80",4);b[4]=1;b[5]=s->profile;
 b[6]=(unsigned char)s->world.scenario_id;b[7]=s->player_index;
 b[8]=s->world.country_count;b[9]=s->orders.remaining;
 b[10]=s->orders.ended;b[11]=s->view_index;
 put16(b+12,s->year);put16(b+14,s->rate);
 for(unsigned i=0;i<s->world.country_count;i++)
  country(&s->world.countries[i],b+GK80_HEAD+i*GK80_COUNTRY_BYTES);
 gk1_sha256_bytes(b,n,b+n);
 return n+GK80_DIGEST;
}
Gk80IO gk80_save_new(const char *path,const Gk79Checkpoint *s){
 if(!path||!*path||!eligible(s))return GK80_IO_BAD;
 unsigned char bytes[GK80_MAX];unsigned n=serialize(s,bytes);
 errno=0;FILE *f=fopen(path,"wbx");
 if(!f)return errno==EEXIST?GK80_IO_EXISTS:GK80_IO_ERROR;
 const size_t wrote=fwrite(bytes,1,n,f);
 const int bad=ferror(f),close_bad=fclose(f);
 if(wrote!=n||bad||close_bad){(void)remove(path);return GK80_IO_ERROR;}
 return GK80_IO_OK;
}
Gk80IO gk80_load(const char *path,Gk79Checkpoint *source){
 if(!path||!*path||!eligible(source))return GK80_IO_BAD;
 FILE *f=fopen(path,"rb");if(!f)return GK80_IO_ERROR;
 unsigned char b[GK80_MAX+1u];const size_t n=fread(b,1,sizeof b,f);
 const int bad=ferror(f),close_bad=fclose(f);
 if(bad||close_bad)return GK80_IO_ERROR;
 const unsigned want=GK80_HEAD+source->world.country_count*GK80_COUNTRY_BYTES+GK80_DIGEST;
 if(n!=want)return GK80_IO_CORRUPT;
 const unsigned body=want-GK80_DIGEST;
 unsigned char sha[32];gk1_sha256_bytes(b,body,sha);
 if(memcmp(b+body,sha,32)!=0)return GK80_IO_CORRUPT;
 if(memcmp(b,"GK80",4)!=0||b[4]!=1||b[5]!=source->profile||
    b[6]!=source->world.scenario_id||b[7]!=source->player_index||
    b[8]!=source->world.country_count||b[11]>=source->world.country_count||
    b[12]!=(unsigned char)source->year||b[13]!=(unsigned char)(source->year>>8)||
    b[14]!=(unsigned char)source->rate||b[15]!=(unsigned char)(source->rate>>8)||
    !((b[9]==3&&b[10]==0)||(b[9]==0&&b[10]==1)))return GK80_IO_CORRUPT;
 /* Every recorded numeric field MUST match the newly independently validated
  * original source, not merely country #1 or the old metadata receipt. */
 unsigned char expected[GK80_COUNTRY_BYTES];
 for(unsigned i=0;i<source->world.country_count;i++){
  country(&source->world.countries[i],expected);
  if(memcmp(expected,b+GK80_HEAD+i*GK80_COUNTRY_BYTES,GK80_COUNTRY_BYTES)!=0)
   return GK80_IO_SOURCE_MISMATCH;
 }
 Gk79Checkpoint after=*source;
 after.orders.remaining=b[9];after.orders.ended=b[10];
 after.view_index=b[11];after.pending_pass=0;
 if(!eligible(&after))return GK80_IO_CORRUPT;
 *source=after;return GK80_IO_OK;
}
