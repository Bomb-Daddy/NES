#ifndef GK109_RMW_FAMILIES_H
#define GK109_RMW_FAMILIES_H
#include <stdint.h>
/* Bounded ORIGINAL 2A03/6502 source-instruction semantics. Not game RNG or
 * authentic Mesen evidence. Source-visible read-modify-write operation family;
 * write old, then new on consecutive cycles via GK108 bus hook. */
typedef enum { GK109_NONE=0, GK109_ASL, GK109_LSR, GK109_ROL,
               GK109_ROR, GK109_DEC, GK109_INC } Gk109Kind;
typedef enum { GK109_ZP=0, GK109_ZPX=1, GK109_ABS=2,
               GK109_ABSX=3 } Gk109Mode;
typedef struct { Gk109Kind kind; Gk109Mode mode; uint8_t cycles; } Gk109Spec;
typedef struct { uint8_t value,carry,n,z; } Gk109Result;
/* The 24 documented memory-RMW opcodes only. Bad opcode/NULL preserves out. */
int gk109_rmw_decode(uint8_t opcode,Gk109Spec *out);
/* Bit-accurate binary 2A03 update; accepts carry_in 0/1 only. Atomic fail. */
int gk109_rmw_compute(Gk109Kind kind,uint8_t old,uint8_t carry_in,
                      Gk109Result *out);
#endif
