#ifndef GK217_FINANCE_PREVIEW_H
#define GK217_FINANCE_PREVIEW_H
#include "gk214_spring_batch.h"
/* Copyright-clean, player-facing Classic finance explorer. This is an actual
 * executable model of food/tax/pay over 14/27 country template data, NOT a
 * reconstructed original NES setup, RNG scheduler, or playable first Spring.
 * All results are value-only; no app state, orders or seasons are mutated. */
typedef enum { GK217_OK=0, GK217_INVALID=1, GK217_FINANCE_REFUSED=2 } Gk217Status;
typedef struct {
  uint8_t country_count, selected_country, original_spring_generated;
  uint16_t before_gold, after_gold, before_food, after_food;
  uint16_t before_troops, after_troops;
  uint16_t troops_lost;
  uint32_t taxes, payroll, food_consumed;
  uint32_t total_taxes, total_payroll, total_food_consumed;
  uint16_t countries_starved;
} Gk217Preview;
/* Atomic failure: leaves the caller's output byte-for-byte unchanged.
 * country_index is zero-based; world must be a native, legitimate 14/27-country
 * PRE_SETUP dataset with fixed source country identities. */
Gk217Status gk217_preview_country(const Gk1LoadedScenario *world,
                                  unsigned country_index,Gk217Preview *out);
#endif
