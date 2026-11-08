#ifndef GK89_SPRING_RECONCILE_H
#define GK89_SPRING_RECONCILE_H
#include "gk79_stock_checkpoint.h"
#include "gk75_spring_economy.h"
#include <stdint.h>
/* Pure, read-only comparison of one independently SHA-authenticated STOCK
 * Spring record to the existing source-verified finance calculation applied
 * to a pristine original PRE_SETUP template. No original RAM/ROM is bundled.
 * Any difference is an UNRESOLVED setup/season/RNG effect, NOT an inferred
 * formula, a generated first Spring, or a gameplay mutation. */
typedef enum {GK89_OK=0,GK89_INVALID=1,GK89_UNVERIFIED=2,
              GK89_TEMPLATE_INVALID=3,GK89_FINANCE_UNAVAILABLE=4} Gk89Status;
typedef struct {
 uint8_t country_id;
 uint16_t template_food,template_gold,template_troops;
 uint16_t finance_food,finance_gold,finance_troops;
 uint16_t observed_food,observed_gold,observed_troops;
 int32_t residual_food,residual_gold,residual_troops;
 uint8_t food_matched,gold_matched,troops_matched;
} Gk89SpringReconciliation;
Gk89Status gk89_compare_spring(const Gk1LoadedScenario *pristine_template,
                              const Gk79Checkpoint *authentic_stock,
                              uint8_t country_index,
                              Gk89SpringReconciliation *output);
#endif
