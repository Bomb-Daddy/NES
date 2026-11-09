# EXEC0130 — integrate into the COMPLETE EXEC0129 native C11 game

**Parent:** complete EXEC0129 native source ZIP SHA-256 `01b739e205d349d60ac4879354b029609e71ff89fa53210b6abbe32ae0963275`. Preserve every existing source, country, save, controller and profile. This public directory contains only copyright-clean **delta** source and tests, not the complete native game or any ROM.

1. Copy `gk130_dma_bus.c`, `gk130_dma_bus.h` and `test_gk130_dma_bus.c` into the inherited full `execution_129` game tree, then version its new working copy `execution_130`.
2. In `gk83_reset_trace.c`, add `#include "gk130_dma_bus.h"`. Replace the existing hypothetical `$4014` WorkRAM-only `gk129_dma_from_cpu_ram` call with:
```c
Gk130DmaResult result=gk130_dma_cpu_bus(&m->oam,m->ram,m->known,
                &m->prgram,m->rom+PRG_START,GK130_PRG_BYTES,1u,
                m->control,m->prg,m->control_known,m->prg_known,
                v,1u,m->source_dma_apu_phase,&stall);
m->result.source_dma_last_page=v;
m->result.source_dma_last_result=(uint8_t)result;
if(result==GK130_DMA_OK){
    /* KEEP every inherited existing successful-DMA side effect intact */
```
The rejection branch must remain a source gate, **not** mark unknown peripherals as verified.
3. In `Gk83Trace` (`gk83_reset_trace.h`), add `uint8_t source_dma_last_page,source_dma_last_result;`.
4. In `CMakeLists.txt`, append `gk130_dma_bus.c` to `gk1_app_state`. Add executable `test_gk130_dma_bus`, link it to `gk1_app_state`, and register one new CTest. Carry forward all 87 previous tests and update version/NSIS/Linux descriptors to `1.30.0-dev1`.
5. Exact complete integrated and versioned 369-file source, 14,131 intact historic ZIP entries: locally verified `GK1_EXEC0130_Complete_Historical_C11_Source_v1.00.zip` SHA-256 `8a0017d229cf4d5a325a6681269508405ead57e44000c148598adcd4ec723cb6`. This complete ZIP **is not present on public GitHub**.
6. Regression: native Linux Release/Debug/independent archive extraction **88/88** each. Focused ASan/UBSan DMA PASS. Full sanitizer not certified. Two Linux graphical 120-frame tests PASS. Physical Xbox, Win32, NSIS, new Mesen gameplay and original first Spring **NOT RUN**.

**Hard stop:** this path models *source-checked* bus reads, but hypothetical APU alignment is not original hardware evidence. Default original-ROM reset still stops at unknown WorkRAM `$00F5` after 70 instructions; original first playable Spring and 3 orders are **not** implemented. Never publish private ROM bytes, raw SRAM or licensed assets.
