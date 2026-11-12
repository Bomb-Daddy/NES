#ifndef GK226_PRACTICE_CHECKPOINT_H
#define GK226_PRACTICE_CHECKPOINT_H
#include "gk224_practice_save.h"
/* EXEC0226: second and later controller-X Enhanced practice saves.
 * Validates BOTH the existing source-bound receipt and replacement state.
 * Writes a same-directory private temporary and atomically replaces the
 * old receipt. Never touches Classic, original ROM, or campaign saves. */
Gk224Status gk226_practice_checkpoint(const char *path,const char *csv,
 const Gk1LoadedScenario *source,const Gk222Spring *state,uint8_t command);
#endif
