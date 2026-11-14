# Genghis Khan I — EXEC0272: source-original Spring audit provenance

This is an actual C11 patch to the existing complete, inherited EXEC0271 native application, **not a replacement game and not completed NES first-Spring generation**.

The public game-level Spring finance batch auditor previously accepted a PRE_SETUP world after port labor-planner/Enhanced edits and described its results as a canonical source-template projection. This change verifies the mutable world against the retained original country baseline before permitting that label. It preserves the separate what-if view and rejects invalid input without modifying the game or output.

## Reproduce
Extract the exact EXEC0271 complete C11 source. From its root:

```sh
git apply research/Genghis_Khan_NES/execution_272_source_original_template_guard/GK1_EXEC0272_Integration.patch
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target test_gk214_spring_batch gk1_native_shell -j4
ctest --test-dir build -R 'GK1_(208|211|214|215)' --output-on-failure
(cd build && ./gk1_native_shell --audit-both-spring-finance-batches)
```

The patch changes exactly `execution_199/gk1_app_state.c` and `execution_199/test_gk214_spring_batch.c`. The test must FAIL on the unmodified predecessor (country1 Gold mutation wrongly accepted), and pass with the patch. Independently replaying the patch on EXEC0271 yielded byte-identical changed files. All other 1,094 inherited archive entries remained byte-identical.

## Verification on Linux
- Native `gk1_native_shell` compilation/link: PASS.
- C11 `test_gk214_spring_batch`: 220 assertions PASS.
- Source-backed CTest 208/211/214/215: 4/4 PASS.
- Existing archived original stock seed assertions: 41 PASS; existing fresh fourteen-stage original stock evidence assertions: 56 PASS, historical evidence only.
- Actual CLI audit covers 14 Mongol + 27 World **uninitialized templates**; it explicitly reports that it did **not** generate setup, RNG or player orders.

No new headless Mesen run. No actual Win32/NSIS/physical Xbox or complete-campaign testing of EXEC0272. The independent original-stock first Spring initializer (difficulty, ruler, character RNG, seasonal transforms) remains unfinished.

Original ROM, licensed data and raw private Mesen captures belong in the PRIVATE research Drive, never in public GitHub. This folder contains copyright-clean patch and reproduction instructions only.

Prior EXEC0271 predecessor: https://github.com/Bomb-Daddy/NES/tree/main/research/Genghis_Khan_NES/execution_271_win32_classic_view
