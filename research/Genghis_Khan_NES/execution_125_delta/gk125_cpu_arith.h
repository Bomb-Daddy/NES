#ifndef GK125_CPU_ARITH_H
#define GK125_CPU_ARITH_H
#include <stdint.h>
typedef enum { GK125_ADC=0, GK125_SBC=1 } Gk125Kind;
typedef enum { GK125_IMM, GK125_ZP, GK125_ZPX, GK125_ABS,
 GK125_ABSX, GK125_ABSY, GK125_INDX, GK125_INDY } Gk125Mode;
typedef struct { Gk125Kind kind; Gk125Mode mode; } Gk125Arithmetic;
/* NES 2A03 uses binary arithmetic even when decimal flag D is set.
 * This implementation never reads untrusted source ROM/RAM or emulates RNG.
 * Invalid output pointer leaves all caller state unchanged. */
int gk125_decode(uint8_t opcode,Gk125Arithmetic *out);
uint8_t gk125_cycles(uint8_t opcode,uint8_t page_crossed);
int gk125_eval(Gk125Kind kind,uint8_t a,uint8_t operand,uint8_t status,
               uint8_t *result_a,uint8_t *result_status);
#endif
