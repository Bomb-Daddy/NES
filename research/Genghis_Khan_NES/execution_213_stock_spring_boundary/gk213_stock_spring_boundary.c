#include "gk213_stock_spring_boundary.h"
#include <string.h>
Gk213Status gk213_check_observed_mongol_boundary(const Gk213Inputs *in,
 const Gk211Report *tr,Gk213Boundary *out){
 if(!in||!tr||!out)return GK213_ARGUMENT;
 /* Caller must obtain tr from the authenticated gk211 private-source
  * reader, not synthesize this structure. The current observed history has
  * a distinct start-food bonus before the Spring feed and a later troop
  * increment. We do not generalize bonus to other rulers or nations. */
 if(tr->validated_stages!=GK211_STAGES||!tr->source_observation_only||
    tr->independent_initialization_implemented||
    tr->observed_food_doubling_step!=9u||
    tr->observed_spring_economy_step!=12u||
    tr->observed_order_grant_step!=14u||
    tr->stage[7].year!=1174u||tr->stage[11].year!=1175u||
    tr->stage[13].orders!=3u)return GK213_UNGROUNDED;
 if(in->stock_pre_food!=tr->stage[7].food||
    in->stock_pre_gold!=tr->stage[7].gold||
    in->stock_pre_troops!=tr->stage[7].troops||
    in->stock_pre_food>499u||
    in->observed_post_season_troops!=tr->stage[11].troops ||
    in->observed_post_season_troops<in->stock_pre_troops)
  return GK213_UNGROUNDED;
 Gk75SpringFields pre={0};
 pre.food=(uint16_t)(2u*in->stock_pre_food);
 pre.gold=in->stock_pre_gold;pre.troops=in->stock_pre_troops;
 pre.town=in->stock_pre_town;pre.masons=in->stock_pre_masons;
 pre.artisans=in->stock_pre_artisans;pre.food_workers=in->stock_pre_food_workers;
 pre.economy=in->observed_post_setup_economy;
 pre.morale=in->observed_post_setup_morale;pre.tax=in->observed_tax;
 if(pre.food!=tr->stage[8].food)return GK213_INCONSISTENT;
 Gk75SpringResult calc;
 if(gk75_spring_food_tax_pay(&pre,&calc)!=GK75_OK||calc.starved||
    calc.after.food!=tr->stage[11].food||
    calc.after.gold!=tr->stage[11].gold||
    calc.after.troops!=in->stock_pre_troops)
  return GK213_INCONSISTENT;
 Gk213Boundary next={0};
 next.food_after_start_bonus=pre.food;
 next.food_after_feeding=calc.after.food;
 next.gold_after_taxes_and_wages=calc.after.gold;
 next.troops_before_growth=calc.after.troops;
 next.troops_after_observed_growth=in->observed_post_season_troops;
 next.tax_gross=calc.gross_tax;next.wages=calc.payroll;
 next.source_observation_only=1;
 /* Source attests input/output, but NOT the provenance of all intermediate
  * economy/morale values, initial RNG seed, or general setup chronology. */
 *out=next;
 return GK213_OK;
}
