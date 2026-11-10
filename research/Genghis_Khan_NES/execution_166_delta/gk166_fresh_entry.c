#include "gk166_fresh_entry.h"
#include "gk165_fresh_json.h"
#include "gk162_original_entry.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <string.h>

static void wipe(void *p,size_t n){volatile uint8_t *v=(volatile uint8_t*)p;while(n--)*v++=0;}
Gk166Result gk166_enter_original_fresh_json(Gk1App *a,
 const char *csv,const char *json,uint8_t player){
 if(!a||!csv||!*csv||!json||!*json||player!=0u)return GK166_ARGUMENT;
 if(a->rules_profile>1u||!a->world_ready[0]||
    a->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[0].scenario_id!=GK1_MONGOL_CONQUEST||
    a->worlds[0].country_count!=14u)return GK166_TEMPLATE;
 if(!gk1_verified_original_csv(csv))return GK166_TEMPLATE;
 uint8_t raw[GK165_FRESH_SIZE];
 Gk165Status extracted=gk165_extract_fresh_json(json,raw);
 if(extracted!=GK165_OK){wipe(raw,sizeof raw);
  return extracted==GK165_IO?GK166_SOURCE_UNAVAILABLE:GK166_SOURCE;}
 Gk77StockSpring *observed=calloc(1,sizeof *observed);
 Gk79Checkpoint *candidate=malloc(sizeof *candidate);
 if(!observed||!candidate){free(observed);free(candidate);wipe(raw,sizeof raw);
  return GK166_SOURCE_UNAVAILABLE;}
 observed->profile=GK77_FRESH_MONGOL_WORKRAM;
 observed->scenario=GK1_MONGOL_CONQUEST;
 observed->country_count=14u;
 observed->orders=3u;
 observed->year=1175u;
 observed->rate=121u; /* This is one authenticated receipt, never global RNG. */
 Gk166Result result=GK166_SOURCE;
 for(unsigned i=0;i<14;i++){
  if(gk77_decode(raw+i*GK77_RECORD_BYTES,GK77_RECORD_BYTES,
        &observed->countries[i])!=GK77_OK)goto finish;
 }
 if(gk79_from_decoded(&a->worlds[0],observed,0u,candidate)!=GK79_OK)goto finish;
 /* Source provenance is established only by gk165_extract_fresh_json's
  * exact pinned fresh original WorkRAM 14x83 SHA; never set on user data. */
 candidate->source_verified=1u;
 if(!gk85_raw_source_intact(candidate)||!gk88_stock_projection_intact(candidate))goto finish;
 switch(gk162_commit_verified_checkpoint(a,csv,candidate)){
  case GK162_ENTRY_OK:result=GK166_OK;break;
  case GK162_ENTRY_TEMPLATE:result=GK166_TEMPLATE;break;
  default:break;
 }
finish:
 wipe(raw,sizeof raw);wipe(observed,sizeof *observed);wipe(candidate,sizeof *candidate);
 free(observed);free(candidate);
 return result;
}
