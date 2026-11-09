# EXEC0131 — original stock boot provenance, not a playable first Spring

Verified November 9, 2026. This is the EXISTING complete native C11 Genghis Khan I game, not ROTK II, the custom KOEI-inspired game, or a standalone demonstration.

## Source ancestry
* Begin from the complete verified EXEC0129 historical source ZIP, SHA256 01b739e205d349d60ac4879354b029609e71ff89fa53210b6abbe32ae0963275.
* Recover and integrate the public EXEC0130 gk130_dma_bus.c/h/test source from execution_130_delta, preserving all preexisting source and the game target. The private full EXEC0130 ZIP was unavailable. Consequently EXEC0131 is a reconstructed lineage from the intact EXEC0129 full source plus the EXEC0130 public delta, **not byte-identical complete EXEC0130 ancestry**.
* In the same real native C11 game target add gk131_boot_gate.c/h, test_gk131_boot_gate.c. Apply the two source-provenance patches from this directory to gk83_reset_trace.c/h (patches are against full EXEC0129), keeping all inherited original CPU logic and source gates. Link gk130_dma_bus.c and gk131_boot_gate.c into gk1_app_state and add both new tests to CMake. Update source/install version to 1.31.0-dev1.
* Actual complete integrated source and its historical provenance archive: GK1_EXEC0131_Complete_Historical_C11_Source_v1.00.zip, SHA256 9552ef0945136bc75a99c36015d1b0133faabf64367b2c75ca5608ae6127dee7. This archive preserves all 14,131 predecessor ZIP members by content digest. Public GitHub contains only this copyright-clean delta.

## Actual implementation
The authenticated stock USA NES interpreter now records exactly where a first UNKNOWN CPU RAM value is read, the opcode, source MMC1 bank, count of prior writes and known reads at $00F5. Source trace using original private ROM (262160 bytes, SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3) produces 70 instructions, PC $8060, LDA $00F5 (opcode $AD) in source PRG bank14, with **ZERO known prior writes** to $00F5. A distinct explicit archived MesenCE RamPowerOnState=1 AllZeros source premise produces 142 instructions and then halts at unmodeled PPUSTATUS $2002, PC $C5BE. Neither is independently playable first Spring; archived emulator settings do not prove universal stock power-on RAM or RNG.

## Reproduction
Use the full integrated source ZIP:
cmake -S execution_131 -B build -DCMAKE_BUILD_TYPE=Release -G Ninja
cmake --build build -j8
GK1_ORIGINAL_STOCK_ROM=/path/to/private-stock.nes ctest --test-dir build -j8 --output-on-failure
The exact private ROM must stay local and SHA-checked; do not distribute it or raw RAM, screenshots, licensed art/audio. Linux Debian package: cpack --config build/CPackConfig.cmake -G DEB. X11/Xvfb: xvfb-run -a -s '-screen 0 1280x720x24' build/gk1_native_shell --smoke-frames 120.

## QA and gates
**TESTED_PORT_LINUX:** 89/89 Release, 89/89 Debug, 89/89 independently extracted historical source; focused ASan+UBSan 2/2; full sanitizer ran 88/89 before timeout, NOT CERTIFIED. Both compiled and Debian-extracted Linux GUI 120 frames PASS. **VERIFIED_STATIC:** private original ROM SHA and exact source gates, negative controls/rollback/determinism. **OBSERVED_RUNTIME_STOCK:** only PREVIOUS original Mesen captures: fresh Mongol1175 G430 F208 T61 Rate121 3 orders; archived Mongol1175 G439 F208 T61 Rate101 3 orders; archived World1206 G193 F332 T116 Rate114 3 orders; these different histories must not be hardcoded. **NOT RUN:** new Mesen gameplay, native Win32, NSIS installation, physical Xbox controller and complete campaigns.

**Open:** native authentic original Spring setup/difficulty/character RNG, true 14/27 first-Spring country evolution/3 orders, live Trade scheduler, AI, diplomacy, dynasty, battle aftermath, audiovisual fidelity, complete campaigns. Keep recurring implementation enabled. Publication is not implementer read/execute acknowledgment.