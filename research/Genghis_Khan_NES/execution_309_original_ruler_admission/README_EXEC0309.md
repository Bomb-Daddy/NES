# GK1 EXEC0309: original-stock HUMAN ruler admission (integrated C11 patch)

## Scope / no false Spring acceptance

This source patch corrects a real player-facing original-stock checkpoint admission flaw in the complete inherited Gk1App. A copied or external original-stock first Spring is *not* itself evidence that a country is a human-selectable ruler. Original country records for all 14 Mongol and 27 World countries remain intact. Only allowed original human rulers are Mongol country index 0 (Temujin), World 0/6/10/18; the others remain viewable, but cannot be falsely entered as human player checkpoint. Neither original RNG nor Spring reconstruction is implemented by this patch. Classic vs Enhanced profile logic unaffected.

The `stage309.py` refuses an unknown source SHA, duplicates, missing verified EXEC0307 app registration, malformed CR, or tampered test; stages C source, CTest source and CMake atomically with rollback. It does NOT import licensed ROM assets, original-country data or emulator captures. The code is mounted into the original existing C11 Gk1App, never a standalone replacement game.

## Reproduction

Run `python3 test_stage309.py /path/to/PRESTAGED_EXEC0308_COMPLETE_GAME/execution_199` (the inherited source must contain GK1_307 registration and unmodified gk162). Then `python3 stage309.py /path/to/PRESTAGED_EXEC0308_COMPLETE_GAME/execution_199`, `cmake -S ... -B ... -DCMAKE_BUILD_TYPE=Release`, `cmake --build ...`, `ctest --test-dir ... -R GK1_309_StockHumanRulers_FailClosed14And27 --output-on-failure`. For Windows stage after stage307 and before CMake, on native WinMain source. `stage309.py` validates source SHA and refuses unknown variants; never bypass refusal by reusing a different earlier source.

## QA evidence

- VERIFIED_STATIC: source SHA, original source SHA and exact diff, 19/19 Python hostile checks.
- TESTED_PORT_LINUX: full complete native Release build succeeded; 235/235 eligible CTest prior to exact generated-comment reconciliation; subsequent exact stage-generated C11 source full build PASS and three relevant CTests PASS. Full subsequent 235-test rerun was interrupted while executing historical long-running X11 tests; not claim completed.
- Negative control: original gk162 without fix FAILS `GK1_309` at line 28, then patched source PASSES.
- OBSERVED_RUNTIME_STOCK archived/fresh: older original Mesen records preserved; NOT rerun this implementation.
- TESTED_PORT_WIN32_CI: NOT RUN for EXEC0309; previous EXEC0308 224/224 independently retained. Physical Xbox hardware and complete campaigns: NOT RUN.
- Original source-authentic first Spring full RNG/abilities/difficulty and three usable controller orders still OPEN. No complete release acceptance.

PRIVATE original USA ROM SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3 and 34-member gameplay ZIP remain only in private Genghis Drive; never ship in this source package or public GitHub. No agent acknowledgement from publication; do not disable recurring work.
