#ifndef GK212_STOCK_RAW_VARIATION_H
#define GK212_STOCK_RAW_VARIATION_H
#include "gk112_stock_setup_deltas.h"
/* Comparison of TWO distinct authenticated ORIGINAL stock Spring1175 histories.
 * This never identifies causality, RNG consumption order or universal constants.
 * The raw 83-byte records are private caller inputs; output contains only
 * aggregate counts, never licensed/source records or individual byte values. */
#define GK212_RAW_BYTES GK77_RECORD_BYTES
#define GK212_COUNTRIES GK1_COUNTRIES_MONGOL
typedef enum {
 GK212_OK=0,GK212_INVALID=1,GK212_WRONG_PROFILE=2,
 GK212_SOURCE_REFUSED=3,GK212_SETUP_REFUSED=4
} Gk212Status;
typedef struct {
 uint8_t country_count;
 uint16_t cells_compared; /* 14*83 */
 uint16_t differing_cells;
 uint8_t offsets_differing;
 uint8_t countries_differing;
 uint8_t differences_per_offset[GK212_RAW_BYTES];
 uint8_t differences_per_country[GK212_COUNTRIES];
 /* Exact source-projected numeric metrics; NOT an exhaustive raw-field map. */
 uint16_t numeric_metric_differences;
 uint8_t numeric_fields_differing;
 /* 1 marks only an observed equality in the TWO histories.  Never an
  * assertion of an NES invariant over other RNG/choice histories. */
 uint8_t equal_in_two_histories[GK212_RAW_BYTES];
} Gk212Variation;
/* Validates canonical 41-country CSV, pristine pre-setup, exact original 83B
 * country hash, correct WorkRAM-vs-SaveRAM provenance and raw/projected
 * integrity for BOTH states. Atomic refusal: output and inputs untouched. */
Gk212Status gk212_compare_original_mongol_raw(const char *canonical_csv,
 const Gk1LoadedScenario *pre,const Gk79Checkpoint *fresh,
 const Gk79Checkpoint *archived,Gk212Variation *out);
#endif
