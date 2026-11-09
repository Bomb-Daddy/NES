# GK1 EXEC0128 — integrated original NES OAM source-register research

This is the copyright-clean delta for the **complete** Genghis Khan I native C11 executable inherited from EXEC0127, not a replacement game. Do not use it without the full EXEC0127 C11 source archive, and do not merge with ROTK II.

## Source changes actually built/tested

New exact code files: `gk128_oam.c`, `gk128_oam.h` and `test_gk128_oam.c` (GitHub blob IDs verified). The full integrated EXEC0128 build additionally changes:

1. `CMakeLists.txt`: add `gk128_oam.c` to the existing `gk1_app_state` library sources. Add executable `test_gk128_oam` linked to `gk1_app_state`; register CTest `GK1_128_SourceGatedOAMRegisters_ExplicitAPUPhaseDMA`.
2. `gk83_reset_trace.c`: include `gk128_oam.h`, embed `Gk128Oam oam;` in source trace state, and in the existing source-only PPU-scheduled CPU write path handle mirrored `$2003` and `$2004` as in `INTEGRATION.md`.
3. `gk83_reset_trace.h`: add `uint32_t oam_address_writes,oam_direct_writes,oam_dma_transfers;` and `uint16_t first_unknown_oam_access;` to `Gk83Trace`.
4. Package and native shell display label `1.28.0-dev1`, preserve all existing source, profile choices, controller UI, save files and 14/27 PRE_SETUP country templates. No original licensed ROM, SRAM, images or audio included.

## Exact release evidence (Nov. 9, 2026)

Release **86/86**, Debug **86/86**, independent clean-source extraction **86/86** all PASS. Focused ASan+UBSan bounded module harness PASS; **full sanitizer suite NOT CERTIFIED**. Native 1280×720 Linux smoke 120 frames and extracted Debian executable 120 frames PASS. Windows, NSIS, physical Xbox and new original Mesen gameplay run NOT RUN.

Source archive SHA256: `c3b8db61ed56445b303366fd025c258e223791cb2fa845b11f2a81f22f34221b` (21,892,901 bytes, 358 current files; 13,417 earlier archive entries preserved byte-identically). Debian research package SHA256: `8876fc7c60f978d8ac575e03a51e352e678a3277b926749b7789dd8971c81bfa`. QA ZIP SHA256 `822e23d2bfd7da8253af8a7e1b69991d821d4bbd20ebd0cf73f0d04ba35c6a2e`. These are local artifacts, Drive binary upload **NOT VERIFIED**.

**Important:** the original stock source interpreter still fails closed on $4014 DMA, unknown $00F5 after 70 reset instructions, and source-unmodeled rendering. The 513/514-cycle DMA API requires *independent* APU alignment evidence, not CPU cycle parity. This is NOT original NES Spring/three-order gameplay.

Published current cumulative master and handoff in verified Retro Gaming/Collaboration Drive. Keep raw original ROM/evidence in private Genghis research Drive. Hourly task continues until Director stop/release acceptance.
