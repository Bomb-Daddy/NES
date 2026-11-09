#ifndef GK101_MMC1_CYCLES_H
#define GK101_MMC1_CYCLES_H
#include <stdint.h>
/* Ricoh 2A03/MMC1 serial-port cycle filter. The MMC1 normally rejects
 * a second bit-0 write in the immediately following CPU cycle. Reset
 * writes with bit7 set are never suppressed. This is the CPU-bus rule,
 * not a PPU-vblank or stock-first-Spring source observation. */
typedef struct {
 uint64_t previous_cycle;
 uint32_t accepted, ignored, resets;
 uint8_t has_previous;
} Gk101Mmc1Filter;
typedef enum {GK101_ACCEPT=1,GK101_IGNORED=2,GK101_RESET=3,
              GK101_NONMONOTONIC=4} Gk101Decision;
Gk101Decision gk101_mmc1_write(Gk101Mmc1Filter *filter,
                                uint64_t cpu_cycle,uint8_t value);
#endif
