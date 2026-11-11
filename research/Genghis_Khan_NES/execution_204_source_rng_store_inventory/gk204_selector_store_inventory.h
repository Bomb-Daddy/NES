#ifndef GK204_SELECTOR_STORE_INVENTORY_H
#define GK204_SELECTOR_STORE_INVENTORY_H
#include <stddef.h>
#include <stdint.h>
/* Original USA NES only. All entries are raw PRG byte-sequence candidates,
 * NEVER disassembled reachable instructions, executed BRKs, or Spring RNG.
 * Candidate PC in banks 0..14 is a hypothetical $8000-$BFFF window.
 * The fixed bank (15) is at $C000-$FFFF. */
#define GK204_MAX_CANDIDATES 64u
typedef enum { GK204_OK=0, GK204_ARGUMENT=1,
 GK204_SOURCE_REFUSED=2, GK204_TOO_MANY=3 } Gk204Status;
typedef enum { GK204_RAW_DIRECT_STORE=1, GK204_RAW_DIRECT_RMW=2,
 GK204_RAW_INDEXED_CANDIDATE=3, GK204_RAW_ABSOLUTE_CANDIDATE=4 } Gk204Kind;
typedef struct {
 uint8_t bank;
 uint16_t bank_offset;
 uint16_t hypothetical_pc;
 uint8_t opcode;
 uint8_t kind;
 uint8_t adjacent_lda_immediate;
 uint8_t immediate_value; /* meaningful only when adjacent_lda_immediate */
} Gk204Site;
typedef struct {
 uint8_t verified_original_stock;
 uint8_t source_only_not_runtime;
 uint8_t rng_selector; /* from authenticated GK202 IRQ dispatch table, decimal17 */
 uint8_t site_count;
 uint8_t raw_direct_store_count;
 uint8_t raw_direct_rmw_count;
 uint8_t raw_indexed_count;
 uint8_t raw_absolute_count;
 uint8_t adjacent_lda_rng_store_count;
 Gk204Site sites[GK204_MAX_CANDIDATES];
} Gk204Inventory;
/* Both calls are failure-atomic; no output write on refusal.
 * Never passes private ROM bytes to a report or a public source archive. */
Gk204Status gk204_inventory_bytes(const uint8_t *,size_t,Gk204Inventory *);
Gk204Status gk204_inventory_file(const char *,Gk204Inventory *);
#endif
