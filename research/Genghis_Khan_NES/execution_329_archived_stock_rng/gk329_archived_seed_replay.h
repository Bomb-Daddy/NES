#ifndef GK329_ARCHIVED_SEED_REPLAY_H
#define GK329_ARCHIVED_SEED_REPLAY_H
#include <stddef.h>
#include <stdint.h>
#include "gk327_fixed6502_rng.h"
#include "gk139_mesen_state.h"
/* Archived ORIGINAL Spring snapshots only, never a fresh-reset source.
 * Exact privately extracted named-field data and transient original ROM are
 * required. No original raw bytes are compiled, logged or published. */
typedef enum {
 GK329_OK=0, GK329_ARGUMENT, GK329_NOT_ARCHIVED_SOURCE,
 GK329_BAD_NAMED_FIELDS, GK329_WRONG_RAM_BANK,
 GK329_WRONG_CPU_BOUNDARY, GK329_ROM_OR_CPU_REJECTED,
 GK329_SOURCE_PARITY_FAILURE
} Gk329Status;
typedef struct {
 Gk139MesenProof archived;
 Gk327RngReceipt source_instruction_result;
 uint16_t source_draw15;
 uint8_t scenario_index; /* 0=archived Mongol 1175; 1=archived World 1206 */
 uint8_t evidence_is_archived_restored; /* 1, never a fresh reset */
 uint8_t causal_spring_rng_known; /* always 0 */
 uint8_t first_spring_generated; /* always 0 */
 uint8_t player_orders_granted; /* always 0 */
} Gk329Receipt;
/* Exact historical uncompressed named-field stream SHA + size checked, then
 * original Mesen CPU RAM parsed, source $C289 CPU instructions executed, and
 * independent 48-bit math parity checked. Failure leaves out byte-identical.
 * This is the NEXT draw after an archived Spring checkpoint. It is NOT the
 * unknown draw that generated the checkpoint's Spring Rate or country stats. */
Gk329Status gk329_replay_archived_next_draw(
 uint8_t scenario_index,const uint8_t *original_rom,size_t rom_size,
 const uint8_t *archived_mesen_named_fields,size_t fields_size,
 Gk329Receipt *out);
#endif
