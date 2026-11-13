#ifndef GK245_OWNER_SOURCE_H
#define GK245_OWNER_SOURCE_H
#include "gk239_owner_world_mss.h"
#include "gk238_owner_startup.h"
/* Authenticated owner-private stock import for the SAME native C11 app,
 * never a generated original Spring. Both OS entrypoints must use this gate.
 * No source paths are retained, copied into public logs or written to disk. */
typedef enum {
 GK245_NO_REQUEST=0, GK245_FRESH_IMPORTED=1, GK245_ARCHIVED_IMPORTED=2,
 GK245_FRESH_REFUSED=3, GK245_ARCHIVED_REFUSED=4,
 GK245_CONFLICT=5, GK245_ISOLATED=6
} Gk245Outcome;
Gk245Outcome gk245_owner_source_startup(Gk1App *app,const char *canonical_csv,
 int loaded,int diagnostic,const char *fresh_workram_json,
 const char *archived_world_saveram_mss);
#endif
