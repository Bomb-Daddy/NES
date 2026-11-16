#ifndef GK331_STOCK_OFFER_RNG_H
#define GK331_STOCK_OFFER_RNG_H
#include <stdint.h>
/* Original-USA NES source-authenticated Trade OFFER draw schedule.
 * This does not establish whether a merchant is in the country, the initial
 * game RNG seed, a player-ready first Spring, or a playable order. No original
 * ROM, original RAM, or licensed asset is stored in the public source. */
typedef enum {GK331_OK=0,GK331_BAD=1,GK331_UNVERIFIED_ROM=2,
              GK331_SOURCE_MISMATCH=3,GK331_STAGE=4} Gk331Status;
typedef struct {
 uint8_t buy[3][17];
 uint8_t sell[3][11];
 uint8_t final_seed[6];
 uint16_t draws;
 uint8_t source_verified,offers_generated,merchant_presence_known;
} Gk331Offers;
/* Valid only with original stock ROM SHA gate and EXPLICIT caller-provided
 * source-known seed. The returned masks are candidate source math for exactly
 * that seed; callers must not assume this was the live merchant RNG history. */
Gk331Status gk331_offer_masks(const char *private_original_rom,
                              const uint8_t source_seed[6],Gk331Offers *out);
#endif
