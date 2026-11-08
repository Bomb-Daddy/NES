# GK1 standalone native execution_060 — Enhanced Demobilize integration

2026-11-07 · Source scope: **existing** Genghis Khan I English NES-to-Windows C11 native project. This is not ROTK II or the third original strategy game.

## New tested code, not a new game

- Integrated `gk60_demobilize.c/.h` into the existing native app and the existing 14/27-country mutable world state.
- The source-verified *approved modified-ROM* Demobilize arithmetic is `0 < q <= min(Troops, 999 - undrafted_reserve)`. An accepted transaction subtracts `q` from troops and drafted population and adds `q` to undrafted reserve. Total population and all unrelated stocks stay unchanged. The already-recorded eligibility percentage recomputes `floor(reserve*percent/100)` to preserve the existing exact 45-column dataset and save format invariants. All invalid, zero, capacity-overflow and inconsistent-record attempts are no-ops.
- Controller route: Title -> Campaigns -> Mongol/World -> selected country -> View workforce planner -> Start Enhanced Demobilize preview; Up/Down +/-1, Right/Left +/-10, A apply, B or Start return. Classic stock profile explicitly refuses entry to this modified-only feature. It does not automatically assign freed manpower, advance game time or consume hypothetical Body Points.
- X and Y retain existing three-slot pre-setup state persistence and original checksum/source identity validation, without changing any older format. Active world and all 41 country records retain independence.
- The UI explicitly identifies this as `GK1_PRE_SETUP_TEMPLATE`, **not** a playable NES Spring, not original-stock Demobilize and not a claim about original order costs. The numerical mechanics have an already-accepted modified-ROM oracle; this increment did not re-run settled emulator cases.

## Reproducible Linux QA commands

From the complete source bundle where `execution_060`, `execution_049` and other inherited execution folders are siblings:

```sh
cmake -S execution_060 -B build60 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build60 -j4
ctest --test-dir build60 --output-on-failure
xvfb-run -a build60/gk1_native_shell --smoke-demob 80
GK1_SCENARIO_CSV=execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv build60/gk1_native_shell --demob-preview-export new.gkp 1 1 10
GK1_SCENARIO_CSV=execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv build60/gk1_native_shell --demob-preview-import new.gkp
```

The CLI export is an explicit QA aid, not a keyboard gameplay shortcut. It writes a new exclusive-create **pre-setup** file only. The GUI remains controller-only. A second export to the same path must fail (exit 6); corrupted import must fail (exit 7); unsupported arguments fail (exit 4). All 41 original source data rows retain their exact Git blob and SHA256 identities. The `gk1_native_shell` main application was extended directly, with inherited tactical movement, Divide, research replay, setup wizard, workforce planner, 3-slot save and Linux/Windows packaging scaffolding preserved.

The current status is **TESTED_PORT_LINUX**, source-backed bounded Enhanced modifier only. The original NES Classic campaign, authentic first Spring, seasonal AI/orders, full strategic and combat mechanics, native Windows compile/runtime/NSIS installer and actual physical Xbox controller remain **NOT FINISHED/NOT TESTED**. No copyrighted ROM, SRAM, screenshots or audio are in this public source bundle. Existing user campaigns and prior files are untouched.
