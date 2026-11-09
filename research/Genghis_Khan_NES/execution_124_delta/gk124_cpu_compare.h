#ifndef GK124_CPU_COMPARE_H
#define GK124_CPU_COMPARE_H
#include <stdint.h>
/* All 14 official NMOS 2A03 CMP/CPX/CPY opcode forms; no unofficial
 * opcodes, no guessed cartridge RAM or PPU values. Binary comparison is
 * unaffected by the 2A03's disabled decimal arithmetic. */
typedef enum { GK124_CMP_A=0, GK124_CPX_X=1, GK124_CPY_Y=2 } Gk124CompareReg;
typedef enum { GK124_IMM=0, GK124_ZP, GK124_ZPX, GK124_ABS,
 GK124_ABSX, GK124_ABSY, GK124_INDX, GK124_INDY } Gk124CompareMode;
typedef struct { Gk124CompareReg reg; Gk124CompareMode mode; } Gk124Compare;
int gk124_compare_decode(uint8_t op, Gk124Compare *out);
uint8_t gk124_compare_flags(uint8_t old_status,uint8_t register_value,uint8_t operand);
uint8_t gk124_compare_cycles(uint8_t op,uint8_t page_crossed);
#endif
