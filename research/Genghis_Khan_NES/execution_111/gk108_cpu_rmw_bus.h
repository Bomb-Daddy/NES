#ifndef GK108_CPU_RMW_BUS_H
#define GK108_CPU_RMW_BUS_H
#include <stdint.h>
/* The 2A03 inherits NMOS 6502 read-modify-write double-write behavior:
 * on the last two cycles it writes the old value and then the new value.
 * Hardware callback sees two consecutive CPU bus write cycles, including
 * mapper/PPU side effects; never reduce these to one final RAM write.
 * A source-tracer primitive, NOT a stock Genghis RNG/season claim. */
typedef void (*Gk108WriteBus)(void *ctx,uint16_t address,uint8_t data,uint64_t cycle);
int gk108_cpu_rmw_pair(uint64_t final_cpu_cycle,uint16_t address,
                       uint8_t old_value,uint8_t new_value,
                       Gk108WriteBus writer,void *ctx);
#endif
