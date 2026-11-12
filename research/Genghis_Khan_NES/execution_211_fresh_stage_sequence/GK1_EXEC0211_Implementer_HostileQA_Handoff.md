# GK1 EXEC0211 — Authenticated original-stock 14-stage reset chronology and integrated C11 validator

**Date:** November 11, 2026 (America/Detroit). **Project:** Existing English Genghis Khan I NES → native Windows/Linux port. **Exact predecessor:** EXEC0210 complete native C11 archive, not the stale EXEC0076/EXEC0062 branch. Candidate version remains `1.99.0-dev1`.

## Executable work, not another research-only memo

Added `gk211_fresh_sequence.c/.h` and `test_gk211_fresh_sequence.c` to the ACTUAL `execution_199` CMake application/library and added `gk1_app_original_fresh_sequence()` to the existing `Gk1App`. The same-app gate refuses non-Classic mode, modified original PRE_SETUP country templates, missing canonical CSV or an already active source checkpoint. Read-only result, rollback by construction; no mutation to countries/orders/controller/save.

The validator authenticates ALL FOURTEEN separate owner-recorded original NES Mesen fresh-reset controller-stage JSON files by complete SHA-256, checks CPU `nesMemory` mapping to physical `nesWorkRam` and blank `nesSaveRam` at **every stage**, verifies monotonic emulator frame/CPU cycle receipt ordering, and finally invokes the genuine native `gk165_extract_fresh_json` digest gate on the 14×83 original country bytes. All raw receipts, original ROM and licensed media remain PRIVATE, never in the public ZIP or GitHub source.

### Newly source-grounded event boundaries (one archived history only)

| Mesen archived capture | Observed original state transition | What this does **not** establish |
|---|---|---|
| Step 02 | 14-country 1174 data first becomes visible in WorkRAM | No independently synthesized base stats |
| Step 09 | Food 129 → 258 | Not a generalized country-wide setup formula |
| Step 10 | 1174 → 1175; Rate 0 → 121 | Does not prove RNG seed/draw order or difficulty |
| Step 12 | Gold 425 → 430; Food 258 → 208; Troops 60 → 61 | No universal seasonal economy formula from one history |
| Step 14 | Real original first Spring three-order menu appears | Port still cannot generate that menu independently |

This differs from earlier EXEC0114 compressed 6-row model: it authenticates and audits all 14 original detailed JSON stages with separate CPU/WorkRAM/SaveRAM views and real chronological CPU cycle stamps. The original fresh receipt Rate121 does not supersede the separately observed archived 1175 Rate101 or World1206 Rate114.

## Commands and actual outcomes

From the complete original EXEC0210 archive (SHA256 `23049050b89272369a12dbfd9acfc7d859e74628e1838201bef43ba402e0f902`), apply the public patch at source root:

```sh
git apply --check GK1_EXEC0211_CopyrightClean_Integration.patch
git apply GK1_EXEC0211_CopyrightClean_Integration.patch
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
```

**Independent exact patch replay:** all seven changed/new paths byte-identical SHA-256 to tested integrated source. **Linux Release:** 174/174 CTest PASS; **Debug:** 174/174 PASS; **independent clean source replay:** 174/174 PASS. **Private source validator:** 58/58 focused assertions PASS, including 14 authentic original source stages and owner original 41-country CSV. **ASan+UBSan:** same 58 assertions PASS. **Hostile QA:** 17 deliberately tampered/missing/swapped private receipt scenarios rejected atomically. **1280×720 X11/Xvfb native:** 120 frames PASS. **Debian-extracted wrapper:** 120 frames PASS. Separate Debian .deb built with CPack and extracted.

The private positive QA invocation is `build/test_gk211_fresh_sequence /private/stages execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv`; third argument may point to a *separately corrupted temporary copy* of private stages to verify atomic failure. Never copy private original data into public source/QA files.

## Evidence levels and blocking gaps

- **VERIFIED_STATIC:** 14 SHA-pinned archived source receipt identities; CPU/WorkRAM/SaveRAM cross-check; complete source ZIP identity, patch replay and QA data.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET:** the *previous* November 7 genuine original Mesen reset→Spring source capture; stepwise proof of one history, not a new emulation this run.
- **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** prior, separate archived Mongol1175/World1206 SaveRAM checkpoints; never conflated with fresh WorkRAM.
- **TESTED_PORT_LINUX:** actual current C11 game build/tests/sanitizers, independent clean build, graphical run and Debian extracted run.
- **NOT RUN:** new original NES Mesen execution/CPU RNG watch, actual native Win32/NSIS, physical Xbox controller, full games.

**Critical open milestone:** original exact setup/character RNG chronology and independent variable-source dynamic first Spring for all 14/27 countries; merchant appearance/RNG, full autonomous AI, battle victory/aftermath, dynasty, diplomacy, authentic art/audio, Windows installer and physical-controller campaigns. A read-only attested event chronology is NOT an executable stock startup or playable completed game. Preserve original Classic/Enhanced separation, all previous artifacts, and current recurring execution until Director explicitly stops or an accepted verified complete release.
