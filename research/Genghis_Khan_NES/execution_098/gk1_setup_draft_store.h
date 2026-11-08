#ifndef GK1_SETUP_DRAFT_STORE_H
#define GK1_SETUP_DRAFT_STORE_H
#include "gk1_pre_setup_store.h"
/* Extension layered over the unmodified 2594-byte EXEC0051 state.  It records
 * player setup INTENT only, never applies an unverified NES setup transform. */
#define GK52_PRE_SETUP_BYTES (GK51_PRE_SETUP_BYTES + 24u)
Gk1StoreResult gk52_setup_encode(const Gk1App *app,const Gk1TemplateDataset *original,
                                  uint8_t *out,size_t size);
/* Both v1 2594-byte and v2 2618-byte saves are accepted.  Failures are atomic. */
Gk1StoreResult gk52_setup_decode(const uint8_t *data,size_t size,
                                  const Gk1TemplateDataset *original,Gk1App *app);
Gk1StoreResult gk52_setup_save_new(const char *path,const Gk1App *app,
                                    const Gk1TemplateDataset *original);
Gk1StoreResult gk52_setup_load(const char *path,const Gk1TemplateDataset *original,Gk1App *app);
#endif
