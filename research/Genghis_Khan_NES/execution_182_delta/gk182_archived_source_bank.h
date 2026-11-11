#ifndef GK182_ARCHIVED_SOURCE_BANK_H
#define GK182_ARCHIVED_SOURCE_BANK_H
#include "gk144_archived_mapper.h"
#include "gk181_bank_seed_trace.h"
#include "gk77_stock_spring.h"
/* EXEC0182: exact original ROM plus exact original archived Mesen snapshot.
 * Links a real SAVED-INSTANT mapper bank and CPU seed to a conditional
 * static ROM-source scan, without claiming the bank/seed was present at RESET.
 * Never initializes Spring, grants a Classic order, or changes the game.
 * The original stock ROM and Mesen captures remain private.
 */
typedef enum {GK182_OK=0,GK182_ARGUMENT=1,GK182_SOURCE=2,
 GK182_ARCHIVE=3,GK182_STATE=4,GK182_SCAN=5,GK182_UNAVAILABLE=6} Gk182Status;
typedef struct {
 Gk77Profile profile;
 Gk144ArchivedMapper mapped_at_saved_instant;
 uint64_t saved_seed48;
 Gk181Map conditional_source_sites;
 uint8_t original_mss_sha_verified;
 uint8_t stock_rom_sha_verified;
 uint8_t proof_is_saved_instant_only;
 uint8_t first_spring_generated; /* ALWAYS zero */
} Gk182SavedBankProof;
Gk182Status gk182_original_saved_bank(const char *private_rom,
 const char *private_mss,Gk77Profile profile,Gk182SavedBankProof *out);
#endif
