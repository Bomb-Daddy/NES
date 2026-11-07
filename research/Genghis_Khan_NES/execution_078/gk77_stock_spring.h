#ifndef GK77_STOCK_SPRING_H
#define GK77_STOCK_SPRING_H
#include <stddef.h>
#include <stdint.h>
/* A read-only FIRST-SPRING ORACLE from *actual* ORIGINAL STOCK NES Mesen runs.
 * It is not a game-state initializer. No original ROM, country records, or
 * user checkpoint bytes are compiled or published; hashes are receipt IDs.
 * Exact 83-byte records must be supplied out-of-band by their owner.
 * Fresh Spring1175 active bank: nesWorkRam physical $0DCD.
 * Restored Spring1175/1206 bank: nesSaveRam physical $0DCD.
 * Historical source iNES SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3.
 */
#define GK77_RECORD_BYTES 83u
#define GK77_MAX_COUNTRIES 27u
typedef enum {
 GK77_FRESH_MONGOL_WORKRAM=1,
 GK77_ARCHIVED_MONGOL_SAVERAM=2,
 GK77_ARCHIVED_WORLD_SAVERAM=3
} Gk77Profile;
typedef enum {GK77_OK=0,GK77_INVALID=1,GK77_IO=2,GK77_SIZE=3,GK77_SHA=4} Gk77Result;
typedef struct {
 uint16_t arms,skill,troops,economy,town,defense,masons,food,artisans,
          food_workers,gold,morale,tax,extra_tax,undrafted;
 uint16_t goods[10]; /* Fur..Perfume, original +0x22 + 2*i; Food is +0x36 */
} Gk77Country;
typedef struct {
 Gk77Profile profile;
 uint8_t scenario,country_count,orders;
 uint16_t year,rate;
 Gk77Country countries[GK77_MAX_COUNTRIES];
} Gk77StockSpring;
/* Decode 83 bytes from a private ROM-derived record; no identity asserted. */
Gk77Result gk77_decode(const uint8_t *record,size_t length,Gk77Country *out);
/* SHA-bound actual stock evidence only, with atomic refusal on all errors. */
Gk77Result gk77_load(const char *path,Gk77Profile profile,Gk77StockSpring *out);
Gk77Profile gk77_profile_parse(const char *name);
const char *gk77_profile_name(Gk77Profile profile);
#endif
