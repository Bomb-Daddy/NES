#ifndef GK224_PRACTICE_SAVE_H
#define GK224_PRACTICE_SAVE_H
#include "gk222_enhanced_spring_lab.h"
#include <stddef.h>
#include <stdint.h>
/* EXEC0224: ENHANCED PORT-ONLY bounded practice receipt save, NOT stock NES.
 * Canonical 80-byte format (independent of C struct packing/CPU byte order).
 * Record includes source-identity digest and SHA256 integrity, not game ROM.
 * A clean load replays receipts from the current validated source and refuses
 * mismatched user labor drafts, all without changing output or game state. */
#define GK224_PRACTICE_BYTES 80u
typedef enum {GK224_OK=0,GK224_SOURCE,GK224_STATE,GK224_CORRUPT,
              GK224_UNAVAILABLE,GK224_EXISTS} Gk224Status;
Gk224Status gk224_encode(const Gk1LoadedScenario *source,
                        const Gk222Spring *state,uint8_t command,
                        uint8_t out[GK224_PRACTICE_BYTES]);
Gk224Status gk224_decode(const Gk1LoadedScenario *source,
                        const uint8_t *bytes,size_t length,
                        Gk222Spring *out,uint8_t *command);
Gk224Status gk224_save_new(const char *path,const char *csv,
                          const Gk1LoadedScenario *source,
                          const Gk222Spring *state,uint8_t command);
Gk224Status gk224_load(const char *path,const char *csv,
                      const Gk1LoadedScenario *source,
                      Gk222Spring *out,uint8_t *command);
#endif
