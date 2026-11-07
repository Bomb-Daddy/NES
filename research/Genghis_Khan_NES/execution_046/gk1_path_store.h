#ifndef GK1_PATH_STORE_H
#define GK1_PATH_STORE_H
#include "gk1_app_state.h"
#include "gk1_replay_store.h"
#include <stddef.h>
#include <stdint.h>
#define GK46_PATH_BYTES 64
int gk46_path_encode(const Gk1App *app,uint8_t *dst,size_t len);
int gk46_path_decode(const uint8_t *src,size_t len,Gk1App *out);
Gk1StoreResult gk46_path_save_new(const char *path,const Gk1App *app);
Gk1StoreResult gk46_path_load(const char *path,Gk1App *out);
#endif
