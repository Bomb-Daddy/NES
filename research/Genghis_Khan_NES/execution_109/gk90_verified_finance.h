#ifndef GK90_VERIFIED_FINANCE_H
#define GK90_VERIFIED_FINANCE_H
#include "gk89_spring_reconcile.h"
/* PUBLIC research CSV identity and its 41 complete PRE_SETUP country records
 * are verified BEFORE source finance analysis. The already-authenticated
 * private NES Spring checkpoint is independently checked by GK89.
 * No country, output, checkpoint, or order state is changed on refusal.
 * This is a provenance gate, NOT a generated original first Spring. */
typedef enum {
 GK90_OK=0, GK90_ARGUMENT=1, GK90_CSV_NOT_AUTHENTIC=2,
 GK90_TEMPLATE_DRIFT=3, GK90_STOCK_REFUSED=4
} Gk90Result;
Gk90Result gk90_compare_spring_verified(const char *original_csv_path,
  const Gk1LoadedScenario *claimed_pre_setup,
  const Gk79Checkpoint *original_stock_spring,
  uint8_t country_index,Gk89SpringReconciliation *out);
#endif
