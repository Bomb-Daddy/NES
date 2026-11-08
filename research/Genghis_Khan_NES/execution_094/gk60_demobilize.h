#ifndef GK60_DEMOBILIZE_H
#define GK60_DEMOBILIZE_H
#include "../execution_049/gk1_scenario_factory.h"
#include <stdint.h>
/* Source-verified Enhanced-only Demobilize arithmetic, integrated with the
 * existing native app.  These functions do not advance a season or validate
 * NES action costs.  The caller must separately enforce profile/phase. */
typedef enum { GK60_DEMOB_OK=0, GK60_DEMOB_REFUSED=1 } Gk60DemobStatus;
uint16_t gk60_demob_max(const Gk1CountryTemplate *country);
/* Atomic: even forged inconsistent country records are never modified. */
Gk60DemobStatus gk60_demob_apply(Gk1CountryTemplate *country,uint16_t qty);
#endif
