#include "gk162_original_entry.h"
#include "gk117_stock_pristine_gate.h"
#include "gk183_checkpoint_isolation.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <string.h>

/* EXEC0276: source-history-compatible controller setup admission.
 * Archived restored Mesen states document the actual RAM/season but DO NOT
 * document what native setup wizard choices were used to reach that RAM.
 * Never label one as produced by an arbitrary new wizard configuration.
 * Cold source continuation from an UNTOUCHED title screen remains legal.
 * Fresh stock 1175 retains the separately observed EXEC0275 choices. */
static int pristine_title(const Gk1App *a,unsigned w){
 if(a->setup_stage[w]!=GK53_SETUP_PLAYERS ||
    a->setup_candidate[w]!=255u || a->setup_player_count[w]!=1u ||
    a->setup_difficulty[w]!=1u || a->setup_player_slot[w]!=0u ||
    a->setup_ability_method[w]!=0u || a->setup_method_cursor[w]!=0u ||
    a->manual_bonus_active[w] || a->manual_bonus_person[w] ||
    a->manual_bonus_ability[w])return 0;
 for(unsigned i=0;i<4u;i++)if(a->setup_rulers[w][i]!=255u)return 0;
 for(unsigned i=0;i<5u;i++)for(unsigned j=0;j<6u;j++)
  if(a->manual_bonus[w][i][j])return 0;
 return 1;
}
int gk162_original_setup_compatible(const Gk1App *a,Gk77Profile profile){
 if(!a || profile<GK77_FRESH_MONGOL_WORKRAM ||
   profile>GK77_ARCHIVED_WORLD_SAVERAM)return 0;
 unsigned w=(profile==GK77_ARCHIVED_WORLD_SAVERAM)?1u:0u;
 if(profile!=GK77_FRESH_MONGOL_WORKRAM)return pristine_title(a,w);
 /* EXEC0275's known fresh original Temujin/one-player/difficulty-1/auto
  * compatibility (not proof that the native game generated original RNG). */
 if(a->setup_stage[0]==GK53_SETUP_PLAYERS)return 1;
 if(a->setup_stage[0]!=GK53_SETUP_AWAIT_SOURCE || a->active_world!=0u ||
    a->setup_player_count[0]!=1u || a->setup_player_slot[0]!=1u ||
    a->setup_candidate[0]!=0u || a->setup_rulers[0][0]!=0u ||
    a->setup_rulers[0][1]!=255u || a->setup_rulers[0][2]!=255u ||
    a->setup_rulers[0][3]!=255u || a->setup_difficulty[0]!=1u ||
    a->setup_ability_method[0]!=1u || a->manual_bonus_active[0])return 0;
 for(unsigned i=0;i<5u;i++)for(unsigned j=0;j<6u;j++)
  if(a->manual_bonus[0][i][j])return 0;
 return 1;
}

Gk162EntryStatus gk162_enter_original_checkpoint(Gk1App *a,
 const char *csv,const char *records,Gk77Profile profile,uint8_t player){
 if(!a||!csv||!*csv||!records||!*records||
    (profile!=GK77_FRESH_MONGOL_WORKRAM&&
     profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK162_ENTRY_ARGUMENT;
 unsigned w=profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 if(a->rules_profile>1u||!a->world_ready[w]||
    a->worlds[w].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[w].scenario_id!=(w?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||
    a->worlds[w].country_count!=(w?27u:14u) ||
    !gk162_original_setup_compatible(a,profile))return GK162_ENTRY_TEMPLATE;
 /* Production source gate does all identity, SHA, and source-projection checks
  * into a separate candidate; this function cannot mint a playable source. */
 Gk79Checkpoint *candidate=malloc(sizeof *candidate);
 if(!candidate)return GK162_ENTRY_SOURCE;
 Gk117Status ok=gk117_open_attested_original(csv,&a->worlds[w],
   records,profile,player,candidate);
 if(ok!=GK117_OK){free(candidate);
  return ok==GK117_TEMPLATE_DRIFT?GK162_ENTRY_TEMPLATE:GK162_ENTRY_SOURCE;
 }
 if(candidate->orders.remaining!=3u||candidate->orders.season!=GK71_SPRING||
    candidate->year!=(w?1206u:1175u)||!gk85_raw_source_intact(candidate)||
    !gk88_stock_projection_intact(candidate)){
   free(candidate);return GK162_ENTRY_SOURCE;
 }
 Gk162EntryStatus committed=gk162_commit_verified_checkpoint(a,csv,candidate);
 free(candidate);
 return committed;
}

Gk162EntryStatus gk162_commit_verified_checkpoint(Gk1App *a,const char *csv,
 const Gk79Checkpoint *source){
 if(!a||!csv||!*csv||!source||!source->valid||source->source_verified!=1u||
    source->profile<GK77_FRESH_MONGOL_WORKRAM||
    source->profile>GK77_ARCHIVED_WORLD_SAVERAM)return GK162_ENTRY_ARGUMENT;
 unsigned w=source->profile==GK77_ARCHIVED_WORLD_SAVERAM?1u:0u;
 if(a->rules_profile>1u||!a->world_ready[w]||
    a->worlds[w].phase!=GK1_PRE_SETUP_TEMPLATE||
    a->worlds[w].scenario_id!=(w?GK1_WORLD_CONQUEST:GK1_MONGOL_CONQUEST)||
    a->worlds[w].country_count!=(w?27u:14u)||
    source->world.scenario_id!=a->worlds[w].scenario_id||
    source->world.country_count!=a->worlds[w].country_count ||
    !gk162_original_setup_compatible(a,source->profile))return GK162_ENTRY_TEMPLATE;
 /* A public commit helper must not allow a SHA-authenticated record to
  * borrow an arbitrary mutated, PRE_SETUP-labeled native template. Verify
  * full 41-country canonical source and exact original scenario bytes. */
 if(!gk1_verified_original_csv(csv))return GK162_ENTRY_TEMPLATE;
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1LoadedScenario *canonical=malloc(sizeof *canonical);
 if(!dataset||!canonical){free(dataset);free(canonical);return GK162_ENTRY_SOURCE;}
 int verified=gk1_template_load(csv,dataset)==GK1_LOAD_OK&&
     gk1_open_loaded_scenario(dataset,a->worlds[w].scenario_id,canonical)==GK1_LOAD_OK&&
     memcmp(canonical,&a->worlds[w],sizeof *canonical)==0;
 free(dataset);free(canonical);
 if(!verified)return GK162_ENTRY_TEMPLATE;
 if(source->orders.remaining!=3u||source->orders.season!=GK71_SPRING||
    source->orders.allowance!=3u||source->orders.governor!=GK71_BASE||
    source->orders.country_id!=source->player_index+1u||
    source->year!=(w?1206u:1175u)||
    source->orders.scenario!=source->world.scenario_id||
    !gk85_raw_source_intact(source)||!gk88_stock_projection_intact(source))
    return GK162_ENTRY_SOURCE;
 a->stock_checkpoint=*source;
 /* A source profile switch must not leave any Enhanced rollover chain active:
  * new original checkpoints have different SHA/bank/years and cannot inherit
  * Enhanced receipts or workers from the previous profile. */
 /* Source attestation and canonical-template gates above MUST pass first.
  * Clear the entire Enhanced chain (including post-Year2 continuation),
  * plus any pending PORT storage I/O from the previous source. */
 gk183_reset_checkpoint_derived(a);
 a->active_world=(uint8_t)w;
 a->spring_stock_mode=1u;
 a->spring_stock_count=a->stock_checkpoint.world.country_count;
 a->spring_archive_cursor=source->player_index;
 a->spring_preview_mode=0u;
 a->stock_trade_merchant=0u;a->stock_trade_item=0u;a->stock_trade_sell=0u;
 a->stock_reconcile_view=0u;
 a->stock_command_cursor=0u;
 a->stock_checkpoint.pending_pass=0u;
 a->stock_checkpoint.raw_view=0u;
 a->screen=GK1_SCREEN_STOCK_CONTINUE;
 strcpy(a->status,"ORIGINAL STOCK CHECKPOINT RESTORED; VIEW/PASS ONLY; NO NEW NES SETUP");
 return GK162_ENTRY_OK;
}
