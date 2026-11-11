#ifndef GK193_AUTO_SOURCE_ENTRY_H
#define GK193_AUTO_SOURCE_ENTRY_H
#include "gk1_app_state.h"
#include "gk77_stock_spring.h"
/* Authenticated original Spring continuation from one owner-private source.
 * Recognizes the file's actual content, not its extension or a requested bank.
 * The entry point NEVER synthesizes stock NES setup/RNG or unlocks unsupported
 * commands. The whole native app and detected-profile output are transactional.
 * No private NES ROM/record is bundled, logged or written by this function. */
typedef enum {
 GK193_OK=0, GK193_ARGUMENT=1, GK193_TEMPLATE=2,
 GK193_SOURCE=3, GK193_UNAVAILABLE=4
} Gk193Result;
Gk193Result gk193_enter_original_auto(Gk1App *app, const char *canonical_csv,
 const char *owner_private_source, uint8_t player_index, Gk77Profile *detected);
#endif
