#ifndef GK214_SPRING_BATCH_H
#define GK214_SPRING_BATCH_H
#include "gk75_spring_economy.h"
/* Executes the authentic source-bounded Spring FOOD/TAX/PAY arithmetic for
 * ALL 14/27 countries in one atomic pass. The caller supplies a PREPARED
 * snapshot; this function does not invent setup, initial food bonuses, RNG,
 * ages, growth, AI, order grants or a new season. PRE_SETUP inputs are valid
 * research projections ONLY, not post-setup playable-world evidence. */
typedef enum {GK214_OK=0,GK214_BAD_INPUT=1,GK214_BAD_COUNTRY=2,
              GK214_BAD_FINANCE=3} Gk214Status;
typedef struct {
 Gk1LoadedScenario after;
 Gk75SpringResult country[GK1_COUNTRIES_WORLD];
 uint32_t gross_taxes,payroll,food_consumed;
 uint16_t starved_country_count,troops_lost;
 uint8_t count,source_bounded_only,generated_setup,generated_rng,orders_granted;
} Gk214Batch;
/* No output writes on failure. PRESERVES world.phase and all non-finance
 * fields, including goods0..9 and rulers. Supports distinct 14/27 campaigns.
 * Call only on an appropriately PREPARED state; never interpret this call as
 * proof that a template is an initialized original Spring. */
Gk214Status gk214_project_spring_batch(const Gk1LoadedScenario *prepared,
 Gk214Batch *out);
#endif
