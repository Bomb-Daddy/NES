#ifndef GK199_CLASSIC_TAX_UNDO_H
#define GK199_CLASSIC_TAX_UNDO_H
#include "gk187_classic_assign.h"
/* Port-only controller convenience. The authentic NES has not been shown to
 * permit TAX Undo, and this never claims to reproduce unknown original
 * Leadership, Charm, morale or RNG effects.
 * Only a replay-attested, last-unpassed TAX action can be reversed.
 * On refusal both state and selection remain byte-identical. */
Gk187Result gk199_undo_last_tax(Gk187ClassicAssign *state,
 const Gk79Checkpoint *source, uint8_t *selection);
#endif
