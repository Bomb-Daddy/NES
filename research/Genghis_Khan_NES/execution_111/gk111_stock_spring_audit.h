#ifndef GK111_STOCK_SPRING_AUDIT_H
#define GK111_STOCK_SPRING_AUDIT_H
#include "gk110_spring_stage.h"
#include "gk79_stock_checkpoint.h"
/* EXEC0111: full world original-stock versus source-derived finance GAP.
 * A real 14/27-country SHA-authenticated private original NES Mesen Spring
 * checkpoint is REQUIRED; no canned Spring constants or ROM bytes ship.
 * Residuals are unaccounted-for combined setup/season/RNG/AI effects,
 * never inferred formulas or permission to initialize playable Spring. */
typedef enum {
 GK111_OK=0,GK111_ARGUMENT=1,GK111_SOURCE_REFUSED=2,
 GK111_TEMPLATE_DRIFT=3,GK111_STOCK_REFUSED=4,
 GK111_FINANCE_REFUSED=5
} Gk111Result;
typedef struct {
 uint8_t profile,scenario_id,country_count;
 uint8_t setup_resolved,first_spring_ready,orders_granted;
 uint8_t food_exact,gold_exact,troops_exact,all_three_exact;
 uint8_t food_mismatch,gold_mismatch,troops_mismatch;
 uint8_t first_difference_country; /* one-based; 0 iff no difference */
 int32_t food_residual[GK1_COUNTRIES_WORLD];
 int32_t gold_residual[GK1_COUNTRIES_WORLD];
 int32_t troops_residual[GK1_COUNTRIES_WORLD];
 int32_t food_residual_total,gold_residual_total,troops_residual_total;
 uint32_t food_absolute_total,gold_absolute_total,troops_absolute_total;
} Gk111StockSpringAudit;
/* No input or output state changes on refusal; read-only on success.
 * A caller cannot authenticate a fabricated checkpoint by setting flags:
 * private full raw SHA must match a historically verified original profile. */
Gk111Result gk111_compare_stock_spring_world(const char *verified_csv,
 const Gk1LoadedScenario *pristine_template,const Gk79Checkpoint *stock,
 Gk111StockSpringAudit *out);
#endif
