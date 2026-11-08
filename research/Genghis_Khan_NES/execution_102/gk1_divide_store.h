#ifndef GK48_DIVIDE_STORE_H
#define GK48_DIVIDE_STORE_H
#include "gk1_app_state.h"
#include "gk1_replay_store.h"
#include <stdint.h>
#include <stddef.h>
/* Exactly 64 bytes. Source-event replay only, approved Enhanced ROM. */
#define GK48_DIVIDE_BYTES 64
int gk48_divide_encode(const Gk1App *app,uint8_t *dst,size_t len);
int gk48_divide_decode(const uint8_t *src,size_t len,Gk1App *out);
Gk1StoreResult gk48_divide_save_new(const char *path,const Gk1App *app);
Gk1StoreResult gk48_divide_load(const char *path,Gk1App *app);
#endif
