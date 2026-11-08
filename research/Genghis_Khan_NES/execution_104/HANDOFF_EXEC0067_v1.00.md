# Genghis Khan I English NES-to-Windows — EXEC0067 integration handoff v1.00

**Date:** 2026-11-07. **Authority:** Director + 2026-11-07 primary manual audit 0003 v2.01, original published KOEI English NES manual pp.22–23, 25, 29. **Actual source baseline:** EXEC0066 exact ZIP SHA-256 `d29b65ca004c1f667fd6395ffb97656ed9ceedfb1db51b5b5433783b3133a84e`. Same native game, not a second executable or unrelated ROTK II/KOEI game. Entire prior 0066 source still present, unchanged in its own archive folder.

## Exact implementation

1. Correct `gk64_terrain_mobility(GK64_WATER)` from **8** to original manual's **6**; seven named terrains now 3/4/5/6/6/4/5. Original observed raw bank5 tile index0=8 has no proven classification and is untouched.
2. Correct false stock/modified Divide manual disagreement: original p29 cavalry minimum Mobility **13**, infantry/archer **11**. `gk64_divide_manual(unit_type,mobility,eligible)` implements both with no RNG; inherited `gk64_divide_cavalry(profile,mobility,eligible)` now returns 12-false/13-true in the manual-derived preview for either profile. Classic ROM parity **not tested**, confirmed Enhanced original Mesen 12/13 **historical evidence only**, not repeated in this run.
3. Add `gk64_season_geography` pure, read-only country-ID-based hazard **eligibility**. Winter severe cold: all Mongol14 and World1–7,15–19; summer storms: no Mongol and World2,18,19,23–27. The function validates both campaign source identities and all country indices and refuses invalid data without modifying the caller's output.
4. Render the new selected-country hazard eligibility directly in the **same existing controller-only country picker** and the existing manual seasonal preview. The game continues to display that its 41 original-country records are `PRE_SETUP_TEMPLATE`, NOT first playable Spring. No hazard RNG, tax, growth, crop harvest, strategic action, or event side effects were invented.
5. Add strict C11 test `test_gk67_manual_audit_corrections.c` with **335 direct assertions**, covering all 41 source countries, all seven terrain costs, 16×3 unit/Mobility combinations, 16×2 profile cavalry cases, invalid inputs, no world mutation and actual app controller scenario switch. The 6-vs-8 daughter marriage contradiction remains unresolved and guarded.
6. Version only the new installed package and menu title to **0.67.0-dev1**; preserve controller, battle/Divide, research viewer, market PRE_SETUP guard, Enhanced-only Demobilize, existing three-slot save/cold-load, original 41-column CSV, 1280×720 native UI and intended both complete campaigns.

## Execution results — not assertions of Windows/stock NES parity

Executed in `/mnt/data/gk1_exec0067_work` with the *complete* inherited EXEC0066 source unzipped, 0067 modifications made in place, then renamed to `execution_067` and old `execution_066` restored byte-for-byte from the original archive. All exit status zero unless indicated:

```sh
cmake -S execution_066 -B build_release -DCMAKE_BUILD_TYPE=Release
cmake --build build_release -j 4
ctest --test-dir build_release --output-on-failure       # 25/25 PASS
cmake -S execution_066 -B build_debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build_debug -j 4
ctest --test-dir build_debug --output-on-failure         # 25/25 PASS
cmake -S execution_066 -B build_sanitized -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer -O1' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build build_sanitized -j 4
ASAN_OPTIONS=detect_leaks=0 ctest --test-dir build_sanitized --output-on-failure   # 25/25 PASS
./build_release/test_gk67_manual_audit_corrections ./execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv  # 335 PASS
xvfb-run -a ./build_release/gk1_native_shell --smoke-scenario 80             # 80 frames PASS
xvfb-run -a ./build_release/gk1_native_shell --smoke-manual-rules 90         # 90 frames PASS
cpack --config build_release/CPackConfig.cmake -B packaged                    # Debian package PASS
# Independent source after restoring folder names:
cmake -S execution_067 -B clean_0067 -DCMAKE_BUILD_TYPE=Release
cmake --build clean_0067 -j 4
ctest --test-dir clean_0067 --output-on-failure                              # 25/25 PASS
```

Additional Xvfb 110-frame actual Linux captures `qa67_x11_country_geography.png` and `qa67_x11_manual_season_geography.png` visually reviewed: correct labels and source values, no text clipping. Debian archive extracted into isolated `deb_extract` and actual *separate native Linux* processes passed version, 14/27 roster, protected pre-setup save, quit and cold-load. Negative controls (exit codes): duplicate-save refused **6**; tampered save refused **7**; nonexistent scenario **4**; bad CSV SHA **8**. All stdout and stderr logs provided under `qa_exec0067` in complete source ZIP. Original 41-country CSV SHA-256 preserved: `fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581`.

**Classification:** MANUAL_DERIVED new rule/eligibility values; TESTED_PORT_LINUX_C11 and X11 for execution/tests/render; Linux Debian extracted native process and source-identity tests. No original native Mesen run, Win32 executable/NSIS installed launch, nor physical Xbox run in this increment. Historical Mesen evidence retained unmodified. No authentic first-Spring setup or actual seasonal event outcome is asserted.

**Current blocker to full player-ready world:** exact first-Spring automatic stat/setup/season transforms have not been established in matching original NES runtime or source. Next work must pursue verified startup behavior or independent source-backed real strategic order processing with phase gate, then complete Trade controller, dynasty/diplomacy/AI/battle aftermath and all intended campaigns. Do not remove or bypass true PRE_SETUP guard.
