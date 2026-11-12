#ifndef GK207_SEED_WRITE_WATCH_H
#define GK207_SEED_WRITE_WATCH_H
#include <stdint.h>
/* Execution of actual decoded 6502 RAM stores by the guarded interpreter;
 * not a static ROM-pattern search and NOT live Mesen gameplay. Only writes
 * witnessed after reset count. Unknown power-on bytes never become a seed. */
#define GK207_MAX_STORED_EVENTS 32u
#define GK207_FULL_SIX_WRITES UINT8_C(0x3f)
typedef enum {GK207_INVALID=0,GK207_OUTSIDE=1,GK207_RECORDED=2} Gk207Status;
typedef struct {
 uint16_t pc, cpu_address;
 uint8_t byte_index, value, bank_known, bank;
 uint64_t cpu_cycle;
} Gk207SeedEvent;
typedef struct {
 uint32_t completed_writes, dropped_details;
 uint8_t written_mask; /* 1 bit per $0084..$0089, NEVER inferred from power-on */
 uint8_t latest_written[6]; /* only valid where corresponding mask bit set */
 Gk207SeedEvent first[GK207_MAX_STORED_EVENTS],last;
} Gk207SeedWatch;
/* Reject bad arguments/overflow atomically. All internal RAM mirrors accepted.
 * RAM stores at $0084..$0089 only; PPU, mapper, PRG-RAM stores never alias.
 * Caller provides CPU-executed instruction PC and final STORE bus cycle. */
Gk207Status gk207_note_completed_seed_store(Gk207SeedWatch *watch,
 uint16_t source_pc,uint16_t cpu_address,uint8_t value,
 uint8_t bank_known,uint8_t bank,uint64_t final_cpu_cycle);
int gk207_source_written_complete(const Gk207SeedWatch *watch);
#endif
