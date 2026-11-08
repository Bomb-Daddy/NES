# GK1 EXEC0075 — source-derived original NES Spring food/tax/payroll in same native C11 shell

**2026-11-07.** Incremental implementation on the full, immutable EXEC0074 combined Genghis I native application tree. All inherited files and BOTH PRE_SETUP country datasets retained. This is NOT first-Spring campaign unlock, original stock Mesen parity proof, a completed game, or a Windows/hardware test.

## What actually changes

`gk75_spring_economy.c/.h` implements bounded original English NES 6502 source bank1 `$8099` (Food and group-by-group shortage) then `$8106` (tax calculated using Economy/Tax/Morale and credited before one-Gold-per-troop wages). Original instruction-replay details from accepted v1.07 `04_Annual_engine.md`; the known observed winter1175→Spring1176 country1 source-controlled input reproduces Food289→238, Tax61, payroll61, Gold449→449, with town+10 intervention Gold453. This C calculation IS port code, not a new original ROM execution. It deliberately omits common work, all seasonal event/RNG, family, AI, growth and first Spring setup. Input output is atomic. Invalid raw game-range input is rejected.

Controller **A** in the inherited manual rules opens the 14-country archived Spring comparison. **START** toggles a second clearly labeled source-derived food/tax/payroll WHAT-IF preview for the original PRE_SETUP CSV values; D-pad selects country; B returns. The private archived 8192-byte RAM is still optional and hash-gated; preview is available without it. Both World and Mongol campaign worlds remain PRE_SETUP and byte-identical before/after; save/load remains blocked in this screen. The what-if projection is NOT the real Spring because setup stats/resources and common seasonal sequence are missing.

New CLI test contracts `--spring-source-preview 1 1`, `--spring-source-preview 2 1` and `--spring-source-pad-check`. No original ROM bytes, private screenshots, or save-state data are redistributed.

## Build from source ZIP root

```sh
cmake -S execution_075 -B build075 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build075 -j4
ctest --test-dir build075 --output-on-failure
build075/gk1_native_shell --spring-source-pad-check
build075/gk1_native_shell --spring-source-preview 1 1
xvfb-run -a build075/gk1_native_shell --smoke-spring-archive 60
cmake --build build075 --target package
python3 execution_075/packaging/test_installed_layout.py build075/gk1-native-research_0.75.0~dev1_amd64.deb
```

## Outstanding / next real step

User-authorized ORIGINAL stock English NES ROM exact SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` is not present in the inspected Drive private folder listing, recent original capture ZIP, nor the 40MB Temujin recovery ZIP inspected here. The recovered 40MB archive contains Mesen host code and research appendices but no `.nes`; do not substitute the modified game or synthesize original boot. Recover that exact private stock ROM and native matching Mesen, trace true setup/startup and Spring first player phase; only then advance the live scenario phase, AI and Trade menus. Daughter marriage ages6/7/8, natural water and battle terminal remain OPEN. Manual p29 cavalry Divide13 and p25 Ocean6 corrections already closed and preserved.
