# GK1 EXEC0217 — Implementer / Hostile QA Hand-off — Native Classic Spring Finance Preview

2026-11-12. Status: **UNCERTIFIED RESEARCH CANDIDATE** `1.99.0-dev1`. Source predecessor **EXEC0216** checked in Drive Collaboration/GitHub; complete native app preserved in `execution_199` and eight inherited linked historical directories.

## Exactly what was implemented

- `gk217_finance_preview.c/.h`: read-only source-bound `gk217_preview_country`. Requires installed 14/27-country canonical PRE_SETUP source, validates whole-country identity through existing gk214 financial engine; copies assembled view only on success. Rejects incomplete models; never unlocks Spring, RNG, seasonal orders, original merchant presence or changes the world.
- `gk1_app_state.[ch]`: new `GK1_SCREEN_SPRING_FINANCE_PREVIEW`; Classic two consecutive START from labor plan opens it, DPAD navigates; A confirms read-only, B/START closes. Enhanced START still enters Enhanced Demobilize. Classic original trade gate and pre-setup state unchanged.
- `gk1_native_shell.c`: actual 1280×720 X11 rendered view showing current country Gold/Food/Troops before/after, tax, wages, feeding, starvation losses and 14/27 all-country totals; explicit "NOT GAME START" and "NO RNG, CHARACTER SETUP, SEASON, AI OR 3 ORDERS" labels. `--smoke-classic-finance 120` tests real native rendering and controller-app input path.
- `test_gk217_finance_preview.c`, `CMakeLists.txt`: 41-country positive source comparisons, corrupt whole-world country identity, invalid phase/cursor, atomic failure, Classic vs Enhanced controller navigation, native Xvfb integration. Seven code paths exact patch replay.

## Exact reproduction commands

```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build --output-on-failure -j8
xvfb-run -a -s '-screen 0 1280x720x24' build/gk1_native_shell --smoke-classic-finance 120
cpack --config build/CPackConfig.cmake -G DEB -B build
mkdir -p extracted; dpkg-deb -x build/gk1-native-research_1.99.0~dev1_amd64.deb extracted
xvfb-run -a -s '-screen 0 1280x720x24' extracted/opt/gk1-native-research/gk1_native_shell --smoke-classic-finance 120
```

Source replay: extract EXEC0216 complete ZIP; `git apply --unsafe-paths GK1_EXEC0217_Integration.patch` at the archive root; compare seven source paths SHA256 against EXEC0217 full source ZIP; configure, build, 181/181 clean CTest. Focused sanitizer test: configure CMAKE_C_FLAGS `-fsanitize=address,undefined -fno-omit-frame-pointer` and CMAKE_EXE_LINKER_FLAGS `-fsanitize=address,undefined`; run `test_gk217_finance_preview <build>/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv`.

## Recorded results and evidence classes

- VERIFIED_STATIC: 7/7 exact source replay, 14+27 genuine source-template country IDs, 41/41 projection comparisons against same existing finance code, negative whole-country identity, invalid phase, overflow/refusal atomic, original records unchanged, isolated Classic/Enhanced menu behavior.
- OBSERVED_RUNTIME_STOCK fresh WorkRAM / archived SaveRAM: previous authenticated source retained, **no new Mesen execution**. Existing stock evidence is not a hard-coded new first Spring.
- TESTED_PORT_LINUX: Release 181/181, Debug 181/181, independent clean rebuild 181/181; **focused ASan/UBSan runtime** PASS; native and Debian Xvfb graphical 120 frame PASS. Physical Xbox NOT RUN.
- NOT RUN: Win32/NSIS installer, physical Xbox controller, first playable all-country authentic original Spring or whole campaign.

## Open gates

Original startup seed/ruler/difficulty, RNG draw/store chronology, actual setup and authentic Spring order 3; original Trade merchant generator and player operations; autonomous AI; dynasty/diplomacy, battle ends, Win32/NSIS and physical-Xbox full campaigns. Next implementation must integrate confirmed transforms, not make the UI pretend the game is playable.

**Safety/publication:** original `.nes` ROM and Mesen states, raw country bytes and licensed screenshots/audio private only in Genghis research. Copyright-clean new C11/tests/patch and derivative ledger only in public GitHub and Collaboration Drive. Publication is not read/execution acknowledgement by any agent.
