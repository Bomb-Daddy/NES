# Enhanced Run018 — English NES Q09 attacked-destination arrival (PARTIAL)

**Status: TESTED_PORT / PARTIAL; NOT a finished Windows release.** This research branch publishes source-backed patches and regression evidence; **the canonical source is the separate Original-KOEI Google Drive root** `1i5kLog2o1tC9c2dsjdF2CLFnXYQHVk_l`. Do not mix with Classic NES `ROTK2_NES_Windows` or DOS, or with `Original_KOEI_Windows_Game/Build40R4`.

## Immutable ancestry
- Direct parent: Enhanced Run017 full source ZIP SHA256 `d052013bff0cad1a3fc9be9902647f9d087aba19f6c19609d9aaa9092ec115a6` (534 members).
- Run018 local candidate: SHA256 `95a9e1133684d7b18841d275bf8d32d0b45b814b30521836cbf5922dd56e3ba5`; 148741521 bytes, 535 CRC-clean ZIP members; 532/534 unchanged from Run017. Deterministic rebuild identical twice.
- Source of behavior: SNES hsovl2 D648/D69A and Classic source-backed Q09 regression, **not a direct original NES emulator replay**.

## Production correction
Apply `RUN018_PRODUCTION.patch` **ONLY** to exact Run017 extracted Enhanced original-game source. Changes `third_branch/officer_battle_adapter.py` and `third_branch/integrated.py`, adds `tests/test_run018_q09_attacked_arrival.py`.
A successful source-tagged Flee or whole-army retreat into a province already under attack immediately moves the real officer and its normal strategic troop station, while storing unscaled original battle strength in `q09_pending_attacked_arrivals`. This record is explicitly `TACTICAL_ACTIVATION_UNVERIFIED`: the **second simultaneous recipient tactical battle is not playable yet**, and four-slot acceptance is OPEN. Source-unknown fallback and peaceful behavior unchanged, market/economy/BP unmodified.

## Actual Linux Python3.13/Xvfb test evidence
- Pristine Run017 + new regression: **5 failed, 2 passed** (expected RED).
- Corrected Run018: **7 passed**.
- Mutant suppressing attacked-recipient write: **4 expected failures, 3 passed**; restored 7/7.
- Selected actual Enhanced battle/controller/economy suite: **223 passed**.
- Fresh extracted new full-source ZIP: **40 passed** relevant modules.
- Software-controller NES Flee menu through real Campaign: PASS under Linux Xvfb.
- Actual .ro2 process#1 save, process#2 cold-load: PASS, 10147-byte save; state fingerprint `de632c8da2502b3f8c57b76ff4668981b1695b6910e237b40893d650be16fdd7`.
- Full unfiltered pytest has **2 pre-existing collection blockers** (private ROTK2_TEST_ROM, historical rollback dotted import); attempted broad run timed out around 70%, **not fully green**.
- NO actual original NES 30 golden replays, native Windows, physical Xbox, or Director acceptance.

## Publication status
Drive Run018 folder: https://drive.google.com/drive/folders/1jfOKUY10stIyNpSbKEve_WGitXhIodAn , correct original-game parent confirmed.
**FULL SOURCE/QA DRIVE BINARIES BLOCKED**: upload via Library returned `container_session_unavailable`; direct connector returned `BLOCKED_FILE_REFERENCE`. Do **not** call Run018 Drive-published until all 18 source parts and QA archive are independently read back to matching SHA/CRC. Latest fully Drive-verified Enhanced parent remains **Run017**.

## Next executable code
Actual second simultaneous tactical encounter at attacked destination, preserving original four-list actor/controller phase and battle-scale troop unit; original emulator traces, ordinary-scenario source officer byte hydration, native PC/physical controller acceptance. Preserve all former run archives and exact source ancestry.
