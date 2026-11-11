# GK1 EXEC0193 — Source implementer / hostile QA handoff (2026-11-11)

**Predecessor:** Complete inherited EXEC0192, not obsolete EXEC0076/0062. This is the SAME existing native 1280×720 C11 X11/Win32-XInput game and retains all historic work.

## Source and functionality

Implemented `gk193_auto_source_entry.c/.h` in actual existing `Gk1App` (linked to `gk1_app_state`), new `--play-original-auto <private source>` and `--smoke-original-auto <private source> <1..120 frames>` controller game entry. Content sniff reads original Mesen `.mss` header and selects correct active SaveRAM profile by exact previously established SHA-verified original 14×83 Mongol vs 27×83 World source records, or recognizes prior original Mesen fresh final `14_after_wait` JSON and delegates to the established exact WorkRAM SHA validation. It never uses filename extension, universal Gold/Rate constant, or wrong RAM bank. The only caller-visible in-game mutation is authenticated candidate-to-actual-app commit; on any refusal, entire original `Gk1App` and detected-output remain byte-identical. Original Classic/Enhanced guard is enforced. The entered campaign supports inherited Classic controller source-attested labor/Pass/Undo/save; it does NOT generate original stock first Spring, NPC orders, merchant RNG or complete campaigns.

## Executed tests

- Linux Release CMake+Ninja compile PASS; **156/156 CTest PASS**. Debug **156/156 PASS**.
- Exact private original Mesen **fresh WorkRAM final JSON**, **archived Mongol SaveRAM `.mss`** and **archived World SaveRAM `.mss`** each entered actual C11 game, correct 14/14/27 countries, original Spring gold 430/439/193, Food 208/208/332, Rate 121/101/114, 3 orders; **72/72 focused assertions PASS**. Negative controls: null args, impossible player index, source absent/truncated, wrong Enhanced profile, wrong immutable canonical CSV, post-import entire-app rollback, unchanged detected profile.
- Focused original-private ASan+UBSan test **72/72 PASS**, no sanitizer reports.
- `xvfb-run -a <native> --smoke-original-auto <private evidence> 120` for each of **3** original source profiles returned 0, 1280×720 and 120 frames. Normal 120-frame native smoke PASS.
- Linux CPack Debian installer generated. Authored NSIS packaging only; **NOT RUN** Windows native executable, installed Windows NSIS, physical Xbox or full campaign.
- Independent delta `diff -ruN execution_192 execution_193` replayed `patch -p1`, **609/609 SHA-256 file equality**. Full historical source archive retains original **43,262** predecessor ZIP entries byte-identity of payload and CRC, adding complete EXEC0193.

## Evidence boundaries

**VERIFIED_STATIC:** Source and hashes, no licensed original distributed; all original 83-byte source authority held private.
**OBSERVED_RUNTIME_STOCK:** Prior, separately preserved original Mesen fresh vs archived original snapshots; NO NEW Mesen ROM emulation performed in EXEC0193.
**TESTED_PORT_LINUX:** C11 Linux game, private source real app entry, controller-side existing behavior only, Xvfb graphics, CTest/sanitizers, Debian package creation.
**NOT RUN:** Win32+NSIS runtime, actual Xbox hardware, stock ROM first-Spring reset parity, 14/27-country completed campaigns.

## Current top priority

Reconstruct exact NES reset/setup/difficulty/ability RNG and actual 14/27-country first Spring state transforms and three source-authentic playable Classic commands from original emulator/source trace. Avoid additional arbitrary read-only source stubs or manual re-research. Next Trade presence/RNG, autonomous AI, battle ends, diplomacy/dynasty, audio, save/cold-load full campaigns, then native Windows and physical Xbox.

Publication does not constitute any other agent's read/execute acknowledgment or Director release acceptance. Keep recurring development enabled.
