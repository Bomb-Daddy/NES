# Run016 Classic NES Q14 no-refuge training correction — 2026-11-06

Project: **SUP-0016 canonical NES Classic**. Do not confuse this source with Original-KOEI Enhanced or DOS.

## Exact ancestry
- Immutable Run015 parent full-source SHA256: `fdbbc394cbffe98044f994fe498d6445b912a6510fd175178257da3242c73506` (160 ZIP members).
- Run016 complete candidate SHA256: `a0e218c87734b833c92821fff3dd1afd67e51f0d5494da9bee8ce8bcca1fb0cd`; 4,327,529 bytes; 161 CRC-clean ZIP members.
- Existing entries: 159 unchanged, one changed `src/rotk2/battle/engine.py`; new `src/tests/test_run016_q14_no_refuge_training.py`. No deletion or silent overwrite.
- Canonical source/evidence on Drive: https://drive.google.com/drive/folders/1W2pTK4Wocijmxpr864orPSibR_xftf93 ; full source Drive ID `1HJQUQ66K_6D-aIMmBk0z6omp-KedyVgY`, QA Drive ID `1Zx7CRHOJdA6zrSIBIBHkvKMcqnZwa1pT`. Independent Drive download bytes match and ZIP CRC passes, including QA SHA256 `c341bebad3febb320845c079e6356018ab02366b75ec57da7803012c336143b5`.

## Source-backed behavior and code
The documented original authorized ROM source path sengo $84F6 -> root $4EB0 makes an ordinary released officer with no legal refuge a free officer in the contested province with 0 troops and training exactly 100. Existing Classic did not reset the training byte (17 remained 17). Run016 sets `o.training=100` on only that source-matched branch in the production battle engine. Refuged Release and Recruit do not reset training. Source reference: [verified original findings](../../../source_findings/q14_q09/STATIC_FINDINGS_v1.00.md) (VERIFIED_STATIC, source bytes checked in prior work). Original special-flagged route $4ED8-$4EE8 is outside this patch; do not extrapolate.

## Actual executed tests
- RED original parent: **2 failed, 2 passed**, including wrong no-refuge training.
- GREEN targeted: **5 passed**, including real-state settlement, disk `.ro2` save/cold-load and JSON cold-load, no-refuge, refuge and Recruit negative controls and actual Linux Xvfb software-controller `BattleScreen` -> `ListPrompt` Release selection.
- Applicable Linux Python 3.13/Xvfb broad suite: **287 passed, 37 skipped, 1 deselected**. Fresh full-source ZIP extraction produces **the same result**. Reproducible archive rebuild with parent SHA guard matched byte-for-byte.
- Deliberate mutant reset-to-99: **3 expected failures, 2 passes**. Unfiltered test collection **NOT GREEN**, 3 collection errors from absent private `ROTK2_TEST_ROM` and historical import-path tests. Logs, full production source, SHA member manifest and deterministic rebuild script are in Drive QA archive.

## Completion and limits
Q14 ordinary no-refuge free-officer training: VERIFIED_STATIC original bytes, TESTED_PORT actual Linux engine, OBSERVED_RUNTIME Linux software-controller only, overall PARTIAL. Original emulator event replay this run is REPORTED_UNREPRODUCED/NOT RUN; native Windows, current-build physical Xbox, Enhanced shared RNG, full NES parity and Director release all OPEN/NOT VERIFIED. No original-NES emulator screenshot, Windows executable or hardware test claimed.

Enhanced Run007 full-source SHA `15ed58d6a0bba0f824bccfbc864a55cc3267bef004071d336f8df0c76b6b6a92` **not present in inspected original-game Run007 Drive folder**. Therefore merging Run009's 18-part parallel donor (SHA `e546d49dfe1f12f290445907997a5dc64f48d67afea8907705cb04abdae971d8`) into a presumed Run007 without exact base is BLOCKED. Preserve its source-derived Q09 capture/RNG and Enhanced marketplace/economy/BP.

Next executable Classic correction: source-characterize root $4ED8–$4EE8 flagged-officer fallback, then red/green engine/controller/save/negative QA. Enhanced next: recover verified Run007 complete source from producer worktree or exact verified binary, selective Run009 GUI and Run006 per-officer merge, full suites and 30 source-golden event traces. Never merge NES Classic source trees.
