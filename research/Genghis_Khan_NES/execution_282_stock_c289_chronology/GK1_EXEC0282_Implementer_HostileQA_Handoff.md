# GK1 EXEC0282 — Current Native C11 Original C289 RNG Trace Validation Handoff

Date: 2026-11-14. Inherit full exact EXEC0280 ZIP, 1,159 files and EXEC0281 Win32 success; do not regress. This is the SAME native game (`execution_199/gk1_app_state.c` and `gk1_app_state` CMake static library), not a new prototype.

## New actual source

* `gk282_stock_rng_trace.c/h` implements a fail-closed source step auditor for structured alleged original NES C289 RNG telemetry; it checks the ORIGINAL USA ROM via the existing SHA/mapper validator, extracts source fixed-bank coefficients, crosschecks six-byte before/after seeds and 15-bit draws through authentic source arithmetic, detects discontinuous seeds and pairs candidate difficulty-Rate values with logged later rate writes.
* The `gk1_app_original_c289_trace()` API is linked into the ACTUAL native app state and gated to Classic, a loaded original 14/27 country world and first-Spring pre-setup stage; the method is read-only and NEVER unlocks Spring/orders.
* `test_gk282_stock_rng_trace.c` executes source-based synthetic traces with the private authenticated ROM (21 assertions), deliberate corrupted seed, forged extra record, malformed return, wrong original ROM and failure-atomic output; public CI without ROM positively tests refusal (2 assertions), it does not pretend ROM is unavailable or fake gameplay.
* `GK1_EXEC0282_Private_Mesen_C289_Trace.lua` is an **UNEXECUTED** proposed instrumentation source script, to be vetted against matching emulator and its documented API before any runtime execution. It produces PRIVATE gameplay evidence, not to be committed with captures or uploaded publicly. Its demonstrated usability is NOT CLAIMED.

## Exact local verification actually executed

* Private original ROM 262160 bytes SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, private historical Mesen evidence 34 members SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a` physically checked. ROM/evidence excluded from output archive and GitHub.
* `cmake --build build --target gk1_native_shell test_gk282_stock_rng_trace -j 6` returned exit0 with full native Linux app linked.
* `build/test_gk282_stock_rng_trace /private/authorized_original.nes /private/synthetic-c289-trace.tmp` returned exit0, **21 assertions PASS**; synthetic input is NOT original Mesen observation.
* `ctest --test-dir build -R 'gk280_original_rate_bridge|gk282_original_c289_trace|GK1_278_OriginalSpring24DirectFieldsAtomicAndNoFakeParity' --output-on-failure --timeout 25 -j 3` returned exit0 **3/3 PASS**.
* A separate attempted full eligible 226-test run was interrupted by a runtime timeout at `GK1_208_ArchiveSourceSeedIdentityAndAtomicFailureGates` after 160 completed; **no 226/226 PASS claimed**. Previous EXEC0281 225/225 eligibility and Win32 212/212 are historical predecessor results, NOT new acceptance.

## Evidence distinctions / next implementation

VERIFIED_STATIC: source diff/inheritance/hash and private ROM SHA readback. TESTED_PORT_LINUX: compiled full native app and 3 targeted CTests; full suite **not completed**. OBSERVED_RUNTIME_STOCK_FRESH_RESET and OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: historical source evidence inherited, **no new Mesen observed**. NOT RUN: EXEC0282 Windows, original emulator Lua recording, physical Xbox, full campaigns. No player-ready original stock first Spring produced; sources lack actual seed chronology from fresh setup. Do not assert the candidate `Rate` equals observed 101/121/114 solely from one seed.

NEXT: run instrumentation with exact matched original private ROM+Mesen (or source-verified CPU tracer) on two independent fresh startup histories and archived starts; verify event ordering, seed writer and RNG/character/difficulty paths across the 14/27 countries. Then implement true Spring seasonal processing/3 orders and controller Trade, AI, battle aftermath, diplomacy, dynasty, save, Windows Xbox campaigns. Source comparison alone is not a functional substitute. Do not disable recurring implementation.

## Reproduction from this archive

```sh
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target gk1_native_shell test_gk282_stock_rng_trace -j 6
ctest --test-dir build -R 'gk280_original_rate_bridge|gk282_original_c289_trace|GK1_278_OriginalSpring24DirectFieldsAtomicAndNoFakeParity' --output-on-failure
```

Set environment `GK1_PRIVATE_ORIGINAL_ROM=/private/owned_stock_USA.nes` during CMake configure only for nonpublic original-ROM positive test; never store the ROM in the archive. The patch and manifest identify exactly the three modified existing source files and four new source files. The complete ZIP preserves all other EXEC0280 members byte-identically.
