#ifndef GK166_FRESH_ENTRY_H
#define GK166_FRESH_ENTRY_H
#include "gk1_app_state.h"
/* An original owner-private fresh WorkRAM observation is projected into
 * the current native app without writing the raw source to disk. This is a
 * checkpoint continuation only, never independent stock NES Spring setup.
 * All PRE_SETUP originals stay unchanged and only VIEW/PASS are executable. */
typedef enum {GK166_OK=0,GK166_ARGUMENT=1,GK166_TEMPLATE=2,
 GK166_SOURCE=3,GK166_SOURCE_UNAVAILABLE=4} Gk166Result;
Gk166Result gk166_enter_original_fresh_json(Gk1App *app,
 const char *canonical_csv,const char *owner_private_json,uint8_t player_index);
#endif
