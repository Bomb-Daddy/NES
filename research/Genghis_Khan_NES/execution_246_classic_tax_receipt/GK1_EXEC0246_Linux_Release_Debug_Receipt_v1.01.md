# EXEC0246 Post-Build Native Acceptance Receipt v1.01 — November 13, 2026

This is the LATER status after the initial EXEC0246 code handoff and the earlier compiler timeouts. Those timeouts were transient client execution limits; subsequent incremental compilation completed successfully. This document supersedes **only** the initial EXEC0246 handoff's unfinished full-Linux-build status; all earlier source/red-green evidence remains in force.

## Actual C11 test execution
1. Full inherited executable and every test target on Linux Release: `cmake --build /mnt/data/GK1_EXEC0246_WORK/build -j16` — **exit 0**; `gk1_native_shell` linked successfully.
2. `ctest --test-dir /mnt/data/GK1_EXEC0246_WORK/build --output-on-failure -j12` — **209/209 PASS, exit 0**, elapsed 10.64s. Includes real bounded X11 1280×720 controller UI, save/quit/load regression and EXEC0246 new negative test.
3. Full Linux Debug build `cmake --build /mnt/data/GK1_EXEC0246_WORK/build-debug -j16` — **exit 0**; `ctest --test-dir /mnt/data/GK1_EXEC0246_WORK/build-debug --output-on-failure -j12` — **209/209 PASS, exit 0**, elapsed 10.54s.
4. Direct RELEASE and DEBUG authenticated source private tests: `test_gk246_classic_tax_receipt <country.csv> <original fresh Mongol 14×83 WorkRAM> <original archived Mongol 14×83 SaveRAM> <original archived World 27×83 SaveRAM>` — **234/234 assertions per configuration**, exit 0. Legacy `test_gk188_classic_receipt`: **144/144 per build**, existing `test_gk189_classic_cold_save`: **200/200 per build**. Private fixtures are NOT in source ZIP/GitHub.
5. Independent exact patch replay using EXEC0245 source ZIP + `GK1_EXEC0246_Integration.patch`: `patch -p1` exited 0, all three modified/added source files byte-identical to tested source. ZIP CRC validation passed. Public GitHub exact readback verified for five new source/QA files.

## Evidence classification
- VERIFIED_STATIC: original auth private evidence SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`; independently verified code patch and GitHub blobs; no private ROM/data distributed.
- TESTED_PORT_LINUX: complete integrated Release and Debug native C11 builds, each 209/209 CTests; three private original stock checkpoint source/replay positive cases.
- OBSERVED_RUNTIME_STOCK_ARCHIVED and OBSERVED_RUNTIME_STOCK_FRESH: existing authentic original Mesen evidence reused, **no NEW original stock emulator run**. No original NES Tax parity established.
- NOT RUN: EXEC0246 Win32 MSVC/NSIS, physical Xbox, full original 14/27-country campaigns.
- Remaining essential G02/G11: original independent first-Spring ruler/difficulty/ability RNG and source-authentic three-order complete playable campaign are **OPEN**. Existing stock imports are checkpoint replay, not independently generated NES behavior. No claims of full release or agent acknowledgment.

## Director release policy
Retain complete inherited C11 game and all historical evidence. Do not publish private original ROM/SaveRAM/WorkRAM/graphics/audio. Do not disable hourly build task absent Director explicit stop or accepted complete release. This receipt is a code/test/publication report, not proof agents read or executed a Drive document.
