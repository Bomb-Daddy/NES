# GK1 EXEC0050 — original 41-country scenario factory integrated into inherited native application

Status: **TESTED_PORT_LINUX** for two original *pre-setup* source rosters in the actual inherited 1280×720 desktop application. Not an original NES Spring-state oracle, native Windows acceptance, physical Xbox test, or complete strategy game.

## Source authority

Only the existing independent Genghis Khan I English NES-to-Windows source: `execution_048`, preserved original dependencies `execution_034`, `035`, `039`, `040`, `045`, `047`, plus source-verified `execution_049` (all 41 original 45-column numeric pre-setup records). Never mix with ROTK II or the separate custom KOEI-inspired game.

The exact numeric source file `execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv` is 10,345 bytes; SHA256 `fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581`. Its original published Git blob is `82126d270f45f3addcbebfd2bd196d5323476f9d`. Runtime now gates the source against the exact SHA-256 **before importing** it. The original 45-column parser additionally validates semantic relations. A valid-looking numeric mutation is NOT silently accepted as original.

The two scenarios are opened using `gk1_open_loaded_scenario` into separate **mutable** native `Gk1App.worlds[]` objects with 14 and 27 country records respectively, retaining their `GK1_PRE_SETUP_TEMPLATE` phase. No fabricated player/ruler/difficulty/stat/season initialization occurs. The first playable Spring values are not these templates (original Mongol source Temujin: gold 425, food 129, troops 60, reserve 32). Controller D-pad displays all entries, their original names, lord, gold, food, troops, undrafted reserve, morale, tax, economy, arms, skill, defense, specialty. Controller B returns to the campaign picker. Controller A inspects but explicitly refuses to advance an unproven first turn. Returning between campaigns preserves independent mutable state, and importing a tactical research receipt does not wipe the loaded country lists. Campaign save/load explicitly rejects unsupported operation; previous Divide/path/research replay save/load remains separate.

## Build and run

From archive root:

```sh
cmake -S execution_050 -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
ctest --test-dir build --output-on-failure
./build/gk1_native_shell
./build/gk1_native_shell --scenario-report 1
./build/gk1_native_shell --scenario-report 2
Xvfb :86 -screen 0 1280x720x24 &
DISPLAY=:86 ./build/gk1_native_shell --smoke-scenario 120
```

The CMake target stages the exact v1.07 CSV alongside the executable; the runtime resolves the data relative to its actual executable path on Linux and Windows, independent of the working directory. To intentionally supply another file for negative tests, set `GK1_SCENARIO_CSV=/full/path/to/file`; an invalid override MUST fail closed, never silently fall back to the bundled CSV. The manual 1280×720 native application uses controller-only navigation. Smoke scripts run software-generated controller actions, **not a physical Xbox device**.

## QA classifications and results

- **VERIFIED_STATIC**: original public dataset / exact SHA256 and previous recorded source research, no new NES gameplay.
- **TESTED_PORT_LINUX**: strict GCC C11 Debug, Release, ASan+UBSan CTest **10/10 each**; integrated controller input and scenario switching, pre-setup stat propagation, an isolated in-memory country mutation, original source still unchanged, invalid dataset no-mutation, preserved Divide/path/replay regression. Exact source identity test includes empty and `abc` SHA256 vectors and a semantically plausible Gold 425→426 mutation.
- **TESTED_PORT_LINUX**: actual native 1280×720 Linux/X11 `--smoke-scenario 120` 120 frames; separate `--smoke-divide 30` and `--smoke-path 30` from the same compiled executable. Screenshot rendered from actual Xvfb process, not a storyboard.
- **TESTED_PORT_LINUX**: two native process `--scenario-report` reports contain 14/27 original source names/stats; incorrect source override exit 8, absent source exit 8, invalid selector exit 4, scenario smoke source mismatch exit 12. Independent actual Divide replay export/import PASS, attempting to overwrite existing file exit 6.
- **NOT RUN**: actual Windows build, physical Xbox hardware, original first-playable Spring state, authentic player/character/difficulty initialization, unrestricted strategic orders, economy/AI/battles, dynasty, campaign saves or installer. No new Mesen run.

The hash reader is portable strict C11 and is not a copy of the emulator, licensed ROM, sprites, soundtrack, or private checkpoint. Raw original ROM/checkpoints remain private and are absent here.

This is **one inherited game source tree** and one integrated native executable, not a separate scenario explorer or web viewer. The separate `execution_049/gk1_scenario_list.c` remains research/test evidence and is not compiled into the game target.
