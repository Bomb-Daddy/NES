# EXEC0316 — unified original CPU event claim ingestion + native controller hostile QA
Date: 2026-11-15. Predecessor: EXEC0315 (Win32 CI228 success).

## Source change actually implemented
- Linked NEW `gk316_cpu_claims_ingest.c/h` into inherited native C11 `Gk1App`, with `gk1_app_original_cpu_claims_parse` gated to Classic PRE_SETUP `GK53_SETUP_AWAIT_SOURCE`. This is NEW executable code, not a separate strategy prototype.
- Parses bounded 4096-event original NES CPU claim transcript (NMI, mapped STORE with A/X/Y, C289 pre/post), rejects invalid headers, malformed line-endings, overflows, chronology, bad targets or duplicate fields atomically. The transcript's stock ROM SHA is an assertion by the file, not independent proof of source identity. Does not mutate country state, generate statistics, grant orders or claim Mesen execution.
- Added a distinct, copyright-clean `GK1_EXEC0316_Private_Stock_CPU_Trace.lua` intended for the user's private original-stock Mesen research (untested on actual Mesen this run). Its output MUST remain PRIVATE.
- Added 300,000-input deterministic hostile original app/controller stress, with immutable source-baseline, both campaign country count, no fake Spring and candidate/ruler uniqueness controls.
- Added `stage316.py` source-hash-pinned, atomic additive integration with refuse/rollback tests. Preserves existing complete game, setup, source-gated Classic behavior, and Enhanced profile.

## Local TESTED_PORT_LINUX
- `cmake -S /mnt/data/gk1work/execution_199 -B /mnt/data/gk1build -DCMAKE_BUILD_TYPE=Debug`: PASS.
- `cmake --build /mnt/data/gk1build -j8`: PASS, including native 1280x720 X11 executable.
- `ctest --test-dir /mnt/data/gk1build -R GK1_316 --output-on-failure`: **2/2 PASS**, 51 new C11 parse/stage checks and 300,000 controller samples.
- `xvfb-run -a ctest --test-dir /mnt/data/gk1build --exclude-regex 'GK1_257_CurrentWin32Workflow|GK1_226_FourSeparateX11' -j8`: **237/237 eligible PASS**, includes rendered X11 and cold-load tests.
- Entire unfiltered 239-test suite did NOT pass: the older inherited archive contains a stale Windows-workflow source metadata gate (one known failure) and four-separate-X11 cold-load test exceeded the tool-client time limit. Neither exclusion implies those tests passed.
- Sanitizer source: `cmake -S ... -B /mnt/data/gk1asan -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'`, `cmake --build ... --target test_gk316_cpu_claims test_gk316_controller_fuzz -j8`, `ctest --test-dir /mnt/data/gk1asan -R GK1_316 --output-on-failure`: **2/2 PASS**.
- `python test_stage316.py`: **8/8 PASS**; test source SHA pinning, duplicate rejects, missing anchors, rollback after injected failure.
- `stage316.py /mnt/data/GK1_EXEC0316_baseline_stage` applied successfully to complete inherited EXEC0300 source C/H/CMake preimage. Actual new code was separately compiled and executed against the inherited full source.

## Honest acceptance states
- VERIFIED_STATIC: project source hash-pinned; source and current ROM identity historical evidence remain separate.
- OBSERVED_RUNTIME_STOCK_FRESH_RESET: retained prior original stock screenshot, 14 real 83-byte country records, source input pattern; **NO new Mesen run**.
- OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: retained previous 14/27-country first-Spring captures, not compared to new runtime capture in this run.
- TESTED_PORT_LINUX: C11 and 237 eligible tests PASS. Not universal original NES parity.
- TESTED_PORT_WIN32_CI: prior EXEC0315 CI228 retained; EXEC0316 **NOT RUN** on Windows.
- PHYSICAL_XBOX: NOT RUN. FULL_CAMPAIGNS: NOT RUN. Release acceptance OPEN.
- Crucial unmet game requirement: authentic independent original-stock first playable Spring setup/RNG, 14/27 country transformations, 3 genuinely usable orders, followed by real Trade/AI/battle/dynasty campaigns. **Do not claim complete.**

## Source privacy
Public ZIP and GitHub files are code, source hashes, tests and QA logs ONLY. No original USA NES ROM, modified ROM, RAM/SRAM states, Mesen game screenshots, copyrighted audio or original country records. Raw new trace output belongs to private Genghis research Drive, never public GitHub.

## Reproduction
Against verified latest complete C11 app state after inherited source staging:
`python research/Genghis_Khan_NES/execution_316_stock_cpu_trace_ingest/test_stage316.py`
`python research/Genghis_Khan_NES/execution_316_stock_cpu_trace_ingest/stage316.py current-gk1/execution_199`
`cmake -S current-gk1/execution_199 -B build -DCMAKE_BUILD_TYPE=Debug`
`cmake --build build -j8`
`ctest --test-dir build -R GK1_316 --output-on-failure`

Mesen Lua is provided but NOT executed or validated in the original console this run. Source trace claims must be independently reviewed and passed through original authenticated `gk302/gk304/gk312` before trusted source correlation. Publication does not imply agent acknowledgement.
