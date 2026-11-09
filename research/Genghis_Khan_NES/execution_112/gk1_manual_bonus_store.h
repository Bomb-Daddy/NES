#ifndef GK1_MANUAL_BONUS_STORE_H
#define GK1_MANUAL_BONUS_STORE_H
#include "gk1_manual_setup_store.h"
/* EXEC0055 stores both worlds' 5x6 SIX-ABILITY BONUS DRAFTS. 100 global
 * staged points per world is a conservative input ceiling pending NES
 * runtime verification, not a claim about final original setup math. */
#define GK55_PRE_SETUP_BYTES (GK53_PRE_SETUP_BYTES + 80u)
unsigned gk55_bonus_spent(const Gk1App *app,unsigned world);
Gk1StoreResult gk55_setup_encode(const Gk1App *app,const Gk1TemplateDataset *source,
                                  uint8_t *out,size_t bytes);
Gk1StoreResult gk55_setup_decode(const uint8_t *data,size_t bytes,
                                  const Gk1TemplateDataset *source,Gk1App *app);
Gk1StoreResult gk55_setup_save_new(const char *path,const Gk1App *app,
                                    const Gk1TemplateDataset *source);
Gk1StoreResult gk55_setup_load(const char *path,const Gk1TemplateDataset *source,
                                Gk1App *app);
#endif
