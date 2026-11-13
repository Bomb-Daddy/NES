#ifndef GK237_OWNER_FRESH_WIN_H
#define GK237_OWNER_FRESH_WIN_H
#include "gk1_app_state.h"
/* EXEC0237 — portable owner-activated fresh-stock WorkRAM handoff.  This
 * exposes EXISTING authenticated GK166 checkpoint continuation to WinMain.
 * No independent first-Spring generation, invented RNG or automatic import.
 * The caller passes an explicit local owner-private file path. */
typedef enum {
 GK237_NOT_REQUESTED=0, GK237_IMPORTED=1, GK237_REFUSED_CONTEXT=2,
 GK237_REFUSED_SOURCE=3, GK237_INVALID_ARGUMENT=4
} Gk237OwnerResult;
Gk237OwnerResult gk237_owner_fresh_boot(Gk1App *app,
 const char *canonical_scenario_csv,const char *owner_private_json);
#endif
