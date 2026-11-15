#ifndef GK290_SEED_CHRONOLOGY_H
#define GK290_SEED_CHRONOLOGY_H
#include <stddef.h>
#include <stdint.h>
/* Trace schema for source-executed CPU writes into the original NES RNG
 * seed at $0084..$0089, followed by C289 calls. Each record must be collected
 * by independent emulator tracing. A caller-supplied record is NOT in itself
 * evidence of a real Mesen execution. No RAM, screenshots, or ROM are bundled.
 */
#define GK290_MAX_EVENTS 8192u
#define GK290_WRITE 1u
#define GK290_CALL 2u
typedef enum {GK290_OK=0,GK290_ARGUMENT=1,GK290_CHRONOLOGY=2,
 GK290_SEED_UNPROVEN=3,GK290_PARITY=4,GK290_LIMIT=5} Gk290Status;
typedef struct {
 uint64_t cycle;
 uint16_t pc, address; /* WRITE: source store PC/address; CALL: C289 entry */
 uint16_t draw15; /* CALL: original A/$66/$67 15-bit draw */
 uint8_t kind, bank, bank_known, value;
 uint8_t before[6], after[6]; /* CALL: actual CPU seed at C289 entry/return */
} Gk290Event;
typedef struct {
 uint8_t first_explicit_seed[6],final_seed[6];
 uint32_t observed_seed_writes,validated_c289_calls,interleaved_seed_writes;
 uint64_t first_draw_cycle,last_draw_cycle;
 uint8_t six_source_writes_before_first_draw,source_math_parity;
 uint8_t real_mesen_proven,stock_first_spring_generated,orders_granted;
} Gk290Report;
/* Enforces a complete source-written six-byte seed BEFORE first C289 and
 * exactly accounts for every subsequent C289 seed transition. Unknown seed
 * discontinuities are errors, not merely a counter. Fail-atomic output. */
Gk290Status gk290_verify_written_seed_timeline(const Gk290Event *events,
 size_t count,Gk290Report *out);
#endif
