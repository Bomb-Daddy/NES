# Original KOEI Third Branch v1.17 — Direct Han AI Integration

**Date:** 2026-10-30 (America/Detroit)
**Owner and implementer:** Zhuge Liang / ChatGPT (not delegated)
**Branch:** `Retro Gaming/Original_KOEI_Windows_Game` — original Linux/Windows PC title; unrelated to NES Classic and DOS repositories.
**Source ancestry:** exact Drive v1.16 Windows/Linux ZIP SHA256 `d03696c5ebd3059c3c28877b7205c345749288bce6fa314f4fa2efa8ed151fe3`, reconstructed from its two Drive transport fragments with SHA256/byte-length verification. Only additive v3.2 AI overlay and compatible runtime integration applied; v1.16 assets, music, Windows launch scripts and game economy preserved.

## Direct code changes

- Installed the 12 Han-labeled playable AI-policy definitions (`config/KOEI_12_Han_AI_Profiles_v3_2.json`) and deterministic v3 decision engine in `third_branch/historical_ai_v3.py`, preserving `take_turn` imports through the wrapper in `third_branch/historical_ai.py`.
- **Closed previously missing live-game hook:** `third_branch/player_runtime.py` now runs the v3 Han policy even when the active historical scenario uses the Director's `OG_1000_DIRECTOR_V2` economy. Previously this live PC path selected an older v2 chooser, so the v3 names and policies were only test-candidate code rather than actual opponent turns.
- **Compatibility correction:** 1000-point cultivation threshold (instead of legacy 100) and corresponding scoring normalization in the v3 chooser.
- Added `tests/test_han_ai_v117_integration.py` proving profile coverage, current-policy live stage-month routing, nonmutating staging, and cultivation candidate threshold. Included all earlier v3.2 AI tests.
- The `200 BP` maximum and economy, province adjacency, historical rulers, office/officer catalog, Soundware MP3s, WAV effects and historical art remain unchanged. No naval mechanics created.

## Reproducible evidence

Host: Linux x86_64, Python 3.13, `PYTHONPATH=. python -m pytest -q` run in extracted source root.
**TESTED_PORT:** **354 passed, 2 expected xfailed, 3 subtests passed, exit 0**, full unfiltered summary output `QA_V117_FULL_PYTEST.log`. Previously known expected failure cases remain expected failures, not newly fixed.
**TESTED_PORT:** seed 17, actual 15-ruler historical Scenario 1, Director 1000 economy, PC `stage_month_with_opponents` for 12 months, 14 opponent slots each turn. 12 monthly `save_bytes` + cold `load_bytes` fingerprints matched, **153 committed nonbattle opponent actions**, final fingerprint `737823172fef5934673219c021b6c916ef8831fbc7df4353d705fca68fc954c8`.
**TESTED_PORT:** 11 targeted integration + v3.2 AI tests passed, exit 0.
**VERIFIED_STATIC:** source is copied from verified v1.16; no legacy source overwritten. Full source ZIP has all media and scripts. Source-file SHA256 manifest accompanies package.

## Known limitations / not claims

- **NOT RUN:** Windows-native execution, real physical Xbox controller, auditory device QA, new packaged executable. Existing v1.16 Linux graphical tests are inherited historical evidence, not rerun for v1.17.
- **PARTIAL:** 12 distinct Han profiles drive test factions; the 15-ruler engine reuses styles for three additional ruler slots. They are not 12 real multiplayer humans.
- **PARTIAL:** all style choices use current real legal command gateway, but 12-month smoke is not ten-year v1.17 strategy-balance acceptance. Earlier v3.2 ten-year experiment was against v1.10 and cannot be automatically promoted to this newer integrated source.
- **OPEN:** economic balance, 22 vacant province accounting, market-backed liquidity, Sun Ce military follow-through and AI difficulty/competence tuning need separate verifiable fixes.
- **NO OUTSIDE HANDOFF:** integration and test performed directly by Zhuge Liang. No assignment or read acknowledgement is required for another agent to count this artifact as produced; source integration verified locally, Drive publication recorded separately.

**Completion status:** `OG-HANAI-V117-INTEGRATION = PASS VERIFIED (Linux source and deterministic PC turn integration)`, `OG-HANAI-V117-WINDOWS = NOT RUN`, `OG-HANAI-V117-LONGRUN = PARTIAL`.
