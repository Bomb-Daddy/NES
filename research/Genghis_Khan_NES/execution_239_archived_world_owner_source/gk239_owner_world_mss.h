#ifndef GK239_OWNER_WORLD_MSS_H
#define GK239_OWNER_WORLD_MSS_H
#include "gk1_app_state.h"
/* Owner-only archived original World Spring 1206 SaveRAM checkpoint, NOT a
 * fresh reset, generated NES setup/RNG or a release-complete campaign. */
typedef enum {
 GK239_UNREQUESTED=0, GK239_IMPORTED=1, GK239_REFUSED=2,
 GK239_SMOKE_ISOLATED=3, GK239_CONFLICTING_SOURCES=4
} Gk239Status;
/* Both source selectors originate from explicit owner-local environment
 * variables. 'fresh_json' is the existing EXEC0238 WorkRAM import selector.
 * Refuse ambiguous simultaneous selection without opening either file. */
Gk239Status gk239_owner_world_startup(Gk1App *app,
 const char *canonical_csv,const char *world_mss,const char *fresh_json,
 int scenario_loaded,int diagnostic_mode);
#endif
