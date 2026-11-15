# GK1 EXEC0315 — Win32 EXEC0314 source-stage test newline repair

Date: 2026-11-15. Inherits the full 1280x720 C11 game, original source research, and EXEC0314 immutable baseline preservation. This is a Windows QA/integration fix, **not** NES stock Spring, controller-device, or campaign acceptance.

## Reproduced real failure
GitHub Actions run 43766321392 (EXEC0314, 2026-11-15 21:55 UTC) failed before native C11 compilation. The failing step `EXEC0314 preserve authentic 14/27 CSV baselines across native tactical replay cold loads` ran `test_stage314.py` on Windows. Python text-mode `Path.write_text` translated the fixture's LF into CRLF; `stage314.SOURCE_ANCHOR` intentionally requires exact LF bytes and refused the fixture, producing five false `ValueError: original app replay-source anchor missing` errors.

## New executable source and tests
Updated `../execution_314_replay_source_baseline/test_stage314.py` to create its original C and CMake fixtures with `Path.write_bytes` and explicit UTF-8/LF bytes, without modifying the source authentication gate. Added `test_fixture_exact_lf_cross_platform` and `test_translated_crlf_fixture_rejected_without_mutation`. The latter deliberately feeds CRLF to ensure the fail-closed production staging validator still refuses drift and rolls back. All original rollback, duplicate, source-drift, and CMake-drift tests remain.

```bash
python3 research/Genghis_Khan_NES/execution_314_replay_source_baseline/test_stage314.py
```

Local Linux execution of the exact source at Git blob `d20e17dd724d8a2a57418ed4a5931e96a30b66bf`: 10/10 PASS. The previous Windows fixture failure was confirmed in real GitHub CI logs; current native Windows test/packaging result must be checked separately, never inferred from Linux.

Source readback: `research/Genghis_Khan_NES/execution_314_replay_source_baseline/test_stage314.py`. Only one existing source test file changes; the existing C11 source and all previous application mechanics remain intact.

## Scope and acceptance
VERIFIED_STATIC: exact source fix and prior failure independently identified.
TESTED_HOST_LINUX_PYTHON: 10/10 PASS.
TESTED_PORT_WIN32_CI: pending independent finished GitHub Actions run.
TESTED_PORT_LINUX_C11: prior EXEC0314 5/5 related native tests; no new full Linux C11 suite.
OBSERVED_RUNTIME_STOCK (fresh and archived): historical observations only; not rerun.
Physical Xbox and full campaigns: NOT RUN.
Stock first playable Spring with authentic ruler/character/difficulty/RNG, 14/27 countries and three controller orders: OPEN.

No original-stock ROM, licensed assets, SRAM, raw game screenshots, user checkpoints or campaigns may be published.
