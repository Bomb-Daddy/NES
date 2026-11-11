#ifndef GK177_ENHANCED_RECEIPT_H
#define GK177_ENHANCED_RECEIPT_H
#include "gk176_enhanced_cycle.h"
/* PORT-ONLY source-attested Enhanced labor history, not original NES save/turns.
 * Requires the exact private original-stock checkpoint to cold-load.
 * 448-byte canonical, replayable, fixed endian on-disk receipt. */
#define GK177_BYTES 448u
typedef enum {GK177_OK=0,GK177_INPUT,GK177_SOURCE,GK177_CORRUPT,
              GK177_IO,GK177_EXISTS} Gk177Status;
typedef struct {
 Gk116EnhancedAssign spring;
 Gk158EnhancedSummer summer;
 Gk159EnhancedFall fall;
 Gk160EnhancedWinter winter;
 Gk161EnhancedYear2 year2;
 Gk176EnhancedCycle continuation;
} Gk177Stages;
/* Atomic decode: outputs unchanged on every failure, including forged action
 * counts, stale stock source, noncanonical reserved bytes, SHA mismatch. */
Gk177Status gk177_encode(const Gk79Checkpoint *original,
 const Gk177Stages *stages,uint8_t bytes[GK177_BYTES]);
Gk177Status gk177_decode(const Gk79Checkpoint *original,uint8_t enhanced,
 const uint8_t *bytes,size_t length,Gk177Stages *out);
Gk177Status gk177_save_new(const char *path,const Gk79Checkpoint *original,
 const Gk177Stages *stages);
Gk177Status gk177_cold_load(const char *path,const Gk79Checkpoint *original,
 uint8_t enhanced,Gk177Stages *out);
#endif
