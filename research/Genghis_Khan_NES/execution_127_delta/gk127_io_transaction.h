#ifndef GK127_IO_TRANSACTION_H
#define GK127_IO_TRANSACTION_H
#include <stdint.h>
#include "gk126_io_barrier.h"
/* Source-side preflight of both writes from an NMOS read-modify-write.
 * A rejection means NEITHER write is executed. This is a fail-closed
 * boundary, not a substitute for NES peripheral or PPU timing. */
typedef struct {
 Gk126IoBarrier barrier;
 uint8_t rejected_value;
 uint8_t rejected_write_index; /* 0 old, 1 new; zero if accepted */
} Gk127RmwCheck;
int gk127_preflight_rmw(uint16_t address,uint8_t old_value,
                        uint8_t new_value,int ppu_timing_enabled,
                        Gk127RmwCheck *out);
#endif
