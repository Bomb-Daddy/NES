#include "gk217_finance_preview.h"
#include <string.h>
Gk217Status gk217_preview_country(const Gk1LoadedScenario *world,
                                  unsigned country_index,Gk217Preview *out){
 if(!world||!out||world->phase!=GK1_PRE_SETUP_TEMPLATE||
    (world->scenario_id!=GK1_MONGOL_CONQUEST &&
     world->scenario_id!=GK1_WORLD_CONQUEST)||
    country_index>=world->country_count)return GK217_INVALID;
 /* Existing source-grounded full-country engine rejects malformed identities,
  * capacity/finance problems; do not silently fall back to a single-country
  * computation that could mask corruption elsewhere in the 14/27-country map. */
 Gk214Batch batch;
 if(gk214_project_spring_batch(world,&batch)!=GK214_OK)
  return GK217_FINANCE_REFUSED;
 if(!batch.source_bounded_only||batch.generated_setup||batch.generated_rng||
    batch.orders_granted||batch.after.phase!=GK1_PRE_SETUP_TEMPLATE||
    batch.count!=world->country_count)return GK217_FINANCE_REFUSED;
 const Gk1CountryTemplate *before=&world->countries[country_index];
 const Gk1CountryTemplate *after=&batch.after.countries[country_index];
 const Gk75SpringResult *delta=&batch.country[country_index];
 Gk217Preview candidate={0};
 candidate.country_count=batch.count;
 candidate.selected_country=(uint8_t)country_index;
 candidate.before_gold=before->gold;candidate.after_gold=after->gold;
 candidate.before_food=before->goods[10];candidate.after_food=after->goods[10];
 candidate.before_troops=before->troops;candidate.after_troops=after->troops;
 candidate.troops_lost=delta->troops_lost;
 candidate.taxes=delta->gross_tax;
 candidate.payroll=delta->payroll;
 candidate.food_consumed=delta->food_consumed;
 candidate.total_taxes=batch.gross_taxes;
 candidate.total_payroll=batch.payroll;
 candidate.total_food_consumed=batch.food_consumed;
 candidate.countries_starved=batch.starved_country_count;
 candidate.original_spring_generated=0u;
 *out=candidate;
 return GK217_OK;
}
