#ifndef GK227_PRACTICE_TITLE_PREVIEW_H
#define GK227_PRACTICE_TITLE_PREVIEW_H
#include "gk1_app_state.h"
#include <stdint.h>
/* Enhanced PORT-only title inventory. Does not change the running game or
 * certify NES first-Spring, RNG, historical checkpoints or full saves. */
typedef enum {
 GK227_PREVIEW_OK=0, GK227_PREVIEW_MISSING, GK227_PREVIEW_SOURCE,
 GK227_PREVIEW_CORRUPT, GK227_PREVIEW_UNAVAILABLE
} Gk227PreviewStatus;
typedef struct {
 uint8_t scenario, country_count, player_index;
 uint8_t actions_committed, orders_remaining, command;
} Gk227Preview;
/* The two scenario templates must be the complete unmodified 14/27-country
 * public original CSV-derived set. On EVERY failure, *out is byte-unchanged.
 * Rejects symlinks/reparse points, directories, truncated/extended records,
 * invalid source, and forged receipt. Does not touch Gk1App. */
Gk227PreviewStatus gk227_preview_practice_slot(const char *path,
 const char *csv,const Gk1LoadedScenario sources[2],Gk227Preview *out);
#endif
