# EXEC0234 — Native Genghis Khan I World four-player setup / hostile QA handoff

**Date:** 2026-11-12. **Source authority:** verified EXEC0233 complete inherited C11 source archive in Collaboration Drive and corresponding execution_233 Win32 patch chain; not a new rewrite or separate game.

## Executable change

`apply_world_roster.py SOURCE_ROOT` applies two fail-closed changes to the SAME native 1280x720 application: a World four-player committed-lord roster displayed during `GK53_SETUP_LORD` (unselected slots display `--`), and `GK1_234_NativeWorldFourPlayerSetupAndRollback` integrated into CTest. The actual game-controller routing and setup state are exercised, not synthetic copies. `test_gk234_world_four_players.c` sets Classic, visits World campaign (27 original countries), chooses four allowed lords with original country indices 0,6,10,18, sets four human slots/difficulty 5, reverses and restores the fourth selection, leaves original Spring gated, roundtrips v53 source-provenance setup bytes, and attempts each one-byte tamper with zero mutation. Both campaign templates retain PRE_SETUP phase. No new source-country values or RNG are hardcoded.

## Reproduction (Linux)

Use the exact prior inherited `GK1_EXEC0233_Complete_Inherited_C11_Source_Windows_PatchChain_QA.zip` and its embedded original complete `EXEC0230_Complete_Integrated_C11_Source_QA.zip`. Rehydrate chronological 231/232/233 overlays following predecessor README, then run:

```
python3 apply_world_roster.py PATH_TO_HYDRATED_ROOT
cmake -S PATH_TO_HYDRATED_ROOT/execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8
ctest --test-dir build --output-on-failure -j 6
```

The overlay is exact-anchored, refuses replay/changed source and does not touch private ROM files. `test_world_roster_overlay.py` performs clean baseline replay/equivalence and eight positive/negative checks when supplied the 0230 complete ZIP and the independently built 0234 source root.

## Results and release gates

Linux complete source: build success, 201/201 CTest PASS in 9.77s final QA (including installed/native graphical regression suites). Eight overlay replay, equality and refusal checks PASS. **No new native Windows build, NSIS, physical Xbox, Mesen original-stock run or complete campaign executed here.** Prior EXEC0233 Windows CI remains a predecessor result only. Original-verified private USA NES ROM was not read or republished this run. Genuine first Spring generation, character RNG, difficulty, all 14/27 playable seasonal turns and the full strategic game remain OPEN.

Public GitHub should include only this copyright-clean overlay/test and reproducible README. Original ROM / screenshots / RAM only private research. Publishing a handoff is **not** an agent read/execute acknowledgement. Preserve the hourly task until accepted release or explicit stop.
