# Run015 Classic Q09 opponent-captured-list implementation, 2026-11-06

Binding: player-observable English NES ROTK II battle fidelity (NOT source-copy identity). Canonical NES Classic source only, SUP-0016; separate Original-KOEI Enhanced and DOS unchanged. Derived mechanics from approved SNES/NES unified v1.4 Q09 `hsovl2 $D6AC–$D6C8`.

Parent Run014 full archive SHA256 ddb6ee5693e6dce140f4e216d14d241e8c1239aa8ddfb66fd1b7257d46ade398; 159 entries. Output Run015 full source SHA256 fdbbc394cbffe98044f994fe498d6445b912a6510fd175178257da3242c73506; 4,325,906 bytes, 160 CRC-clean members. Source ancestry: 157 parent members unchanged, only `src/rotk2/battle/engine.py` and `src/rotk2/state/models.py` altered, and `tests/test_run015_q09_opponent_capture_list.py` added. Deterministic builder and full member hash manifest in QA evidence.

Source implementation: a failed Flee immediately unlinks the officer on the map and records that officer ID in the opposing battle side's `q09_captured_by_side` list; the original officer record is NOT killed or subject to invented immediate disposition. Added save-compatible default and guard against malformed/duplicate/opposite-side captures. An accepted escape does not get captured, an illegal destination does not roll/mutate. This does not claim Q14 finished: interaction of mid-battle capture and later winner/player disposition remains to be verified.

BEFORE, unmodified Run014 source with seven new tests: `6 failed, 1 passed` (nonzero exit). AFTER source patch: `7 passed`, exit zero. Complete applicable Linux Xvfb selected suites: `282 passed, 37 skipped, 1 deselected`; clean ZIP extraction same. Negative mutation removing the captured-list append: `4 failed, 3 passed`, nonzero exit; restored 7/7 passing. Real `.ro2` file cold load, state hash equality and source-validated RNG two-draw failed escape are tested. No Mesen ROM session, no original-NES live replay, no native Windows or current Xbox hardware.

Exact suite command in ZIP-root:
`PYTHONPATH=src:src/tests:tests DD_TRACE_ENABLED=false DD_INSTRUMENTATION_TELEMETRY_ENABLED=false xvfb-run -a python -m pytest -q src/tests tests --ignore=src/tests/test_build23_q04_original_rom_adjacency.py --tb=short --disable-warnings -k 'not test_bundle_manifest_008'`

Private ROTK2_TEST_ROM missing; adjacency collection would error, not counted as a skip. Obsolete CLASSIC008 bundle-members test is deselected. Unfiltered suite acceptance is NOT claimed. Consult separate completion register for Enhanced Run007 missing-exact-source blocker, parity statuses and next executable step.
