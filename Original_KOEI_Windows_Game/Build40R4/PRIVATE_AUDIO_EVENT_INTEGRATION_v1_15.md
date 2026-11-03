# Third-branch v1.15 — all-in-one PRIVATE audio + event gameplay build

2026-10-30 — Zhuge Liang producer QA, third-branch original strategy game only.

## What changed from v1.14

* The complete, unchanged 15-track Soundware 1989 MP3 archive is installed under `third_branch/assets/audio/soundware_1989/` inside THIS game distribution. No companion ZIP and no manual merging. The 14 original (newly generated) SFX WAVs are also included.
* The game checks per-track SHA-256 and sizes via `third_branch.audio_system.verify_private_assets()`; all 15 MP3 files decode as MP3 via FFprobe. Pygame-ce is included in `requirements-audio.txt` for a cross-platform backend, and fallback playback is in `audio_system.py`. Actual Windows speaker playback is still NOT RUN.
* `third_branch.game_events` processes real deterministic game events under the versioned `OG_EVENTS_V1_PROVISIONAL` policy (new historical 189 campaigns only). The seven environmental event types cause bounded changes to actual province food, support, flood protection, gold, or available workers. The timing is deterministic, source-dependent, and limited to at most one monthly province hazard. These probabilities and resource effects are ORIGINAL-GAME PROVISIONAL DESIGN, NOT authentic ROTK II NES constants.
* Actual Ally/Joint diplomatic commands can deliver or have a secret letter intercepted. Actual friendly gold/food shipments exposed to hostile borders can lose cargo to interception. No intercepted letter/commodity art appears without a real command and resulting state change.
* Rare actual ruler death causes successor appointment, original officer retirement, army transfer without unit creation or destruction, faction label update, and funeral notification. It is never substituted for the unrelated elimination of a faction. Provisional no-age monthly mortality is NOT certified NES parity.
* The event visuals are read from unchanged, hash-verified source PNG files. Actual committed event records are persisted in `event_history`, `event_feed`, and the normal transactional journal. Invalid image or wrong province fails closed. The 1280x720 Tk map opens a controller A/B-closeable art panel with effect explanation and distinct audio cues on actual saved events.

## Actual tests and exclusions

* Full tests: `PYTHONPATH=. KOEI_DISABLE_AUDIO=1 pytest -q --disable-warnings --tb=short`: 338 passed, 2 expected failures, 3 subtests passed, exit zero.
* Twelve new+extended tests exercise complete soundtrack manifest, real monthly event roll and deterministic replay for 24 months, all seven environmental events, three genuine courier command outcomes, succession with troop conservation, save/load, and invalid event rejection.
* Independently observed real 24-month campaign seed=77: Month 5 epidemic p6; Month 6 locusts p9; Month 8 riot p16; Month 12 rebellion p3; Month 15 falling star p3; Month 18 flood p14; Month 20 flood p32; Month 22 flood p11. All 24 states cold loaded. Inspect `EVENT_TRIGGER_QA_24_MONTHS.csv` and `month24_save.ogsave` in QA evidence.
* Actual graphical Xvfb/Tk 1280x720 event panel opened on recorded Month 6 Province 9 locust plague; screenshot `Actual_Saved_Event_UI_v115.png`. The real event deducted 38 food and 20,000 support ticks.
* Actual `ffprobe` inspected all 15 MP3 codecs; SHA256/bytes validated against original private album manifest. Linux backend/silent fallback tested, but **speaker-audible Windows gameplay, physical controller, rights for public distribution, and 120-month 12-profile economic rebalance against this new event policy are NOT RUN**.
* Eight story stills and the 4 season stills remain source-staged; six unseen original-game story screens are not forcibly displayed as fictional game events. Event UI does not invent story sequences. Capital art and unassigned adviser still pending.

## Private recordings rights

These original commercial arrangements and ROM art are for the Project Director's PRIVATE development/testing, not authorized for public storefront distribution. Do not publish the combined ZIP without licensing review/rights clearance. The source file contains music bytes because the Director explicitly requested a self-contained private testing game. Public share must strip/licence the commercial MP3 and ROM-derived art first.

## Execution

Windows: `py -3 -m pip install -r requirements-audio.txt`, then `Play_Third_Branch_PC.bat` (uses local soundtrack automatically). Linux: `python -m pip install -r requirements-audio.txt`, `./Play_Third_Branch_Linux.sh`; Xvfb test requires an available SDL audio backend or `KOEI_DISABLE_AUDIO=1`. Screen: 1280x720. Existing archived saves remain loadable; this provisional event policy is added only to new Scenario 1 game states.
