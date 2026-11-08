#ifndef GK75_SPRING_ECONOMY_H
#define GK75_SPRING_ECONOMY_H
#include <stdint.h>
#include "../execution_049/gk1_scenario_factory.h"
/* Physical original English NES source, bank1 $8099/$8106 and fixed-bank
 * percentage helpers, documented in v1.07 annual-engine original 6502 replay.
 * This is ONLY the bounded Spring FOOD -> TAX -> PAYROLL pair; no setup,
 * pre-season common labor, sickness, age/growth, RNG, AI or state transition.
 * All input fields are ordinary game-range 0..999. No ROM bytes included.
 */
typedef enum {GK75_OK=0,GK75_BAD_INPUT=1} Gk75Result;
typedef struct {
  uint16_t troops,town,masons,artisans,food_workers;
  uint16_t food,gold,economy,morale,tax;
} Gk75SpringFields;
typedef struct {
  Gk75SpringFields after;
  uint16_t food_consumed,gross_tax,payroll,troops_lost;
  uint8_t starved;
} Gk75SpringResult;
/* Atomic: no modification to *out on any failure, and input is const. */
Gk75Result gk75_spring_food_tax_pay(const Gk75SpringFields *in,Gk75SpringResult *out);
/* Preview from an uninitialized original CSV country is expressly NOT a
 * source-authentic first Spring or permission to change game phase/state. */
Gk75Result gk75_template_projection(const Gk1CountryTemplate *country,Gk75SpringResult *out);
#endif
