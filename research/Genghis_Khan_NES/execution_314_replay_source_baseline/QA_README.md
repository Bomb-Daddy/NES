# EXEC0314 — Source-baseline continuity across tactical replay restores

## Verified predecessor and reason
As of 2026-11-15 ~21:30Z, highest published `Bomb-Daddy/NES` execution and Collaboration Drive handoff was EXEC0313 (Win32 CI227). This incremental patch applies AFTER latest staged native C11 code; it does **not** replace the complete game. The full locally retrieved cumulative EXEC0300 source ZIP (SHA256 `3422a6fde1a4c1d8d71ff307b8da3c661cebff731002c2e2460681a76d121dfc`) was the independent integration test base. Later source additions do not alter the exact `retain_original_templates` anchor; patch staging checks it fail-closed.

## Real bug, reproducible negative control
After source CSV installation, `gk1_app_restore_path`, `_restore_divide` or `_restore_session` created a fresh app and retained both mutable scenario worlds but discarded `finance_source_baseline[2]`. This broke authenticated original 14/27-country baseline identity and finance comparison, despite the actual country templates surviving. The new regression *failed* against the unmodified inherited native app on line 14: `a->finance_source_baseline[w].phase == GK1_PRE_SETUP_TEMPLATE`, and passed with the one-function fix.

## Actual executable change
One `memcpy` in existing `retain_original_templates` now preserves both immutable source baselines. It changes no original ROM, player attributes, source-country fields, controller menu behavior, state phase, trade, turn/order budget, previous campaign, or preexisting saved files. The new C11 regression covers all three actual native research-restore functions, both source scenarios (14 and 27), detached mutable-world drafts, and negative replay rollback. 8 Python hostile staging tests validate two input anchors, duplicate rejection and injected failures after 1/2/3 writes restoring original source.

## Reproduction
`python3 -m unittest -q test_stage314.py`
`python3 stage314.py /path/to/latest-staged/execution_199`
`cmake -S /path/to/latest-staged/execution_199 -B /tmp/gk314-build -DCMAKE_BUILD_TYPE=Release`
`cmake --build /tmp/gk314-build --target test_gk314_replay_source_baseline -j4`
`ctest --test-dir /tmp/gk314-build -R GK1_314_ --output-on-failure`

## Acceptance limits
VERIFIED_STATIC: original stock ROM private identity known, source extraction and exact patch verified. TESTED_PORT_LINUX: test314 pass; related prior C11 tests 5/5 pass; new test ASan+UBSan pass. OBSERVED_RUNTIME_STOCK: original archived and fresh captures from prior runs, **not rerun now**. TESTED_PORT_WIN32_CI: previous EXEC0313 CI227 pass only; **EXEC0314 not run on Win32**. Physical Xbox and full campaigns NOT RUN. Original fresh 14/27 Spring generator remains OPEN; no new NES first Spring or authentic merchant event is claimed.

Copyright: ZIP/source contain **no** original ROM, licensed audiovisual assets, RAM snapshots or original source tables.
