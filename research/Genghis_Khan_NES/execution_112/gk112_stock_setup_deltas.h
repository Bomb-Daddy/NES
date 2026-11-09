#ifndef GK112_STOCK_SETUP_DELTAS_H
#define GK112_STOCK_SETUP_DELTAS_H
#include "gk111_stock_spring_audit.h"
/* Source-captured original-stock Spring compared against source-authenticated
 * PRE_SETUP fields, for every country. These are COMBINED setup/season/player/
 * RNG/history deltas, NOT inferred initialization formulas or game mutations.
 * All original raw records remain caller-private and are never embedded here. */
#define GK112_METRIC_COUNT 24u
typedef enum {
 GK112_ARMS=0,GK112_SKILL,GK112_TROOPS,GK112_ECONOMY,GK112_TOWN,
 GK112_DEFENSE,GK112_MASONS,GK112_ARTISANS,GK112_FOOD_WORKERS,
 GK112_GOLD,GK112_MORALE,GK112_TAX,GK112_UNDRAFTED,
 GK112_FUR,GK112_SILK,GK112_JEWELS,GK112_METALS,GK112_MEDICINES,
 GK112_WOODWORKS,GK112_CERAMICS,GK112_GLASS,GK112_TEXTILES,
 GK112_PERFUME,GK112_FOOD
} Gk112Metric;
typedef enum {GK112_OK=0,GK112_ARGUMENT=1,GK112_TEMPLATE_REFUSED=2,
              GK112_STOCK_REFUSED=3,GK112_PROFILE_REFUSED=4} Gk112Status;
typedef struct {
 uint8_t profile,scenario_id,country_count;
 uint8_t setup_resolved,first_spring_ready,orders_granted;
 int32_t delta[GK1_COUNTRIES_WORLD][GK112_METRIC_COUNT];
 uint8_t countries_changed[GK112_METRIC_COUNT];
 uint8_t countries_increased[GK112_METRIC_COUNT];
 uint8_t countries_decreased[GK112_METRIC_COUNT];
 uint8_t fields_changed_in_country[GK1_COUNTRIES_WORLD];
 int32_t signed_total[GK112_METRIC_COUNT];
 uint32_t absolute_total[GK112_METRIC_COUNT];
} Gk112StockDeltas;
typedef struct {
 uint8_t country_count,first_difference_country;
 uint8_t fields_changed_in_country[GK1_COUNTRIES_MONGOL];
 uint8_t countries_changed[GK112_METRIC_COUNT];
 int32_t fresh_minus_archived[GK1_COUNTRIES_MONGOL][GK112_METRIC_COUNT];
 uint32_t absolute_total[GK112_METRIC_COUNT];
} Gk112PairedHistories;
const char *gk112_field_name(unsigned field);
/* All changes only committed to *out after entire source/raw integrity passes.
 * Requires exact original CSV and pinned original-stock 14/27x83 capture.
 * Does NOT turn on any of the three playable Spring flags. */
Gk112Status gk112_compare_setup(const char *csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *stock,
 Gk112StockDeltas *out);
/* Specifically the two distinct ORIGINAL Mongol captures; never imply one
 * history is a universal constant, never substitute the modified ROM. */
Gk112Status gk112_compare_mongol_histories(const char *csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *fresh,
 const Gk79Checkpoint *archived,Gk112PairedHistories *out);
#endif
