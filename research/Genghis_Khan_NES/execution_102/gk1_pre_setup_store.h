#ifndef GK1_PRE_SETUP_STORE_H
#define GK1_PRE_SETUP_STORE_H
#include "gk1_app_state.h"
#include "gk1_replay_store.h"
/* EXEC0051: cross-platform PRE_SETUP ONLY, source-identity gated.
 * Explicitly NOT a playable first-Spring or complete campaign save. */
#define GK51_PRE_SETUP_BYTES 2594u
Gk1StoreResult gk51_pre_setup_encode(const Gk1App *app,const Gk1TemplateDataset *original,
                                     uint8_t *data,size_t size);
Gk1StoreResult gk51_pre_setup_decode(const uint8_t *data,size_t size,
                                     const Gk1TemplateDataset *original,Gk1App *app);
Gk1StoreResult gk51_pre_setup_save_new(const char *file,const Gk1App *app,
                                       const Gk1TemplateDataset *original);
Gk1StoreResult gk51_pre_setup_load(const char *file,const Gk1TemplateDataset *original,
                                   Gk1App *app);
#endif
