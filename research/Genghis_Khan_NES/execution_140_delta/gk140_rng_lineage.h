#ifndef GK140_RNG_LINEAGE_H
#define GK140_RNG_LINEAGE_H
#include <stdint.h>
#include "gk139_mesen_state.h"
/* A compatibility calculation, NOT a proof of intervening PRNG calls.
 * CPU-RAM state may be perturbed by NMI, game setup, or unknown writes.
 * Never turn these matches into a player-ready original Spring. */
typedef enum {
 GK140_LINEAGE_OK=0, GK140_LINEAGE_BAD_INPUT=1,
 GK140_LINEAGE_NO_PURE_CHAIN=2
} Gk140Result;
typedef struct {
 uint32_t tested_transitions;
 uint32_t compatible_counts;
 uint32_t nearest_count;
 uint32_t farthest_count;
 uint64_t elapsed_cpu_cycles;
 uint8_t only_prng_is_unproven; /* always true even for a unique match */
} Gk140Lineage;
/* Observations must be externally authenticated and belong to one scenario,
 * to a monotonic CPU timeline and a single physical country RAM bank.
 * Search counts 0..maximum_draws inclusive; capped at 65,536.
 * On any rejection, *out is unchanged. */
Gk140Result gk140_compare_mesen_seed_lineage(const Gk139MesenProof *earlier,
 const Gk139MesenProof *later,uint32_t maximum_draws,Gk140Lineage *out);
#endif
