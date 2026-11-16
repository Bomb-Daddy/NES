#ifndef GK319_CPU_TRACE_MATH_H
#define GK319_CPU_TRACE_MATH_H
#include "gk316_cpu_claims_ingest.h"
#include <stdint.h>
/* Original-stock CPU event CLAIMS -> exact existing C289 and C106 models.
 * A claimed trace is NOT proof of execution in Mesen or complete first Spring.
 * Never changes Gk1App, original country statistics, game phase or orders. */
typedef enum {
 GK319_OK=0, GK319_ARGUMENT=1, GK319_CLAIM=2,
 GK319_CHRONOLOGY=3, GK319_SEED_UNPROVEN=4, GK319_PARITY=5,
 GK319_SOURCE=6
} Gk319Status;
typedef struct {
 uint8_t world;
 uint8_t first_seed_at_call[6], final_seed[6];
 uint16_t source_stores, nmis_enabled, nmis_gated, c289_calls;
 uint64_t first_draw_cycle, last_draw_cycle;
 uint8_t source_math_parity; /* verified model of the supplied CLAIM, not provenance */
 uint8_t observed_stock_emulator, first_spring_generated, player_orders_granted; /* ALWAYS 0 */
} Gk319Receipt;
/* Require six explicit CPU STORE source claims before first C289. Subsequent
 * stores and NMI (zero/nonzero $8A gates) are accounted for IN ORDER.  Checks
 * every STORE prior byte and the 15-bit C289 draw and entire 48-bit after
 * state. Fails atomically; never promotes claims to authenticated gameplay. */
Gk319Status gk319_reconcile_cpu_trace_math(const Gk316Trace *trace,
                                            Gk319Receipt *out);
#endif
