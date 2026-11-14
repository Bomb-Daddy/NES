#include "gk164_mss_entry.h"
#include "gk162_original_entry.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <string.h>

static void wipe(void *p,size_t n){
 volatile uint8_t *b=(volatile uint8_t*)p;
 while(n--)*b++=0;
}
Gk164Result gk164_enter_original_mss(Gk1App *app,const char *csv,
 const char *mss,Gk77Profile profile,uint8_t player){
 if(!app||!csv||!*csv||!mss||!*mss||
    (profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK164_ARGUMENT;
 unsigned w=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 if(app->rules_profile>1u||!app->world_ready[w]||
    app->worlds[w].phase!=GK1_PRE_SETUP_TEMPLATE||
    app->worlds[w].scenario_id!=(w?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||
    app->worlds[w].country_count!=(w?27u:14u) ||
    !gk162_original_setup_compatible(app,profile))return GK164_TEMPLATE;
 /* Exact 41-country CSV and every unsourced PRE_SETUP field are checked
  * again atomically inside gk162_commit_verified_checkpoint before commit.
  * Reject a clearly invalid canonical CSV early, before private state reads. */
 if(!gk1_verified_original_csv(csv))return GK164_TEMPLATE;
 /* No intermediate raw source file is ever created. The independently
  * SHA-bound original Mesen archive importer performs format+bank+source
  * provenance checks before any candidate live-state mutation. */
 uint8_t raw[GK163_MAX_RECORDS]={0};size_t raw_size=0;
 Gk163Status extracted=gk163_mesen_extract(mss,profile,raw,&raw_size);
 if(extracted!=GK163_OK){wipe(raw,sizeof raw);
   return extracted==GK163_UNAVAILABLE?GK164_SOURCE_UNAVAILABLE:GK164_SOURCE;
 }
 const unsigned n=profile==GK77_ARCHIVED_WORLD_SAVERAM?27u:14u;
 if(raw_size!=n*GK77_RECORD_BYTES){wipe(raw,sizeof raw);return GK164_SOURCE;}
 /* Metadata rate/year identify this exact archived evidence profile only;
  * they are NOT randomized Spring initialization constants. */
 Gk77StockSpring *observed=calloc(1,sizeof *observed);
 Gk79Checkpoint *session=malloc(sizeof *session);
 if(!observed||!session){free(observed);free(session);wipe(raw,sizeof raw);
   return GK164_SOURCE_UNAVAILABLE;}
 observed->profile=profile;observed->scenario=(uint8_t)(w+1u);
 observed->country_count=(uint8_t)n;observed->orders=3u;
 observed->year=(uint16_t)(w?1206u:1175u);
 observed->rate=(uint16_t)(w?114u:101u);
 Gk164Result result=GK164_SOURCE;
 for(unsigned i=0;i<n;i++){
  if(gk77_decode(raw+i*GK77_RECORD_BYTES,GK77_RECORD_BYTES,
     &observed->countries[i])!=GK77_OK)goto finish;
 }
 if(gk79_from_decoded(&app->worlds[w],observed,player,session)!=GK79_OK)goto finish;
 /* Trusted only because the Mesen importer verified exact known-original
  * source digest; gk85 and gk88 repeat integrity/projection checks. */
 session->source_verified=1u;
 if(!gk85_raw_source_intact(session)||!gk88_stock_projection_intact(session))goto finish;
 switch(gk162_commit_verified_checkpoint(app,csv,session)){
  case GK162_ENTRY_OK:result=GK164_OK;break;
  case GK162_ENTRY_TEMPLATE:result=GK164_TEMPLATE;break;
  default:break;
 }
finish:
 wipe(raw,sizeof raw);wipe(observed,sizeof *observed);
 wipe(session,sizeof *session);free(observed);free(session);
 return result;
}
