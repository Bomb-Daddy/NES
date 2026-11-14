#include "gk166_fresh_entry.h"
#include "gk165_fresh_json.h"
#include "gk162_original_entry.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <string.h>

static void wipe(void *p,size_t n){volatile uint8_t *v=(volatile uint8_t*)p;while(n--)*v++=0;}
/* EXEC0275: this ONE authenticated original WorkRAM receipt was captured
 * with Mongol one-human Temujin, difficulty 1, automatically generated
 * attributes, and no manual point allocation. The original B (no-prince)
 * choice and intervening NES RNG history are NOT represented by the port
 * wizard; passing this gate proves compatibility of KNOWN choices only.
 *
 * A title-screen cold continuation (default GK53_SETUP_PLAYERS) remains
 * legal because it makes no claim that the port wizard generated the NES
 * record. Once a user started a wizard, another difficulty/ruler/ability
 * choice must not silently import a fixed unrelated historic Spring.
 * No source data or private ROM bytes are embedded in this module. */
static int gk275_known_fresh_choice_compatible(const Gk1App *a){
 if(a->setup_stage[0]==GK53_SETUP_PLAYERS)return 1;
 if(a->setup_stage[0]!=GK53_SETUP_AWAIT_SOURCE || a->active_world!=0u ||
    a->setup_player_count[0]!=1u||a->setup_player_slot[0]!=1u||
    a->setup_candidate[0]!=0u||a->setup_rulers[0][0]!=0u||
    a->setup_rulers[0][1]!=255u||a->setup_rulers[0][2]!=255u||
    a->setup_rulers[0][3]!=255u||a->setup_difficulty[0]!=1u||
    a->setup_ability_method[0]!=1u||a->manual_bonus_active[0])return 0;
 for(unsigned i=0;i<5u;i++)for(unsigned j=0;j<6u;j++)
  if(a->manual_bonus[0][i][j])return 0;
 return 1;
}
Gk166Result gk166_enter_original_fresh_json(Gk1App *a,
 const char *csv,const char *json,uint8_t player){
 if(!a||!csv||!*csv||!json||!*json||player!=0u)return GK166_ARGUMENT;
 if(a->rules_profile>1u||!a->world_ready[0]||
    a->worlds[0].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[0].scenario_id!=GK1_MONGOL_CONQUEST||
    a->worlds[0].country_count!=14u||
    !gk275_known_fresh_choice_compatible(a))return GK166_TEMPLATE;
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
