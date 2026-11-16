#include "gk321_first_spring_phase.h"
#include "gk75_spring_economy.h"
#include <limits.h>
#include <string.h>
Gk321Status gk321_reconcile_phases(const Gk321BeforeFinance *before,
                      const Gk321AfterPersonnel *later,
                      Gk321PhaseResult *out){
 if(!before||!later||!out)return GK321_ARGUMENT;
 if(later->troops>999u||later->town>999u||later->masons>999u||
    later->artisans>999u||later->food_workers>999u)return GK321_INCONSISTENT;
 unsigned later_sum=(unsigned)later->troops+later->town+later->masons+
                    later->artisans+later->food_workers;
 if(later_sum>999u)return GK321_INCONSISTENT;
 Gk75SpringFields old={before->troops,before->town,before->masons,
   before->artisans,before->food_workers,before->food,before->gold,
   before->economy,before->morale,before->tax};
 Gk75SpringResult step={0};
 if(gk75_spring_food_tax_pay(&old,&step)!=GK75_OK)return GK321_INCONSISTENT;
 /* This is not a full growth/birth model. A later observation with fewer
  * soldiers could reflect other events, but is not this bounded ordering.
  * Do not pretend it proves an NES RNG result. */
 if(later->troops<step.after.troops)return GK321_NEEDS_SOURCE;
 Gk321PhaseResult v={0};
 v.food=step.after.food;v.gold=step.after.gold;
 v.paid_troops=step.after.troops;v.food_consumed=step.food_consumed;
 v.gross_tax=step.gross_tax;v.payroll=step.payroll;
 v.troops_after_personnel=later->troops;
 v.town_after_personnel=later->town;
 v.masons_after_personnel=later->masons;
 v.artisans_after_personnel=later->artisans;
 v.food_workers_after_personnel=later->food_workers;
 *out=v;return GK321_OK;
}
Gk321Status gk321_compare_observed(const Gk321PhaseResult *c,
                      uint16_t food,uint16_t gold){
 if(!c)return GK321_ARGUMENT;
 if(c->observed_stock_emulator||c->first_spring_generated||
    c->player_orders_granted)return GK321_NEEDS_SOURCE;
 return c->food==food&&c->gold==gold?GK321_OK:GK321_INCONSISTENT;
}
