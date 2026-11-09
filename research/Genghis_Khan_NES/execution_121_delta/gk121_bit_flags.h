#ifndef GK121_BIT_FLAGS_H
#define GK121_BIT_FLAGS_H
#include <stdint.h>
/* NMOS 6502 / NES 2A03 BIT: Z = !(A & M), V=M6, N=M7.
 * Carry, interrupt, decimal, break and other status bits are unchanged.
 * Pure source-correct arithmetic; NOT a claim of Spring/RNG parity. */
uint8_t gk121_bit_status(uint8_t accumulator,uint8_t memory,uint8_t status);
#endif
