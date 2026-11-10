#include "gk162_original_entry.h"
#include "gk117_stock_pristine_gate.h"
#include <stdlib.h>
#include <string.h>

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
    a->worlds[w].country_count!=(w?27u:14u))return GK162_ENTRY_TEMPLATE;
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
 a->stock_checkpoint=*candidate;free(candidate);
 /* A source profile switch must not leave any Enhanced rollover chain active:
  * new original checkpoints have different SHA/bank/years and cannot inherit
  * Enhanced receipts or workers from the previous profile. */
 memset(&a->enhanced_assign,0,sizeof a->enhanced_assign);
 memset(&a->enhanced_summer,0,sizeof a->enhanced_summer);
 memset(&a->enhanced_fall,0,sizeof a->enhanced_fall);
 memset(&a->enhanced_winter,0,sizeof a->enhanced_winter);
 memset(&a->enhanced_year2,0,sizeof a->enhanced_year2);
 a->active_world=(uint8_t)w;
 a->spring_stock_mode=1u;
 a->spring_stock_count=a->stock_checkpoint.world.country_count;
 a->spring_archive_cursor=player;
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
