#ifndef GK325_APP_OPENING_BRIDGE_H
#define GK325_APP_OPENING_BRIDGE_H
#include "gk1_app_state.h"
#include "gk325_opening_sequence.h"
/* Read-only gate in the complete inherited game. No PRE_SETUP unlock, seed,
 * country, phase, controller, order or save data mutation. */
Gk325Status gk325_app_audit(const Gk1App *app,const Gk325Trace *trace,Gk325Audit *out);
#endif
