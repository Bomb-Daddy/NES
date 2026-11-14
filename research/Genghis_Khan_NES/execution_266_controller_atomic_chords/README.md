# GK1 EXEC0266 — Implementer / hostile QA handoff, 2026-11-13

**Project** Genghis Khan I English NES → native C11 Windows/Linux. Inherit complete EXEC0265 source; do not restart at EXEC0076. This correction is a production controller input change, not an NES algorithm derivation.

**Defect:** When START+BACK (menu/view) or A+START (confirm/menu) were pressed in the same frame, `gk1_pad_step` emitted two events; the native `pad_actions` loop executed both sequentially against potentially different screens. Risk of bypassing confirmation/navigation intent.

**Code:** `gk1_pad_router.c` adds two disarmed pairs with bit flags 0x20 and 0x40, neutral-rearm semantics; `gk1_pad_router.h` clarifies contract; new `test_gk266_controller_chord.c` covers simultaneous entry, partial release, full release, independent X/Y+MENU behavior, real XInput raw decoding, and disconnect/reconnect; CMake registers GK1_266. Only native source not ROM data.

**Reproduce:** `cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j8 && ctest --test-dir build -R GK1_266_Controller_Atomic_Commands --output-on-failure`. Standalone exact source test: `gcc -std=c11 -Wall -Wextra -Werror -I execution_199 execution_199/gk1_pad_router.c execution_199/gk99_xinput_bridge.c execution_199/test_gk266_controller_chord.c -o test266 && ./test266`. The test MUST fail against predecessor router.

**QA evidence:** 40 new assertions PASS; 108 old chord assertions PASS; previous source test FAIL at line17. Complete build exit0, Linux CTest 220/221 with inherited GK1_257 workflow historical mismatch. Previous CI workflows are not proof Windows compiled this patch. Win32 NSIS, physical Xbox and complete campaigns NOT RUN.

**Hostile follow-ups:** test Start+Back and A+Start concurrently on physical Xbox while switching authentic-verified Classic screens; assure 1 event max and no source data mutations; verify no Windows input driver-specific anomalies. Restore self-contained live CI workflow provenance separately; do not suppress old test or claim all tests green.

**Original-stock first Spring:** source RNG/difficulty/character setup, merchant RNG, AI, battle ending, dynasty, full campaign remain OPEN. No fiction hardcoding Gold/Rate from observed different original seed histories.
