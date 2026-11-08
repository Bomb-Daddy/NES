#include "gk79_stock_checkpoint.h"
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include "gk1_source_identity.h"
#include <string.h>

static int player_allowed(unsigned scenario,unsigned player){
 if(scenario==1)return player==0;
 return scenario==2&&(player==0||player==6||player==10||player==18);
}
Gk79Result gk79_from_decoded(const Gk1LoadedScenario *pre,
                              const Gk77StockSpring *source,
                              uint8_t player_index,Gk79Checkpoint *out){
 if(!pre||!source||!out||pre->phase!=GK1_PRE_SETUP_TEMPLATE||
    source->scenario<1||source->scenario>2||
    source->profile<GK77_FRESH_MONGOL_WORKRAM||
    source->profile>GK77_ARCHIVED_WORLD_SAVERAM||
    source->orders!=3||source->rate<1||source->rate>999||
    ((source->profile==GK77_ARCHIVED_WORLD_SAVERAM)!=(source->scenario==2))||
    pre->scenario_id!=(Gk1ScenarioId)source->scenario||
    pre->country_count!=source->country_count||
    source->country_count!=(source->scenario==1?14u:27u)||
    source->year!=(source->scenario==1?1175u:1206u))return GK79_BAD;
 if(!player_allowed(source->scenario,player_index))return GK79_UNSUPPORTED_PLAYER;
 Gk79Checkpoint next={0};next.world=*pre;
 next.world.phase=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE;
 next.year=source->year;next.rate=source->rate;
 next.profile=(uint8_t)source->profile;
 next.player_index=player_index;next.view_index=player_index;
 for(unsigned i=0;i<source->country_count;i++){
  const Gk77Country *r=&source->countries[i];
  Gk1CountryTemplate *c=&next.world.countries[i];
  if(c->scenario_id!=source->scenario||c->country_id!=i+1u||c->index!=i)
   return GK79_BAD;
  unsigned pop=(unsigned)r->troops+r->town+r->masons+r->artisans+r->food_workers;
  if(pop>UINT16_MAX||pop+r->undrafted>UINT16_MAX||c->eligibility_percent>100u)
   return GK79_BAD;
  c->arms=r->arms;c->skill=r->skill;c->troops=r->troops;
  c->economy=r->economy;c->town=r->town;c->defense=r->defense;
  c->masons=r->masons;c->artisans=r->artisans;
  c->food_workers=r->food_workers;c->gold=r->gold;c->morale=r->morale;
  c->tax=r->tax;c->undrafted_reserve=r->undrafted;
  for(unsigned j=0;j<10;j++)c->goods[j]=r->goods[j];
  c->goods[10]=r->food;
  c->drafted_population=(uint16_t)pop;
  c->combined_population=(uint16_t)(pop+r->undrafted);
  c->eligible_draft_count=(uint16_t)((unsigned)r->undrafted*c->eligibility_percent/100u);
  /* Source r->extra_tax is NOT the boolean port field extra_tax_used.
   * Unsourced historical fields remain from the pre-setup identity template;
   * this checkpoint is intentionally PARTIAL, NOT complete original state. */
 }
 if(gk71_open(&next.world,player_index,GK71_SPRING,GK71_BASE,&next.orders)!=GK71_OK)
  return GK79_BAD;
 /* The game must not silently discard unmodeled original country bytes.
  * Snapshot stays private in RAM and in owner-provided source, not compiled
  * into the copyright-clean binary or serialized into public QA evidence. */
 uint8_t combined[GK77_MAX_COUNTRIES*GK77_RECORD_BYTES];
 for(unsigned i=0;i<source->country_count;i++){
  memcpy(next.stock_raw[i],source->countries[i].raw,GK77_RECORD_BYTES);
  memcpy(combined+i*GK77_RECORD_BYTES,source->countries[i].raw,GK77_RECORD_BYTES);
 }
 gk1_sha256_bytes(combined,(size_t)source->country_count*GK77_RECORD_BYTES,next.raw_sha256);
 next.valid=1;
 *out=next;return GK79_OK;
}
Gk79Result gk79_open_original(const Gk1LoadedScenario *pre,
                              const char *path,Gk77Profile profile,
                              uint8_t player_index,Gk79Checkpoint *out){
 if(!pre||!path||!out)return GK79_BAD;
 Gk77StockSpring source;
 if(gk77_load(path,profile,&source)!=GK77_OK)return GK79_SOURCE_REJECTED;
 Gk79Checkpoint next;
 Gk79Result result=gk79_from_decoded(pre,&source,player_index,&next);
 if(result!=GK79_OK)return result;
 next.source_verified=1u;
 if(!gk85_raw_source_intact(&next))return GK79_BAD;
 *out=next;
 return GK79_OK;
}
/* Authentic checkpoint projection invariant. The 83-byte records are the
 * read-only source authority; ordinary native country mutations must never
 * silently de-couple from that authority while original first-Spring action
 * semantics remain unimplemented. This does NOT establish ROM authenticity:
 * only gk77_load's exact profile SHA can set source_verified=1. */
int gk88_stock_projection_intact(const Gk79Checkpoint *s){
 if(!s||!s->valid||s->source_verified!=1u||
    (s->world.scenario_id!=GK1_MONGOL_CONQUEST&&
     s->world.scenario_id!=GK1_WORLD_CONQUEST)||
    s->world.country_count!=(s->world.scenario_id==GK1_MONGOL_CONQUEST?14u:27u)||
    s->world.phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE||
    ((s->profile==GK77_ARCHIVED_WORLD_SAVERAM)!=(s->world.scenario_id==GK1_WORLD_CONQUEST)))return 0;
 for(unsigned i=0;i<s->world.country_count;i++){
  Gk77Country r;
  const Gk1CountryTemplate *c=&s->world.countries[i];
  if(gk77_decode(s->stock_raw[i],GK77_RECORD_BYTES,&r)!=GK77_OK)return 0;
  if(c->scenario_id!=s->world.scenario_id||c->country_id!=i+1u||c->index!=i||
     c->arms!=r.arms||c->skill!=r.skill||c->troops!=r.troops||
     c->economy!=r.economy||c->town!=r.town||c->defense!=r.defense||
     c->masons!=r.masons||c->artisans!=r.artisans||
     c->food_workers!=r.food_workers||c->gold!=r.gold||
     c->morale!=r.morale||c->tax!=r.tax||
     c->undrafted_reserve!=r.undrafted||c->goods[10]!=r.food)return 0;
  for(unsigned j=0;j<10u;j++)if(c->goods[j]!=r.goods[j])return 0;
  unsigned population=(unsigned)r.troops+r.town+r.masons+r.artisans+r.food_workers;
  if(population>UINT16_MAX||population+r.undrafted>UINT16_MAX||
     c->eligibility_percent>100u||
     c->drafted_population!=population||
     c->combined_population!=population+r.undrafted||
     c->eligible_draft_count!=r.undrafted*c->eligibility_percent/100u)return 0;
 }
 return 1;
}
int gk85_raw_source_intact(const Gk79Checkpoint *s){
 if(!s||!s->valid||(s->world.country_count!=14u&&s->world.country_count!=27u)||
    s->raw_view>1u||s->source_verified>1u)return 0;
 uint8_t sha[32];
 gk1_sha256_bytes((const uint8_t*)s->stock_raw,
                 (size_t)s->world.country_count*GK77_RECORD_BYTES,sha);
 if(memcmp(sha,s->raw_sha256,32)!=0)return 0;
 if(!s->source_verified)return 1;
 /* The SHA on the captured private record must still equal the pinned
  * ORIGINAL-stock profile digest; a rewritten raw+world+digest cannot pass. */
 return gk77_known_source_digest((Gk77Profile)s->profile,sha)&&
        gk88_stock_projection_intact(s);
}
Gk79Result gk79_pass(Gk79Checkpoint *session){
 if(!session||!session->valid||session->profile<1||session->profile>3||
    session->world.phase!=(Gk1ScenarioPhase)GK71_RESERVED_VERIFIED_PLAYABLE_PHASE||
    session->world.scenario_id!=session->orders.scenario||
    session->orders.country_id!=session->player_index+1u||
    !gk85_raw_source_intact(session))return GK79_BAD;
 Gk71Budget next=session->orders;
 Gk71Result r=gk71_pass(&next);
 if(r==GK71_EXHAUSTED)return GK79_EXHAUSTED;
 if(r!=GK71_OK)return GK79_BAD;
 session->orders=next;session->pending_pass=0;
 return GK79_OK;
}

/* This is a portable, 32-byte, original-RAM-free checkpoint PASS receipt.
 * It is not an original NES save/SRAM or a completed native campaign save. */
#define GK79_RECEIPT_SIZE 32u
static int valid_receipt_state(const Gk79Checkpoint *s){
 return s&&s->valid==1u&&s->profile>=1u&&s->profile<=3u&&
        s->orders.playable==1u&&s->orders.season==GK71_SPRING&&
        s->orders.governor==GK71_BASE&&s->orders.allowance==3u&&
        s->orders.completed==0u&&s->orders.country_id==s->player_index+1u&&
        s->orders.scenario==s->world.scenario_id&&
        ((s->orders.remaining==3u&&s->orders.ended==0u)||
         (s->orders.remaining==0u&&s->orders.ended==1u));
}
static void receipt(const Gk79Checkpoint *s,uint8_t b[GK79_RECEIPT_SIZE]){
 memset(b,0,GK79_RECEIPT_SIZE);
 b[0]='G';b[1]='K';b[2]='7';b[3]='9';b[4]=1u;
 b[5]=s->profile;b[6]=(uint8_t)s->world.scenario_id;
 b[7]=s->player_index;b[8]=s->world.country_count;
 b[9]=s->orders.remaining;b[10]=s->orders.ended;
 b[11]=(uint8_t)s->year;b[12]=(uint8_t)(s->year>>8);
 b[13]=(uint8_t)s->rate;b[14]=(uint8_t)(s->rate>>8);
 /* b[15] = reserved zero. SHA256-truncated integrity tag on metadata. */
 uint8_t digest[32];gk1_sha256_bytes(b,16u,digest);
 memcpy(b+16u,digest,16u);
}
Gk79IOStatus gk79_save_receipt_new(const char *path,const Gk79Checkpoint *s){
 if(!path||!*path||!valid_receipt_state(s))return GK79_IO_INVALID;
 uint8_t b[GK79_RECEIPT_SIZE];receipt(s,b);
 errno=0;FILE *fp=fopen(path,"wbx");
 if(!fp)return errno==EEXIST?GK79_IO_EXISTS:GK79_IO_ERROR;
 size_t n=fwrite(b,1,sizeof b,fp);
 int bad=ferror(fp);int close_error=fclose(fp);
 if(n!=sizeof b||bad||close_error){(void)remove(path);return GK79_IO_ERROR;}
 return GK79_IO_OK;
}
Gk79IOStatus gk79_load_receipt(const char *path,Gk79Checkpoint *session){
 if(!path||!*path||!valid_receipt_state(session))return GK79_IO_INVALID;
 FILE *fp=fopen(path,"rb");if(!fp)return GK79_IO_ERROR;
 uint8_t b[GK79_RECEIPT_SIZE+1u];size_t n=fread(b,1,sizeof b,fp);
 int bad=ferror(fp);int close_error=fclose(fp);
 if(bad||close_error)return GK79_IO_ERROR;
 if(n!=GK79_RECEIPT_SIZE)return GK79_IO_CORRUPT;
 Gk79Checkpoint candidate=*session;
 candidate.orders.remaining=b[9];candidate.orders.ended=b[10];
 /* Replay is only 3->0 PASS; cannot synthesize a completed trade, unobserved
  * action count, or arbitrary non-Spring seasonal state. */
 if(!valid_receipt_state(&candidate))return GK79_IO_CORRUPT;
 uint8_t expected[GK79_RECEIPT_SIZE];receipt(&candidate,expected);
 if(memcmp(b,expected,sizeof expected)!=0)return GK79_IO_CORRUPT;
 candidate.pending_pass=0;
 *session=candidate;return GK79_IO_OK;
}
