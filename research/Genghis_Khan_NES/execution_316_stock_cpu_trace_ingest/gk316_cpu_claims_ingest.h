#ifndef GK316_CPU_CLAIMS_INGEST_H
#define GK316_CPU_CLAIMS_INGEST_H
#include <stddef.h>
#include <stdint.h>
/* EXEC0316: bounded, fail-closed ingestion of externally supplied CPU event
 * CLAIMS, for later exact-ROM gk302/gk304/gk312 replay.  This is NOT emulator
 * provenance, stat initialization, a playable Spring, or an order grant.
 * Do NOT publish the private logs or original ROM. */
#define GK316_EVENT_LIMIT 4096u
#define GK316_TRACE_HEADER "GK1_STOCK_CPU_CLAIMS_V1"
typedef enum { GK316_OK=0, GK316_ARGUMENT=1, GK316_IO=2,
 GK316_SYNTAX=3, GK316_ORDER=4, GK316_LIMIT=5, GK316_STAGE=6 } Gk316Status;
typedef enum { GK316_NMI=1, GK316_STORE=2, GK316_C289=3 } Gk316Kind;
typedef struct {
 uint64_t cycle;
 uint16_t pc;
 uint16_t draw15;
 uint8_t kind,bank,gate,target,before,after,a,x,y;
 uint8_t rng_before[6],rng_after[6];
} Gk316Event;
typedef struct {
 Gk316Event events[GK316_EVENT_LIMIT];
 uint8_t initial_seed[6];
 uint16_t event_count,nmi_count,store_count,c289_count;
 uint8_t world; /* 0 fresh Mongol or 1 archived World, only claimed */
 uint8_t source_claim_only; /* always 1 */
 uint8_t observed_stock_emulator; /* always 0 */
 uint8_t first_spring_initialized; /* always 0 */
 uint8_t player_orders_granted; /* always 0 */
} Gk316Trace;
/* Header: GK1_STOCK_CPU_CLAIMS_V1,<64 lowercase hex SHA>,<M|W>,<12 hex seed>.
 * N,<decimal cycle>,<two hex gate 8A>
 * S,<decimal cycle>,<four hex CPU PC>,<decimal bank>,<two hex target>,
 *   <two hex before>,<two hex after>,<two hex A>,<two hex X>,<two hex Y>
 * C,<decimal cycle>,c289,15,<12 hex before>,<12 hex after>,<decimal draw15>
 * Exact canonical LF, no whitespace or surplus fields. All inputs are claims;
 * 6502 opcode/value/ROM verification belongs to existing GK302/304/312. */
Gk316Status gk316_load_cpu_claims(const char *path,Gk316Trace *out);
#endif
