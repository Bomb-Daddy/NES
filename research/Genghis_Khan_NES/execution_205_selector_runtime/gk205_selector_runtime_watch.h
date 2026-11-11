#ifndef GK205_SELECTOR_RUNTIME_WATCH_H
#define GK205_SELECTOR_RUNTIME_WATCH_H
#include <stdint.h>
/* Only source-CPU stores completed in the bounded C11 6502 executor.
 * A BRK is a GATE, not an executed interrupt, and the code never infers
 * a first Spring, original Mesen time, or mapper state when unknown. */
#define GK205_STORED_EVENTS 16u
typedef enum {
 GK205_BAD_INPUT=0,GK205_OUTSIDE=1,GK205_RECORDED=2,GK205_BRK_ATTEMPT=3
} Gk205Result;
typedef struct {
 uint16_t pc, address;
 uint8_t selector_value, prg_bank, bank_known;
 uint64_t bus_cpu_cycle;
} Gk205Store;
typedef struct {
 uint32_t completed_selector_writes;
 uint32_t dropped_event_details;
 uint32_t brk_gates; /* bounded emulator stopped BEFORE executing the BRK */
 uint16_t brk_pc;
 uint8_t brk_selector_known,brk_selector_value;
 uint8_t brk_bank_known,brk_bank;
 Gk205Store first[GK205_STORED_EVENTS];
 Gk205Store last;
} Gk205SelectorWatch;
/* Failure/outside/overflow is atomic. NES internal RAM mirrors included.
 * CPU cycles are a bounded interpreter's clocks, NOT original Mesen clocks. */
Gk205Result gk205_note_completed_selector_store(Gk205SelectorWatch *w,
 uint16_t source_pc,uint16_t cpu_addr,uint8_t value,
 uint8_t prg_known,uint8_t prg_bank,uint64_t write_cycle);
Gk205Result gk205_note_brk_gate(Gk205SelectorWatch *w,uint16_t source_pc,
 uint8_t selector_known,uint8_t selector,uint8_t prg_known,uint8_t prg_bank);
#endif
