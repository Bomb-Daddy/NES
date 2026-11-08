#ifndef GK1_REPLAY_STORE_H
#define GK1_REPLAY_STORE_H
#include "../execution_039/gk1_battle_session.h"
/* Copyright-clean, bounded RESEARCH replay receipt. Not a full game save. */
typedef enum {
 GK1_STORE_BAD=-3, GK1_STORE_CORRUPT=-2, GK1_STORE_IO=-1,
 GK1_STORE_EXISTS=0, GK1_STORE_OK=1
} Gk1StoreResult;
/* EXCLUSIVE create: never overwrite user/old save. 48-byte CRC+replay contract. */
Gk1StoreResult gk1_store_save_new(const char *path,const Gk1BattleSession *session);
/* Decodes a complete exact-length receipt. Failure is atomic for session. */
Gk1StoreResult gk1_store_load(const char *path,Gk1BattleSession *session);
#endif
