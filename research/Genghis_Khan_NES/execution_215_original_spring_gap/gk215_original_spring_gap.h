#ifndef GK215_ORIGINAL_SPRING_GAP_H
#define GK215_ORIGINAL_SPRING_GAP_H
#include "gk79_stock_checkpoint.h"
#include "gk214_spring_batch.h"
/* EXEC0215: Compare real, SHA-authenticated ORIGINAL-stock first-Spring
 * country states against the inherited ORIGINAL-source bounded finance
 * formula applied to untouched pre-setup templates. The residual is the
 * *unimplemented* setup, RNG, growth, AI, diplomacy/other season processing.
 * This diagnostic NEVER interprets a matching field as a verified complete
 * original Spring. The original stock ROM and raw private RAM are external. */
typedef enum {
 GK215_OK=0,GK215_BAD_ARGUMENT=1,GK215_SOURCE_REFUSED=2,
 GK215_TEMPLATE_REFUSED=3,GK215_FINANCE_REFUSED=4
} Gk215Status;
typedef struct {
 int32_t gold,food,troops,town,masons,artisans,food_workers;
 uint8_t differs;
} Gk215CountryGap;
typedef struct {
 uint8_t profile,scenario,country_count,source_observation_only;
 uint8_t generated_setup,generated_rng,generated_ai,orders_granted;
 uint16_t differing_countries,zero_residual_countries;
 uint16_t gold_mismatches,food_mismatches,troop_mismatches;
 uint16_t workforce_mismatches;
 int64_t gold_residual_sum,food_residual_sum,troop_residual_sum;
 Gk215CountryGap country[GK1_COUNTRIES_WORLD];
} Gk215Gap;
/* Identity must be the canonical independently checked original 41-country
 * CSV; original checkpoint only admits previously SHA-verified private NES
 * WorkRAM / SaveRAM. Inputs and output remain byte-identical on failure. */
Gk215Status gk215_compare_original_spring(const char *canonical_csv,
 const Gk1LoadedScenario *original_template,
 const Gk79Checkpoint *authentic_spring,Gk215Gap *out);
#endif
