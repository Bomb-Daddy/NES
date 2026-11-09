#include "gk117_stock_pristine_gate.h"
#include "gk1_source_identity.h"
#include <stdlib.h>
#include <string.h>

Gk117Status gk117_open_attested_original(const char *source_csv,
 const Gk1LoadedScenario *pre,const char *private_records,
 Gk77Profile profile,uint8_t player_index,Gk79Checkpoint *out){
 if(!source_csv||!pre||!private_records||!out||!*private_records||
    (profile!=GK77_FRESH_MONGOL_WORKRAM&&
     profile!=GK77_ARCHIVED_MONGOL_SAVERAM&&
     profile!=GK77_ARCHIVED_WORLD_SAVERAM))return GK117_ARGUMENT;
 if(!gk1_verified_original_csv(source_csv))return GK117_CSV_SOURCE_REFUSED;
 if(pre->phase!=GK1_PRE_SETUP_TEMPLATE||
    (pre->scenario_id!=GK1_MONGOL_CONQUEST&&
     pre->scenario_id!=GK1_WORLD_CONQUEST))return GK117_TEMPLATE_DRIFT;
 /* The original numeric records authenticate only the fields they contain.
  * Unsourced inherited values (tax history, country names, extra flags and
  * draft eligibility) must come from the exact canonical source, not edits. */
 Gk1TemplateDataset *dataset=malloc(sizeof *dataset);
 Gk1LoadedScenario *canonical=malloc(sizeof *canonical);
 if(!dataset||!canonical){free(dataset);free(canonical);return GK117_CSV_SOURCE_REFUSED;}
 int clean=gk1_template_load(source_csv,dataset)==GK1_LOAD_OK&&
     gk1_open_loaded_scenario(dataset,pre->scenario_id,canonical)==GK1_LOAD_OK&&
     memcmp(canonical,pre,sizeof *pre)==0;
 free(dataset);free(canonical);
 if(!clean)return GK117_TEMPLATE_DRIFT;
 /* The exact 14*83 or 27*83 stock source is SHA-checked by GK77/GK79.
  * Use a candidate so the caller's checkpoint cannot be partially updated. */
 Gk79Checkpoint *candidate=malloc(sizeof *candidate);
 if(!candidate)return GK117_CSV_SOURCE_REFUSED;
 Gk79Result r=gk79_open_original(pre,private_records,profile,player_index,candidate);
 if(r!=GK79_OK||!candidate->source_verified||
    !gk85_raw_source_intact(candidate)||!gk88_stock_projection_intact(candidate)){
  free(candidate);return GK117_ORIGINAL_RECORD_REFUSED;
 }
 *out=*candidate;
 free(candidate);
 return GK117_OK;
}
