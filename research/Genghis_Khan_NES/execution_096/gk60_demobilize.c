#include "gk60_demobilize.h"
#include <stdint.h>
/* Approved modified-ROM contract: People > Troops > Demobilize moves q from
 * Troops to undrafted reserve; q <= min(Troops,999-reserve). No automatic
 * assignment to another occupation, gold payment, season or turn advancement.
 * This standalone function updates derived fields of the numeric country
 * record so all existing EXEC0049 loader/EXEC0051 save invariants hold. */
static int consistent(const Gk1CountryTemplate *c){
 if(!c || c->undrafted_reserve>999 || c->eligibility_percent>100)return 0;
 uint32_t draft=(uint32_t)c->troops+c->town+c->masons+c->artisans+c->food_workers;
 return draft==c->drafted_population &&
   draft+c->undrafted_reserve==c->combined_population &&
   ((uint32_t)c->undrafted_reserve*c->eligibility_percent)/100u==c->eligible_draft_count;
}
uint16_t gk60_demob_max(const Gk1CountryTemplate *country){
 if(!consistent(country))return 0;
 uint16_t cap=(uint16_t)(999u-country->undrafted_reserve);
 return country->troops<cap?country->troops:cap;
}
Gk60DemobStatus gk60_demob_apply(Gk1CountryTemplate *country,uint16_t qty){
 if(!country || !qty || !consistent(country) || qty>gk60_demob_max(country))
  return GK60_DEMOB_REFUSED;
 Gk1CountryTemplate next=*country;
 next.troops=(uint16_t)(next.troops-qty);
 next.undrafted_reserve=(uint16_t)(next.undrafted_reserve+qty);
 next.drafted_population=(uint16_t)(next.drafted_population-qty);
 next.eligible_draft_count=(uint16_t)((uint32_t)next.undrafted_reserve*next.eligibility_percent/100u);
 if(!consistent(&next))return GK60_DEMOB_REFUSED;
 *country=next;
 return GK60_DEMOB_OK;
}
