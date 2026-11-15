# GK1 EXEC0294 — exact-ROM STORE-source attestation integrated into complete native C11 app
Date: November 14, 2026 (America/Detroit). Predecessor: EXEC0293. This source changes the existing `execution_199` native game, not a new prototype. Preserve earlier source and game state.

## Authority / restrictions
- Director's original-stock USA NES: 262160 bytes SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, authenticated PRIVATE. Original ROM, source RAM, screenshots, `.mss` and licensed images never published in this archive or public GitHub.
- Previous EXEC0293 power-on Lua tooling is preserved without alteration. This run did **NOT** perform a real Mesen V3 power-on capture; unknown startup RNG timing and actual seed-writer CPU sequence remain open.
- Both campaigns and their inherited code retained. The original source-ROM data are optional at test runtime, never bundled.

## Actual C11 changes
1. New `execution_199/gk294_seed_writer_attest.c/.h` validates *one* exact authenticated original-stock file (length and SHA-256 in a single in-memory read) and rejects unauthenticated ROMs. Maps claimed CPU PC + MMC1 16 KiB PRG bank to PRG offset without selecting another bank for fixed `$C000-$FFFF`; accepts only direct STA/STX/STY zero-page and absolute stores targeting `$84..$89`; checks chronological CPU cycles and all six target seed bytes; refuses malformed/out-of-bank/ambiguous source. Atomic output on any failure. Critically does not report that the claimed source PC was actually executed, nor does it derive the value stored.
2. `execution_199/gk1_app_state.c/.h` includes `gk1_app_attest_seed_writer_source`, only in pristine unaltered Classic PRE_SETUP with the same 14/27 country, difficulty, baseline and archived-checkpoint guards as prior C289 source wrapper. It never changes world, phase, orders, seed, or screen. Enhanced cannot use the stock-only attester.
3. Existing `execution_199/CMakeLists.txt` compiles the code into actual `gk1_app_state` and registers a new standalone live-linked native regression, `GK1_294_OriginalRomSourceSeedWriterAndAtomicRefusals`. Test executable accepts exact private original ROM optionally via argv[2] for a separately executed positive check; no private ROM is required to compile or run public CTest.
4. `execution_199/test_gk294_seed_writer_attest.c` hostile negative tests wrong memory target, PRG bank, CPU PC, timeline, source opcode/operand, cross-bank overrun, missing six-seed coverage, ROM corruption (same length), truncation, append; app-stage mutation refusal, replay equality, and no app changes.

## Verifiable reproduction (Linux)
```
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target test_gk294_seed_writer_attest -j 6
ctest --test-dir build -R GK1_294_ --output-on-failure
./build/test_gk294_seed_writer_attest execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /PRIVATE/ORIGINAL.nes
```
The last private argument is optional and requires authorized ROM. No ROM bytes appear in GitHub or public ZIP. Exact static PRG patterns used as *synthetic* claimed provenance input are six candidate snippets at bank9 `$9DFD` target `$84`, bank8 `$8919` target `$85`, bank5 `$8676` target `$86`, bank1 `$8896` target `$87`, bank3 `$882F` target `$88`, bank0 `$855B` target `$89`. **These are not observed CPU execution locations and may be unreachable or data.** The attester only says these exact bytes at these alleged locations match stores; a future real source trace remains mandatory.

## Results and limitations
- VERIFIED_STATIC: exact private ROM hash independently checked; positive candidate-source 109 C11 assertions pass, including two campaigns/five difficulty levels and hostile mutations. No original ROM raw bytes redistributed.
- TESTED_PORT_LINUX: native C11 Debug app fully compiled. One new registered CTest passes. Selected regression 232/232 passed, selected by explicit exclusion of two inherited tests; **do not call full 234/234 green**. An initial unfiltered suite attempted 234, `GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass` failed against inherited stale Windows workflow, and `GK1_226_FourSeparateX11_ControllerTitleYSafeUpdate` exceeded run time. Follow-up run excluded exactly those two and passed 232/232. These are not new code regressions proven fixed, remain unaccepted.
- OBSERVED_RUNTIME_STOCK_FRESH: earlier Nov 7 fresh actual Mesen evidence remains valid; no new emulator run. OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: earlier 14+27 country states retained; no new emulator run.
- TESTED_PORT_WIN32: **NOT RUN on EXEC0294**; previous historical Windows CI successes remain distinct. Physical Xbox controller: NOT RUN. Full 14/27 campaign completion: NOT RUN.

## Forward priority
The next work MUST be an actual original-stock Mesen power-on trace with CPU STORE-writer PC/bank provenance and successful original source-opcode cross-check, then an authentic generalized first Spring initialization integrated into existing C11 app. Different fresh/archived Rate and Gold samples MUST NOT be hardcoded. Proceed to real player orders, merchant presence, autonomous AI, victory/aftermath, diplomacy, dynasty, save/cold-load, Windows installer and physical Xbox. Publication does not mean any other implementer has read or executed it. Do not disable recurring execution or claim finished release.
