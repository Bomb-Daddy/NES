#ifndef GK323_ORIGINAL_SPRING_H
#define GK323_ORIGINAL_SPRING_H
#include <stddef.h>
#include <stdint.h>
/* Original-stock captured Spring interpreter: source bytes are supplied by a
 * separate authorized private emulator capture at runtime. No ROM bytes, RAM,
 * graphics, checkpoints or stock country constants are included in this code.
 * Decoding an observation is NOT generating the NES setup/RNG or granting orders.
 */
#define GK323_RECORD_BYTES 83u
#define GK323_MAX_COUNTRIES 27u
#define GK323_ROM_SHA256 "ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3"
typedef enum {GK323_FRESH_RESET=1,GK323_ARCHIVED_RESTORED=2} Gk323Condition;
typedef enum {GK323_WORK_RAM=1,GK323_SAVE_RAM=2} Gk323Bank;
typedef enum {GK323_MONGOL=1,GK323_WORLD=2} Gk323World;
typedef enum {
 GK323_OK=0,GK323_ARGUMENT=1,GK323_SOURCE_MISMATCH=2,
 GK323_LENGTH=3,GK323_FIELD=4,GK323_UNSUPPORTED=5
} Gk323Status;
typedef struct {
 uint16_t arms,skill,troops,economy,town,defense,masons;
 uint16_t goods[10],food,artisans,food_workers,gold,morale,tax;
 uint16_t extra_tax_used,undrafted_residents;
} Gk323Country;
typedef struct {
 uint8_t world,condition,source_bank,country_count;
 uint16_t year;
 uint8_t season; /* original source: 0 = Spring */
 uint8_t source_orders; /* observed original menu only, NOT native orders */
 uint8_t generated_by_port; /* must remain 0 */
 uint8_t port_orders_granted; /* must remain 0 */
 Gk323Country countries[GK323_MAX_COUNTRIES];
} Gk323Spring;
/* rom_sha must be exactly 64 lower-case hexadecimal digits plus terminator;
 * records length EXACTLY country_count*83. Caller supplies provenance from
 * a trusted capture process; this parser cannot cryptographically attest
 * untrusted caller claims. Output remains byte-identical on EVERY error.
 * Fresh world 1206 not captured in verified EXEC0076; fail closed there.
 */
Gk323Status gk323_decode(const char *rom_sha,Gk323World world,
 Gk323Condition condition,Gk323Bank bank,unsigned year,
 unsigned season,unsigned source_orders,
 const uint8_t *records,size_t length,Gk323Spring *output);
#endif
