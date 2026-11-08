#ifndef GK98_CPU_ROTATE_H
#define GK98_CPU_ROTATE_H
#include <stdint.h>
typedef struct {uint8_t value,carry;} Gk98Rotate;
/* NMOS 6502 ROR memory: old bit0 is outgoing carry; incoming carry is bit7.
 * Caller updates N/Z from value, and must preserve all other P flags. */
Gk98Rotate gk98_cpu_ror(uint8_t old_value,uint8_t carry_in);
#endif
