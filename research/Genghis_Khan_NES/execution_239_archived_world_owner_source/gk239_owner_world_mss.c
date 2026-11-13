#include "gk239_owner_world_mss.h"
#include "gk163_mesen_import.h"
#include "gk162_original_entry.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void secure_clear(void *p,size_t n){
 volatile unsigned char *v=(volatile unsigned char *)p;
 while(n--)*v++=0;
}
Gk239Status gk239_owner_world_startup(Gk1App *a,
 const char *csv,const char *mss,const char *fresh_json,
 int loaded,int diagnostic){
 if(!mss||!*mss)return GK239_UNREQUESTED;
 /* A diagnostic run must not open owner files, even on inherited env vars. */
 if(diagnostic)return GK239_SMOKE_ISOLATED;
 if(fresh_json&&*fresh_json)return GK239_CONFLICTING_SOURCES;
 if(!a||!csv||!*csv||!loaded||a->screen!=GK1_SCREEN_TITLE||
    a->rules_profile>1u||a->stock_checkpoint.valid||
    !a->world_ready[0]||!a->world_ready[1]||
    a->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[1].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[0].country_count!=14u||a->worlds[1].country_count!=27u||
    a->worlds[1].scenario_id!=GK1_WORLD_CONQUEST||
    !gk1_verified_original_csv(csv))return GK239_REFUSED;
 /* Original Mesen archive contains native 8 KiB nesSaveRam, not fresh
  * nesWorkRam. GK163 verifies the matching state schema, physical bank and
  * pinned stock 27x83 country SHA before exposing any record bytes. */
 uint8_t raw[GK163_MAX_RECORDS];
 size_t size=0u;
 Gk139MesenProof proof={0};
 Gk163Status src=gk163_mesen_extract_with_proof(mss,
    GK77_ARCHIVED_WORLD_SAVERAM,raw,&size,&proof);
 if(src!=GK163_OK||size!=27u*GK77_RECORD_BYTES){
  secure_clear(raw,sizeof raw);return GK239_REFUSED;
 }
 Gk77StockSpring *observed=(Gk77StockSpring*)calloc(1,sizeof *observed);
 Gk79Checkpoint *candidate=(Gk79Checkpoint*)calloc(1,sizeof *candidate);
 if(!observed||!candidate){free(observed);free(candidate);
  secure_clear(raw,sizeof raw);return GK239_REFUSED;}
 observed->profile=GK77_ARCHIVED_WORLD_SAVERAM;
 observed->scenario=GK1_WORLD_CONQUEST;
 observed->country_count=27u;
 observed->orders=3u;
 observed->year=1206u;
 observed->rate=114u; /* only this archived original observation */
 Gk239Status result=GK239_REFUSED;
 for(unsigned i=0;i<27u;i++){
  if(gk77_decode(raw+i*GK77_RECORD_BYTES,GK77_RECORD_BYTES,
      &observed->countries[i])!=GK77_OK)goto done;
 }
 if(gk79_from_decoded(&a->worlds[1],observed,0u,candidate)!=GK79_OK)
  goto done;
 /* Source authenticity derives from GK163 bank+known source SHA, never
  * from this user-visible import request or a synthetic profile. */
 candidate->source_verified=1u;
 if(!gk85_raw_source_intact(candidate)||!gk88_stock_projection_intact(candidate))
  goto done;
 if(gk162_commit_verified_checkpoint(a,csv,candidate)!=GK162_ENTRY_OK)
  goto done;
 a->rules_profile=0u;
 snprintf(a->status,sizeof a->status,
   "OWNER-PRIVATE ARCHIVED WORLD SPRING 1206: 3 SOURCE ORDERS; NOT FRESH RNG");
 result=GK239_IMPORTED;
done:
 secure_clear(raw,sizeof raw);
 secure_clear(observed,sizeof *observed);
 secure_clear(candidate,sizeof *candidate);
 free(observed);free(candidate);
 return result;
}
