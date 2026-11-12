#include "gk212_stock_raw_variation.h"
#include <string.h>
Gk212Status gk212_compare_original_mongol_raw(const char *csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *fresh,
 const Gk79Checkpoint *archived,Gk212Variation *out){
 if(!csv||!pre||!fresh||!archived||!out)return GK212_INVALID;
 if(fresh->profile!=GK77_FRESH_MONGOL_WORKRAM ||
    archived->profile!=GK77_ARCHIVED_MONGOL_SAVERAM ||
    fresh->year!=1175u||archived->year!=1175u ||
    fresh->orders.season!=GK71_SPRING||archived->orders.season!=GK71_SPRING ||
    pre->scenario_id!=GK1_MONGOL_CONQUEST ||
    pre->country_count!=GK212_COUNTRIES ||
    fresh->world.country_count!=GK212_COUNTRIES ||
    archived->world.country_count!=GK212_COUNTRIES)
  return GK212_WRONG_PROFILE;
 Gk112PairedHistories checked={0};
 Gk112Status r=gk112_compare_mongol_histories(csv,pre,fresh,archived,&checked);
 if(r!=GK112_OK)return r==GK112_TEMPLATE_REFUSED?GK212_SETUP_REFUSED:
                         GK212_SOURCE_REFUSED;
 Gk212Variation next={0};
 next.country_count=GK212_COUNTRIES;
 next.cells_compared=GK212_COUNTRIES*GK212_RAW_BYTES;
 for(unsigned offset=0;offset<GK212_RAW_BYTES;offset++){
  for(unsigned country=0;country<GK212_COUNTRIES;country++){
   if(fresh->stock_raw[country][offset]==archived->stock_raw[country][offset])
    continue;
   next.differences_per_offset[offset]++;
   next.differences_per_country[country]++;
   next.differing_cells++;
  }
  next.equal_in_two_histories[offset]=next.differences_per_offset[offset]==0u;
  if(next.differences_per_offset[offset])next.offsets_differing++;
 }
 for(unsigned country=0;country<GK212_COUNTRIES;country++)
  if(next.differences_per_country[country])next.countries_differing++;
 for(unsigned field=0;field<GK112_METRIC_COUNT;field++){
  next.numeric_metric_differences+=checked.countries_changed[field];
  if(checked.countries_changed[field])next.numeric_fields_differing++;
 }
 /* Both the 83-byte raw and decoded numeric projections are source attested.
  * No field's apparent stability is interpreted as a universal RNG rule. */
 *out=next;
 return GK212_OK;
}
