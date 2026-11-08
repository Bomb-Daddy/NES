#include "gk90_verified_finance.h"
#include "gk1_source_identity.h"
#include <string.h>
/* Comparing canonical scenario state as whole structs is valid here because
 * the project source factory zero-initializes the entire structs before
 * writing parsed fields. It consequently also catches forged names, ROM
 * addresses, historical/source columns and OTHER countries which a simple
 * selected-country finance comparison could miss. */
Gk90Result gk90_compare_spring_verified(const char *path,
  const Gk1LoadedScenario *baseline,const Gk79Checkpoint *stock,
  uint8_t index,Gk89SpringReconciliation *out){
 if(!path||!baseline||!stock||!out)return GK90_ARGUMENT;
 if(!gk1_verified_original_csv(path))return GK90_CSV_NOT_AUTHENTIC;
 Gk1TemplateDataset source;
 if(gk1_template_load(path,&source)!=GK1_LOAD_OK)return GK90_CSV_NOT_AUTHENTIC;
 Gk1LoadedScenario actual;
 if(gk1_open_loaded_scenario(&source,baseline->scenario_id,&actual)!=GK1_LOAD_OK||
    memcmp(baseline,&actual,sizeof actual)!=0)return GK90_TEMPLATE_DRIFT;
 Gk89SpringReconciliation candidate;
 if(gk89_compare_spring(&actual,stock,index,&candidate)!=GK89_OK)
  return GK90_STOCK_REFUSED;
 *out=candidate;
 return GK90_OK;
}
