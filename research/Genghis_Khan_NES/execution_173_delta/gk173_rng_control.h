#ifndef GK173_RNG_CONTROL_H
#define GK173_RNG_CONTROL_H
#include <stdint.h>
/* Bounded, actually EXECUTED original-ROM 6502 control transfers.
 * Not an NES/Mesen frame trace. No candidate opcode scans are used here.
 * A reported transfer has completed in the fail-closed source CPU interpreter.
 * The mapper register may not identify a stable physical PRG bank; 0xff
 * explicitly means unknown. Never infer an RNG draw from the transfer alone. */
typedef struct {
 uint32_t dispatch_jsr_ef85, direct_jsr_c289;
 uint32_t fixed_trampoline_jmp_c289, other_jmp_c289;
 uint32_t indirect_jmp_c289;
 uint32_t total;
 uint16_t first_pc,first_target,last_pc,last_target;
 uint8_t first_opcode,first_prg_register,last_opcode,last_prg_register;
 uint64_t first_completed_cycles,last_completed_cycles;
} Gk173RngControl;
typedef enum {GK173_INVALID=0,GK173_IGNORED=1,GK173_NOTED=2} Gk173Status;
/* No memory allocation, no mutable game state. Output unchanged on refusal. */
Gk173Status gk173_note_completed(Gk173RngControl *out,
 uint8_t opcode,uint16_t source_pc,uint16_t destination_pc,
 uint8_t mapper_prg_register,uint64_t completed_cycles);
#endif
