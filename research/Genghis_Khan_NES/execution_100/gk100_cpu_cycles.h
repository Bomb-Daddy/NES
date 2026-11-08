#ifndef GK100_CPU_CYCLES_H
#define GK100_CPU_CYCLES_H
#include <stdint.h>
/* Standard NTSC NES Ricoh 2A03 instruction completion cycles. This module
 * does NOT schedule PPU frames, NMI edges or DMA. Unknown opcodes return 0.
 * `indexed_page_crossed` is an extra cycle only for eligible READ modes.
 * Branches use `taken` and a cross relative to the post-operand PC. */
uint8_t gk100_cpu_cycles(uint8_t opcode, uint8_t taken, uint8_t indexed_page_crossed);
uint8_t gk100_page_crossed(uint16_t base, uint16_t effective);
#endif
