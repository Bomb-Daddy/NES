#ifndef GK83_RESET_TRACE_H
#define GK83_RESET_TRACE_H
#include <stdint.h>
/* Exact, private STOCK ROM 6502 reset-source trace. Only executes instructions
 * while every RAM read is initialized by the trace and every PRG-bank mapping
 * is known. Stops before a peripheral read, unsupported opcode, or bound.
 * No PPU, APU, controller, vblank or original game is simulated. */
typedef enum {
 GK83_INVALID=0, GK83_SOURCE_REFUSED=1, GK83_HARDWARE_GATE=2,
 GK83_UNKNOWN_RAM_GATE=3, GK83_OPCODE_GATE=4, GK83_BANK_GATE=5,
 GK83_BUDGET_GATE=6, GK83_BRK_GATE=7
} Gk83Stop;
typedef struct {
 uint32_t instructions;
 uint16_t stop_pc, first_hardware_read, first_unknown_ram_read;
 uint8_t stop_opcode;
 uint8_t a,x,y,sp,flags;
 uint8_t control,prg,chr0,chr1,control_known,prg_known;
 uint32_t mapper_serial_writes,mapper_register_commits;
 uint32_t ram_writes,peripheral_writes,jsr_count,rom_bank_switches;
 uint16_t first_jsr,first_banked_pc;
 uint32_t hypothetical_ppustatus_reads; /* 0 unless EXEC0092 research-only injection */
 Gk83Stop stop;
} Gk83Trace;
/* On source/hash/argument rejection, output is byte-for-byte unchanged.
 * Successful bounded tracing returns one of the explicit gate states. */
Gk83Stop gk83_trace_original_reset(const char *private_filename,
                                   uint32_t max_instructions,Gk83Trace *output);
#endif
