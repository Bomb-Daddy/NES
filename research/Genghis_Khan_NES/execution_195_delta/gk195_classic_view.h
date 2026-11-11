#ifndef GK195_CLASSIC_VIEW_H
#define GK195_CLASSIC_VIEW_H
#include "gk1_app_state.h"
/* Controller VIEW combines an immutable authenticated NES source with a
 * separately replay-validated Classic allocation. No new NES first Spring,
 * RNG or stock-accurate action is asserted by this derived view. */
typedef enum {GK195_OK=0,GK195_INVALID=1,GK195_SOURCE=2} Gk195ViewResult;
typedef struct {
 Gk1CountryTemplate original, current;
 uint8_t country_index, is_player, derived, actions, remaining, passed;
} Gk195CountryView;
Gk195ViewResult gk195_classic_country_view(const Gk1App *app,
 uint8_t country_index,Gk195CountryView *out);
#endif
