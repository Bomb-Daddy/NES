#ifndef GK329_APP_BRIDGE_H
#define GK329_APP_BRIDGE_H
#include "gk1_app_state.h"
#include "gk329_archived_seed_replay.h"
/* Read-only actual native Gk1App entry. This cannot initialize original NES
 * first Spring, alter source country or grant any controller order. */
Gk329Status gk329_app_archived_rng_next(const Gk1App *app,
 const uint8_t *private_original_rom,size_t rom_size,
 const uint8_t *private_fields,size_t fields_size,Gk329Receipt *out);
#endif
