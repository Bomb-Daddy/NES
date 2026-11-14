# GK1 EXEC0277 — Fresh original-stock title provenance hardening
**Date:** 2026-11-14 · **Status:** tested C11 production code, not release.
**Predecessor:** EXEC0276, verified at Drive Collaboration and GitHub before changes. Complete inherited C11 source ZIP SHA256 `2c78a73583f061fc8be2d4c567aa4b0fc68a785cce26d13acf9a3416f20e9c36`.
**Scope:** the existing Genghis Khan I English NES-to-native 1280×720 C11 Linux/Win32 game, not ROTK II or a separate prototype.

## Concrete bug and correction
In `execution_199/gk162_original_entry.c`, `gk162_original_setup_compatible` formerly returned true for fresh stock `GK77_FRESH_MONGOL_WORKRAM` **whenever setup_stage[0]==GK53_SETUP_PLAYERS**, without checking changed candidate, difficulty, ruler, manual bonuses or active campaign. Any change that returned the wizard to the PLAYERS stage was therefore eligible to import the ONE original-stock fresh 1175 Mesen history and misattribute it to an unrelated choice. The separate `GK53_SETUP_AWAIT_SOURCE` branch also ignored residual method cursor and bonus edit cursors.

EXEC0277 reuses the already-shared `pristine_title` check for the fresh PLAYERS case and requires active Mongol campaign (world0). The positive AWAIT_SOURCE case retains Temujin, one player, difficulty1, automatically generated abilities and no manual allocation; cursor/bonus-editor fields must be at their pristine defaults. Archived original profiles retain their EXEC0276 title-only gate without change. Both direct raw-record and private JSON Mesen import paths reach this shared predicate. No copied RAM, ROM, licensed artwork, original audio or checkpoint is published.

## Actual executable QA
- Unmodified EXEC0276 with *new* C11 test: `GK1_277_...` **FAIL** on altered title candidate (correct adversarial reproduction).
- Patched EXEC0277 with identical regression: **PASS** (untouched-title, positive known-choice wizard, edited candidate/difficulty/ruler/slot/ability method/method cursor/manual bonus/person/ability/other active campaign; full `Gk1App` memory rollback on refused inputs). Production shared entry is called through both raw-record and fresh-JSON paths; nonexistent private filenames prevent test from pretending source data was present.
- Exact full Linux native Release game compiled/linked; complete *applicable* `ctest --test-dir ... -E 'GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass|GK1_226_.*' --timeout 25 -j 10 --output-on-failure` **223/223 PASS**. Three inherited registrations (obsolete Windows provenance and two X11 multi-process tests) excluded, not passes.
- Exact `git apply --check && git apply` on pristine EXEC0276 files; all three changed/new production, CMake, CTest files reproduced exactly. Archive inherited 1132 prior entries unchanged except production `gk162_original_entry.c` and `CMakeLists.txt`; one new regression and five release QA files.
- No new headless Mesen run, no independently generated original NES Spring, no Win32 build in this execution, no physical Xbox controller and no full campaigns.

## Reproduce
```
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release -DPython3_EXECUTABLE=/usr/bin/python3 -DPython3_FIND_VIRTUALENV=NEVER
cmake --build build -j12
ctest --test-dir build -R GK1_277 --output-on-failure
ctest --test-dir build -E 'GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass|GK1_226_.*' --timeout 25 -j10 --output-on-failure
```
This patch is a source-provenance correction, not original-stock first Spring RNG. Do NOT mark Spring setup generated or three native original commands implemented.

## Remaining acceptance gates
Independent authenticated original NES source trace for ruler/difficulty/character RNG, fresh and archived separately; source-authentic 14/27-country seasonal startup; real Trade merchant presence/RNG; autonomous strategic AI, war aftermath, diplomacy/dynasty, audio/graphics; full long-campaign cold-save acceptance, real Windows executable regression and physical Xbox hardware. Maintain privacy separation and published numbering; do not stop hourly development absent Director instruction or accepted complete release.
