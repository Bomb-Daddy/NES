# EXEC0128 exact source integration into inherited full EXEC0127 game

These snippets are from the TESTED integrated C11 source; no private NES bytes are embedded.

In CMakeLists.txt add `gk128_oam.c` to `add_library(gk1_app_state STATIC ...)`. At the test registration tail:
```cmake
add_executable(test_gk128_oam test_gk128_oam.c)
target_link_libraries(test_gk128_oam PRIVATE gk1_app_state)
add_test(NAME GK1_128_SourceGatedOAMRegisters_ExplicitAPUPhaseDMA COMMAND test_gk128_oam)
```

In gk83_reset_trace.c, with existing gk127_io_transaction.h includes:
```c
#include "gk128_oam.h"
```
In existing original CPU trace state `Gk83Machine` add:
```c
Gk128Oam oam;
```
Inside the source-only `if(m->ppu_schedule_enabled&&addr>=0x2000u&&addr<0x4000u){` branch *before* existing PPUCTRL/PPUMASK handling, insert exactly:
```c
/* Actual PPU OAMADDR/OAMDATA CPU register writes, source-only.
 * Unknown OAMADDR is not silently initialized to zero at power-on.
 * Full DMA still refuses unknown initial APU phase via GK126. */
if((addr&7u)==3u){
 if(gk128_set_oam_address(&m->oam,v)!=GK128_OK)m->ppu_schedule_gate=1;
 else m->result.oam_address_writes++;
}else if((addr&7u)==4u){
 if(gk128_write_oam_data(&m->oam,v)!=GK128_OK){
  m->ppu_schedule_gate=1;
  if(!m->result.first_unknown_oam_access)m->result.first_unknown_oam_access=addr;
 }else m->result.oam_direct_writes++;
}
```
In Gk83Trace, gk83_reset_trace.h:
```c
uint32_t oam_address_writes,oam_direct_writes,oam_dma_transfers;
uint16_t first_unknown_oam_access;
```
Keep unknown $4014 DMA blocked in gk83 source interpreter until independent APU phase, DMC/refresh arbitration, source-page memory knowledge and real stock Mesen event evidence are available. Do not invent an initial OAMADDR latch or assume CPU cycle parity equals APU half-cycle.

Run:
```sh
cmake -S execution_128 -B build128 -DCMAKE_BUILD_TYPE=Release
cmake --build build128 -j8
GK1_ORIGINAL_STOCK_ROM=/private/exact_stock_original.nes ctest --test-dir build128 --output-on-failure -j8
(cd build128 && cpack -G DEB)
xvfb-run -a build128/gk1_native_shell --smoke-frames 120
```
The optional ROM path is private; validate 262160B SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3. No new authentic Mesen gameplay claimed.
