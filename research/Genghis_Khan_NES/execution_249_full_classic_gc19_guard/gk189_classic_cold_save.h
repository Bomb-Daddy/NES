#ifndef GK189_CLASSIC_COLD_SAVE_H
#define GK189_CLASSIC_COLD_SAVE_H
#include "gk1_app_state.h"
/* Owner-private, self-contained continuation of an AUTHENTICALLY ATTACHED
 * original stock first-Spring source checkpoint plus derived/manual Classic
 * ASSIGN/Pass actions. Contains licensed original NES *RAM records*: never
 * distribute .gc19 files or embed them in public source, build or tests.
 * It does not generate an NES first Spring, RNG, AI, or next season. */
typedef enum { GK189_OK=0, GK189_INVALID=1, GK189_SOURCE=2,
 GK189_EXISTS=3, GK189_IO=4, GK189_CORRUPT=5, GK189_MISSING=6 } Gk189Result;
/* Public, data-free verification of an opened private .gc19 save handle. */
int gk249_classic_cold_file_guard(int fd,const char *path);
Gk189Result gk189_save_new(const char *private_path,const Gk1App *app);
/* After process restart: strict canonical original CSV and original 14x83 or
 * 27x83 stock SHA attestation, deterministic replay of all saved actions.
 * On ANY error, *app is byte-identical to its input; no file is modified. */
Gk189Result gk189_cold_load(const char *private_path,const char *csv,Gk1App *app);
#endif
