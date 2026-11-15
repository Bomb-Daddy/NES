#ifndef GK312_STOCK_EVENT_PC_H
#define GK312_STOCK_EVENT_PC_H
#include "gk304_stock_store_value.h"
#include <stdint.h>
#include <stddef.h>
/* Additional opt-in CPU-PC provenance for an externally supplied original
 * NES startup event stream. It is deliberately stricter than the older
 * GK302/GK304 math-only claim format. Witnesses are NOT emulator proof. */
typedef struct {
 uint64_t cpu_cycle;
 uint16_t observed_pc; /* NMI: $C106 gate LDA, C289: entry $C289 */
 uint8_t mapped_prg_bank; /* original fixed bank = 15 */
} Gk312PcWitness;
typedef enum { GK312_OK=0,GK312_ARGUMENT=1,GK312_ORIGIN=2,
 GK312_SOURCE=3,GK312_STAGE=4 } Gk312Status;
typedef struct {
 Gk304Report replay;
 uint16_t nmi_pc_witnesses,c289_pc_witnesses,store_pc_witnesses;
 uint8_t strict_cpu_origin_claims_checked;
 uint8_t observed_stock_runtime; /* ALWAYS 0 */
 uint8_t independently_generated_first_spring; /* ALWAYS 0 */
 uint8_t playable_orders_granted; /* ALWAYS 0 */
} Gk312Report;
/* Only an externally supplied CPU-PC/cycle witness per event is accepted.
 * The original ROM, event stream, and claimed register values are checked
 * by the complete inherited GK302+GK304 replay. Failures do not touch out.
 * Source/static evidence cannot authenticate a Mesen execution itself. */
Gk312Status gk312_verify_original_event_pc(const char *original_stock_rom,
 const uint8_t initial[6],const Gk302SeedEvent *events,size_t event_count,
 const Gk304Registers *registers,size_t register_count,
 const Gk312PcWitness *pcs,size_t pc_count,Gk312Report *out);
#endif
