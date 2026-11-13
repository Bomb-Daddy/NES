#ifndef GK238_OWNER_STARTUP_H
#define GK238_OWNER_STARTUP_H
#include "gk237_owner_fresh_win.h"
/* Controller startup gate. Owner paths are never opened in bounded CI smoke
 * runs, and source-import refusal never mutates an app or canonical templates. */
typedef enum {
 GK238_UNREQUESTED=0,
 GK238_OWNER_IMPORTED=1,
 GK238_REFUSED=2,
 GK238_SMOKE_ISOLATED=3
} Gk238Startup;
Gk238Startup gk238_owner_startup(Gk1App *app,const char *canonical_csv,
 const char *private_json,int scenario_loaded,int diagnostic_mode);
#endif
