#ifndef GK321_FIRST_SPRING_PHASE_H
#define GK321_FIRST_SPRING_PHASE_H
#include <stdint.h>
/* Integrates the already verified GK75 6502 Spring FOOD->TAX->PAYROLL
 * calculation with an explicit, separate later personnel/growth snapshot.
 * This is executable source-bound partial initialization, NOT the
 * original game's missing character RNG, scenario setup or usable orders. */
typedef struct {
  uint16_t troops,town,masons,artisans,food_workers;
  uint16_t food,gold,economy,morale,tax;
} Gk321BeforeFinance;
typedef struct {
  uint16_t troops,town,masons,artisans,food_workers;
} Gk321AfterPersonnel;
typedef struct {
  uint16_t food,gold,paid_troops,food_consumed,gross_tax,payroll;
  uint16_t troops_after_personnel,town_after_personnel,masons_after_personnel;
  uint16_t artisans_after_personnel,food_workers_after_personnel;
  uint8_t observed_stock_emulator; /* ALWAYS ZERO: arithmetic candidate only */
  uint8_t first_spring_generated; /* ALWAYS ZERO: complete setup unimplemented */
  uint8_t player_orders_granted; /* ALWAYS ZERO */
} Gk321PhaseResult;
typedef enum {
  GK321_OK=0,GK321_ARGUMENT=1,GK321_INCONSISTENT=2,
  GK321_NEEDS_SOURCE=3
} Gk321Status;
/* Input before_finance is a proposed earlier event state; growth is later.
 * A full original initializer must source both from the exact stock ROM,
 * including difficulty, ruler, ability, and RNG branches. All arithmetic
 * is computed, NOT set to observed sample values. Out unchanged on error.
 * Here we reject later personnel counts below paid troops, but do not
 * invent which personnel events occurred. */
Gk321Status gk321_reconcile_phases(const Gk321BeforeFinance *before,
                      const Gk321AfterPersonnel *later,
                      Gk321PhaseResult *out);
/* Compare a candidate's arithmetic with an independently observed source
 * checkpoint, never grant an order or mutate the source record. */
Gk321Status gk321_compare_observed(const Gk321PhaseResult *candidate,
                       uint16_t actual_food,uint16_t actual_gold);
#endif
