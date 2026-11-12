# GK1 EXEC0220 — Implementer / Hostile-QA Handoff
**Date:** 2026-11-12  **Predecessor:** EXEC0219 (actual complete Drive ZIP; newer than last GitHub EXEC0217).  **Status:** development candidate, NOT a complete original Genghis Khan NES clone.

## Integrated hands-on gameplay behavior
Classic PRE_SETUP labor planner allows an actual controller A one-unit transfer between five workforce categories; LB now reverts the newest accepted draft transfer. The implementation is in linked `execution_199/gk220_labor_history.c/.h`, integrated into `gk1_app_state.h/.c`, `gk1_native_shell.c` X11/Win32 gamepad actions, and existing CMake target. History is bounded to 64, strictly LIFO, per-country, keyed to source world, rejects wrong country or unrelated state changes atomically, does not silently permit overflow or alter source baseline. Existing immutable `gk219` finance/source comparison checks whole-world conservation before/after transfers and undo. Undo history is volatile; on full pre-setup cold-load it resets, while saved workforce drafts remain correctly restored. This is a PORT usability feature, not source-observed original NES Assign or Spring formula. LB is undo in the labor planner only: X still saves, Y loads, RB cycles slot and normal LB behavior elsewhere remains unchanged.

## Source custody and repeatability
Full 847-member source/QA ZIP preserves every one of 844 inherited EXEC0219 archive entries; exactly four modified paths and three new paths. GNU patch from pristine EXEC0219 source `patch -p1 -i GK1_EXEC0220_CopyrightClean_Integration.patch` returned 0 and all 7 paths SHA-256 matched. The original ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` and live Mesen state remain separately private, NEVER present in source archives or GitHub.

## Repro commands and actual results
```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build --output-on-failure -j12
# Release 186/186 PASS
cmake -S execution_199 -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j8
ctest --test-dir build-debug --output-on-failure -j12
# Debug 186/186 PASS
# Independent clean full ZIP extraction: 186/186 PASS
# ASan/UBSan target test_gk220_labor_history: 7/7 suites PASS
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-classic-labor-undo 120
# 1280x720 actual X11, input A then LB, state restored, PASS
cpack -G DEB --config build/CPackConfig.cmake
# Debian installed-layout extracted binary run under Xvfb 120 frames PASS
```

## Hostile QA requirements
1. Verify button wiring: LB only in Classic labor planner, X save preserved and existing native multislots pass, Y load and RB slot unchanged. Native controller smoke is *synthetic source-backed gamepad input*, not physical Xbox acceptance.
2. For all 14/27 country templates verify labor sum conservation, exact undo, 64-stack/full rejection, opposite-country undo rejection, no Gold/Rate/identity tamper, invalid pairs null and out-of-range rejection, no state mutation on refusal.
3. Assert read-only finance/source comparison does not mark Classic game playable or grant original 3 seasonal orders. Wrong-bank SaveRAM/WorkRAM original evidence and source-template projections must remain separate.
4. Run original-stock Mesen with **EXEC0218** inherited recorder next; this is the real missing execution gate. Trace original CPU six RNG bytes `$0084-$0089`, relevant cartridge RAM and physical mapper banks for fresh/archived separately. Run hostile/parity QA on original first Spring, ruler/difficulty, and only then connect 14/27 setup state to main game. Record `VERIFIED_STATIC`, `OBSERVED_RUNTIME_STOCK`, `TESTED_PORT_LINUX`, `NOT RUN` separately.

**Unfinished:** original authentic dynamically generated first playable Spring, ruler/ability setup and RNG chronology, original Trade presence, autonomous AI, dynasty, diplomacy, war aftermath, full campaigns, native Windows installer and physical Xbox runtime. Keep recurring job enabled unless Director stops or verified accepted complete release; publishing is not other agents' acknowledgment.
