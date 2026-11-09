# GK1 EXEC0130 — authenticated CPU-bus DMA source expansion

Complete inherited EXEC0129 native C11 game; **not** an alternate prototype.

New `gk130_dma_bus.c/h` extends the original interpreter's explicitly hypothetical OAM DMA path to all three source-data regions with independently established bytes: mirrored `$0000-$1FFF` CPU RAM, distinct `$6000-$7FFF` PRG-RAM, and exact SHA-authenticated MMC1-mapped `$8000-$FFFF` cartridge PRG ROM. `$2000-$5FFF` is rejected because it would read unmodeled PPU/APU/expansion ports. Unknown SRAM bytes, mapper registers, cartridge identity, OAMADDR and APU DMA phase fail atomically. Data is committed only after all 256 reads are valid; 513/514 CPU stall cycles remain an **explicit timing hypothesis**.

Actual default `gk83_trace_original_reset` still halts at 70 instructions/$00F5, and source-driven Mesen AllZeros still first encounters the PPU `$2002` barrier. Neither this code nor these tests generates Spring 1175 or licenses original merchant RNG. No private ROM bytes or screenshots belong in GitHub or source ZIP. Native Windows, NSIS and physical Xbox are NOT RUN. Do not promote the existing research installer to a complete Windows game.
