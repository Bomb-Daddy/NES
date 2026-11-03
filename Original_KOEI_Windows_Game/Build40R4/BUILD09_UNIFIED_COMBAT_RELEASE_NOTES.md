# KOEI two-mode Build09 — unified ROTK II battle-rule integration (SOURCE)

Date: 2026-11-01. Author: Zhuge Liang. Provenance: hash-verified Build08 12-part full source SHA-256 `00394bdc8e15b34902c99c2bd5cf9111d07752db2604f8248d8c7a3e02a99ed0`, reconstructed without loss and patched. Source authority: Director's November 1 SNES/NES single-battle-mechanics decision and `ROTK2_Battle_Answer_Matrix_v1_4` parts 1 and 2.

## Runtime integration

- **Classic** in this two-mode launcher invokes the original English NES ROM in Mesen/Mesen2 or a configured Classic executable. Its actual combat is the original NES code, not a synthetic copy. This source update does not alter, embed, or distribute the game ROM. Separately developed Python-only ClassicNES Windows recreation remains NOT FULLY INTEGRATED.
- **Enhanced** `third_branch/integrated.py` invokes a NEW live module `third_branch/rotk2_battle_core.py` for the actual player/AI tactical commands and persistent army ledger, rather than only a document or test stand-in. Uses approved 13×12 odd-column-shifted hex neighbors, terrain movement costs, mobility based on training, simultaneous per-exchange troop losses with terrain/difficulty, 30-day defender-favoring timeout, and per-side rice in hundredths. Support for player Normal, Charge, Simultaneous and Fire actions, rain suppression, directional fire spread, commander rice damage, attack-then-move timing and charge break-through. Controller and mouse tactical UI exposes all four actions.
- `third_branch/tactical_view.py` exposes each attack mode and shows live burning units; uses existing 1280×720 tactical interface. Old snapshot tests with fixed 20-day and one-soldier battle assumptions revised to validate the recovered 30-day rule and conservation of actual losses. `tests/test_rotk2_battle_core_v14.py` adds true code-path regression tests.

## Tests performed in Linux Python (headless)

`python -m pytest -q tests/test_rotk2_battle_core_v14.py tests/test_p0_multistation_capture_fix_v3.py tests/test_integrated_branch.py tests/test_tactical_view.py tests/test_battle_art_v14.py tests/test_41_game.py tests/test_p0_v119_castle_and_defensive_occupation.py tests/test_build08_release_integration.py tests/test_director_two_mode_launcher_build08.py tests/test_xbox_controller_v14.py tests/test_army_cohorts_v18.py --disable-warnings`

Result: **128 passed (5.68s), exit 0**, local Python/headless only. Dedicated canonical NES decoder synthetic negatives: **5 passed**, exit 0, `PYTHONPATH` pointing to verified staged code. Both tests are independent of Windows executable launch, player acceptance, and exact Mesen battle traces. Broad unfiltered suite was NOT completed: it entered older long-running simulation cases after 77%; do not call it full PASS. Previous 327-pass/1-fail attempt was resolved by changing legacy one-soldier expectations to exact ledger loss accounting; 128 tests reran green thereafter. No long-lived simulation or 100×120-month stress run authorized/performed for this patch.

## Important limitations (NOT claimed parity-complete)

- Enhanced currently represents each army by **one aggregate counter** rather than the 4 original battle slots and individual officer-unit lists. It therefore cannot yet reproduce original per-officer turns, deployments, commander loss, spontaneous switching, duel negotiation, bribery, reinforcement and capture/Release/Recruit/Behead decisions, or six-round abstract NPC-vs-NPC combat. Its two-unit `Simult.` action can resolve only one adjacent opponent. Original map regions/deployment zones are not yet available in Enhanced's `battle_assets` module; the 41 maps contain terrain but source-derived entrant locations are not yet wired. Current 41-province visual map metadata has two explicitly inferred P41 cells.
- Troop-ratio default 100 is used for current aggregate stacks, not decoded per-officer second troop record. Battle gold is not an in-field pool; the original Q08 30% commander *gold* penalty remains pending (rice implemented). Side-0's extra 60 soldiers in source RAM cannot be accounted for without actual officer lists. Weather/Wind are simulation state, not original NES hardware RNG.
- Classic NES Python rewrite's `engine.py` remains provisional; this two-mode launcher invokes the original NES ROM instead. A separate `nes_terrain.py` offset correction was written to the canonical NES repository independently (original same Drive file ID).
- Legacy Enhanced campaign battle-end capture/retreat policies are safety adaptations, not source-proven original's prisoner menus. No Windows `EXE` has been built or executed in this environment. Source build and test success are not proof of install/packaging/controller hardware.

## Build/use

The full Build09 ZIP is a complete replacement for the original Build08 source ZIP. On Windows, use `BUILD_WINDOWS_EXE.bat` from the source tree with the documented local compiler/dependencies and original English NES ROM configured in the launcher. On Linux, run `BUILD_LINUX_EXE.sh` when PyInstaller and system dependencies are present. The runner currently available to Zhuge Liang lacks PyInstaller, Windows and an interactive display; those executable steps were **NOT RUN** here. The small `KOEI_Build09_Combat_Patch.zip` overlays the corresponding files on original Build08 SHA-256 source and is not a self-contained standalone executable.

## Completion register

- `NES-CMB-01` Source/hash baseline: PASS VERIFIED (Drive bundle SHA)
- `NES-CMB-02` Classic original ROM launcher: static source verified; runtime NOT RUN this turn; separate NES Python rewrite PARTIAL.
- `OG-CMB-01` Enhanced common combat module and tactical command dispatch: TESTED_PORT (128 headless targeted tests), PARTIAL original parity.
- `OG-CMB-02` Full officer/slot/capture/deployment parity: NOT RUN/NOT IMPLEMENTED.
- `OG-CMB-03` Windows installer, physical Xbox, interactive graphics and independent Mesen parity: NOT RUN.
- `NES-CMB-03` NES terrain adjacency 0x28014: TESTED_PORT synthetic 5 tests, readback verification required for source publication.

This is an executable source-level integration increment, not a release-acceptance claim.
