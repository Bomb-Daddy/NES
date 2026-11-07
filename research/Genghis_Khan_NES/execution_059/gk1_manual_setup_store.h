#ifndef GK1_MANUAL_SETUP_STORE_H
#define GK1_MANUAL_SETUP_STORE_H
#include "gk1_setup_draft_store.h"
/* Fully source-manual-constrained inputs; NO character stats/season synthesized. */
#define GK53_PRE_SETUP_BYTES (GK52_PRE_SETUP_BYTES + 36u)
Gk1StoreResult gk53_setup_encode(const Gk1App *app,const Gk1TemplateDataset *source,
                                 uint8_t *out,size_t bytes);
/* Imports legacy 2594/2618 and new 2654-byte setup drafts atomically. */
Gk1StoreResult gk53_setup_decode(const uint8_t *data,size_t bytes,
                                 const Gk1TemplateDataset *source,Gk1App *app);
Gk1StoreResult gk53_setup_save_new(const char *path,const Gk1App *app,
                                   const Gk1TemplateDataset *source);
Gk1StoreResult gk53_setup_load(const char *path,const Gk1TemplateDataset *source,
                               Gk1App *app);
#endif
