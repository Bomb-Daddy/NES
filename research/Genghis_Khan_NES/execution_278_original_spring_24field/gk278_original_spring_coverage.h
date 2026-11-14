#ifndef GK278_ORIGINAL_SPRING_COVERAGE_H
#define GK278_ORIGINAL_SPRING_COVERAGE_H
#include "gk215_original_spring_gap.h"
/* EXEC0278: exhaustive *DIRECT* fields actually decoded from the original
 * 83-byte country record into Gk1CountryTemplate. The port-derived reserve,
 * population and history fields are intentionally excluded. No ROM assets.
 * Index order is stable for independent QA and subsequent source tracing. */
typedef enum {
 GK278_ARMS, GK278_SKILL, GK278_TROOPS, GK278_ECONOMY,
 GK278_TOWN, GK278_DEFENSE, GK278_MASONS, GK278_ARTISANS,
 GK278_FOOD_WORKERS, GK278_GOLD, GK278_MORALE, GK278_TAX,
 GK278_UNDRAFTED, GK278_FUR, GK278_SILK, GK278_JEWELS,
 GK278_METALS, GK278_MEDICINES, GK278_WOODWORKS,
 GK278_CERAMICS, GK278_GLASS, GK278_TEXTILES, GK278_PERFUME,
 GK278_FOOD, GK278_FIELD_COUNT
} Gk278Field;
typedef enum {GK278_OK=0,GK278_ARGUMENT=1,GK278_TEMPLATE=2,
 GK278_SOURCE=3,GK278_PROJECTION=4} Gk278Status;
typedef struct {
 uint16_t unequal_countries;
 int64_t signed_residual;
 int32_t minimum_residual,maximum_residual;
} Gk278FieldGap;
typedef struct {
 uint8_t profile,scenario,country_count,source_observation_only;
 uint8_t independent_initialization,generated_rng,generated_ai,playable_order_granted;
 uint16_t compared_cells,unequal_cells,exact_cells;
 uint16_t countries_with_differences;
 uint16_t unequal_fields_per_country[GK1_COUNTRIES_WORLD];
 Gk278FieldGap field[GK278_FIELD_COUNT];
} Gk278Coverage;
/* Exact authenticated stock source, canonical untouched template, same
 * source-only Spring finance projection as EXEC0214/0215. No state mutation.
 * On ANY refusal, out is byte-for-byte unchanged. This does not infer the
 * original first-Spring initialization or treat projections as gameplay. */
Gk278Status gk278_compare_original_spring_coverage(const char *canonical_csv,
 const Gk1LoadedScenario *untouched_pre_setup,
 const Gk79Checkpoint *original_stock_spring,Gk278Coverage *out);
const char *gk278_field_name(Gk278Field field);
#endif
