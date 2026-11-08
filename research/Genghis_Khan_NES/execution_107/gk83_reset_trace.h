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
 uint64_t completed_cpu_cycles; /* 2A03 cycles for FINISHED instructions only, not PPU scheduling */
 uint32_t ppuctrl_write_attempts,ppuctrl_nmi_enable_attempts;
 uint16_t last_ppuctrl_source_pc;
 uint8_t last_ppuctrl_value,ppuctrl_nmi_requested_at_injection;
 uint64_t nmi_requested_after_completed_cycles;
 uint16_t stop_pc, first_hardware_read, first_unknown_ram_read;
 uint8_t stop_opcode;
 uint8_t a,x,y,sp,flags;
 uint8_t control,prg,chr0,chr1,control_known,prg_known;
 uint32_t mapper_serial_writes,mapper_register_commits;
 uint32_t ram_writes,peripheral_writes,jsr_count,rom_bank_switches;
 uint16_t first_jsr,first_banked_pc;
 uint32_t hypothetical_ppustatus_reads; /* 0 unless EXEC0092 research-only injection */
 uint16_t last_hardware_read; /* latest source-read address on bounded trace */
 uint32_t hypothetical_controller_reads, hypothetical_controller2_reads, hypothetical_controller_writes;
 uint8_t hypothetical_controller_buttons;
 uint16_t hypothetical_nmi_vector;
 uint8_t hypothetical_nmi_injections; /* 0 normally; one-shot research only */
 uint32_t mapper_consecutive_ignored,mapper_bit7_resets,mapper_rmw_double_writes;
 /* EXEC0102 synthetic controller frames: bounded original PRG execution only.
  * Not actual PPU frame times or source-observed pad event chronology. */
 uint32_t scripted_controller_frames,scripted_last_frame_instruction;
 uint8_t scripted_last_buttons;
 /* EXEC0103 phase-parametrized source research; never original runtime parity. */
 uint64_t ppu_scheduled_frames,ppu_scheduled_vblank,ppu_scheduled_nmi_edges;
 uint32_t ppu_scheduled_nmi_injections,ppu_scheduled_status_reads;
 uint8_t ppu_scheduled_ppu_phase,ppu_scheduled_warmup_ignored;
 uint8_t ppu_scheduled_render_gate,ppu_scheduled_race_gate;
 uint32_t ppu_scheduled_frame_origin_dots; /* EXEC0104: hypothetical reset origin */
 /* EXEC0106: explicitly bounded PPU I/O latch path; never stock Mesen parity. */
 uint32_t ppu_bus_latch_writes,ppu_bus_latch_reads;
 uint8_t ppu_bus_latch_known,ppu_bus_latch_gate;
 /* EXEC0107: source-bounded open-bus write-only PPU port reads. */
 uint32_t ppu_write_only_bus_reads,ppu_write_only_age_gates;
 Gk83Stop stop;
} Gk83Trace;
/* On source/hash/argument rejection, output is byte-for-byte unchanged.
 * Successful bounded tracing returns one of the explicit gate states. */
Gk83Stop gk83_trace_original_reset(const char *private_filename,
                                   uint32_t max_instructions,Gk83Trace *output);
/* 128 possible lower-seven-bit constants in the hypothetical PPUSTATUS model.
 * Caller-supplied source only; not original runtime parity. */
Gk83Stop gk105_trace_original_low7(const char *rom,uint32_t origin,
 uint8_t lower7,uint32_t max_steps,Gk83Trace *out);
/* Unrendered PPU-status lower five bits derived from source-observed CPU PPU
 * writes. Unknown power-on latch or sprite flags fail closed. Non-Mesen. */
Gk83Stop gk106_trace_original_ppu_latch(const char *private_rom,
 uint32_t frame_origin_dots,uint32_t max_steps,Gk83Trace *out);
/* Explicit lower5 source-known + power-on sprite flags 0..3 hypotheses.
 * These flags are not claimed to be actual original emulator state. */
Gk83Stop gk106_trace_original_ppu_latch_sprite_hypothesis(const char *private_rom,
 uint32_t frame_origin_dots,uint8_t sprite_choice,
 uint32_t max_steps,Gk83Trace *out);
#endif
