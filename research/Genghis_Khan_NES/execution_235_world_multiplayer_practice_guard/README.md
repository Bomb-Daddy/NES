# Genghis Khan I — EXEC0235 — World multi-human Spring state integrity

**Date:** 2026-11-12. **Predecessor:** EXEC0234 v3.88.1, Windows CI accepted (run `43326322387`). This is a narrow correction inside the **same** inherited C11 game, not a new standalone prototype.

## Identified executable defect

The genuine controller setup flow correctly stores 1–4 World human lords, but the Enhanced finance-practice path `gk1_app_enhanced_spring_lab_open` contains only ONE `Gk222Spring` player and ONE three-action budget. Prior to this correction, pressing A after completing a 2–4-human World setup silently launched a *single-ruler* practice, implicitly dropping turns for the other nominated people. A four-human World roster alone therefore did not demonstrate functioning four-player play.

## Integrated correction

`execution_199/gk1_app_state.c`: Before creating or mutating practice state, reject any setup with `setup_player_count != 1`. In the live controller setup wizard, the status line now clearly says `WORLD MULTI-HUMAN SPRING NOT IMPLEMENTED: ALL RULERS PRESERVED` when pressing A on a multi-human Enhanced setup. Original World and Mongol templates, all human-lord selections, source status, season and order budgets remain intact. Single-human Enhanced World practice remains available with its established three-order budget. Classic original profile never enters this Enhanced lab, unchanged.

`test_gk235_world_multi_guard.c`: Compiled actual game state and pad-router flows, World 1/2/3/4-player setup, one-human permitted, multi-human rejected with zero world/roster/country/order mutation, repeated A refusal. The same game's `CMakeLists.txt` registers an additional CTest. `apply_multiplayer_guard.py` fails closed on stale, edited, duplicated or already patched source. `test_multiplayer_overlay.py` repeats eight positive and hostile source-replay cases.

## Reproduction

From a **copy** of the complete `GK1_EXEC0234_Complete_Inherited_Source_World4_QA.zip`, rehydrate the enclosed EXEC0230 complete source, sequentially apply accepted 0231, 0232, 0233 and 0234 overlays per existing reconstruction README, then apply this **new** overlay:

```sh
python3 apply_multiplayer_guard.py PATH_TO_FULL_HYDRATED_ROOT
cmake -S PATH_TO_FULL_HYDRATED_ROOT/execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8
ctest --test-dir build --output-on-failure -j 6
python3 test_multiplayer_overlay.py PATH_TO_FULL_HYDRATED_ROOT
```

Public source ZIP includes *unchanged* exact predecessor archive and all five new source/test/patch/QA components. `GK1_EXEC0235_Exact_Source_Change.patch` describes actual modifications relative to the applied 0234 source. Do not apply it again after the Python overlay.

## Results and fidelity labels

- **VERIFIED_STATIC:** exact 0234 predecessor source inspected; bounded patch, eight hostile replay controls pass.
- **TESTED_PORT_LINUX:** Release full native build and **202/202 CTests passed**, Debug full native build **202/202 CTests passed**, targeted new and inherited 234 tests **2/2 ASan/UBSan passed**.
- **OBSERVED_RUNTIME_STOCK — fresh:** historical original NES WorkRAM Spring 1175 remains owner-private evidence; **no new original Mesen execution**.
- **OBSERVED_RUNTIME_STOCK — archived:** historical original NES SaveRAM Spring 1175 and World 1206 remain separate; **no new original Mesen execution**.
- **TESTED_PORT_WIN32:** prior **EXEC0234** Windows MSVC/NSIS CI was completed. **EXEC0235 native Win32 and physical Xbox not yet run** unless a later CI run is explicitly linked and independently verified.
- **NOT RUN:** original first Spring generation from original setup/RNG, full World multi-human turns, physical Xbox, full campaigns.

## Unchanged high-priority requirements

User-recovered original USA NES ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` is **not missing** and is never included in public source. Authentic original first Spring ruler/difficulty/character RNG for all 14/27 country conditions, real 3 stock orders, full Trade merchant scheduling, AI, battle aftermath, diplomacy, dynasty, graphics/audio and campaign save remain release blockers. Do not promote this safety correction to a completed multi-human campaign.

Publication is evidence of source availability, **not** another agent's read/execution acknowledgment. Continue the existing hourly work until Director stop or certified complete release.
