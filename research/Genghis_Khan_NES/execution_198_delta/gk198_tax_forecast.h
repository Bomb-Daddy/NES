#ifndef GK198_TAX_FORECAST_H
#define GK198_TAX_FORECAST_H
#include "gk187_classic_assign.h"
#include "gk75_spring_economy.h"
/* Authenticated Classic Spring WHAT-IF using the already source-traced bank1
 * $8099/$8106 food/tax/wage kernel. This is not a next-season simulation:
 * no original ruler attributes, RNG, age/growth, AI or seasonal transitions.
 * Pre-season country and original 83-byte NES records remain immutable.
 */
typedef enum {GK198_OK=0,GK198_INVALID=1,GK198_SOURCE=2,
              GK198_ARITHMETIC=3} Gk198Result;
typedef struct {
 uint8_t proposed_rate,committed_rate;
 uint16_t before_gold,before_food;
 Gk75SpringResult committed,proposed;
 int32_t gold_delta;
 uint8_t action_count,remaining;
} Gk198Forecast;
/* For unit tests and offline numeric analysis only: NOT an original game turn. */
Gk198Result gk198_compare_fields(const Gk75SpringFields *fields,
                                uint8_t proposed_rate,Gk198Forecast *out);
/* Production UI must take its fields from a valid exact-stock authenticated
 * checkpoint and the replay-validated Classic derived player country. */
Gk198Result gk198_classic_tax_forecast(const Gk187ClassicAssign *classic,
 const Gk79Checkpoint *stock,uint8_t proposed_rate,Gk198Forecast *out);
#endif
