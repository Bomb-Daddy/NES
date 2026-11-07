# GK1 EXEC0052 — Profile-gated country candidate setup and cold-save integration

This is an additive implementation in the **existing standalone C11 Genghis Khan I native game** (EXEC0051 is the exact parent). It is not ROTK II, another game, a NES emulator, a finished Windows product, or an original first playable Spring.

## Newly implemented

- The controller-only campaign menu LEFT/RIGHT explicitly selects **Classic stock** or **Enhanced approved modified** behavior. The original 14/27 source-verified pre-setup country rosters remain available under either profile; the three research battle menus are **unavailable** under Classic because their fixtures only prove Enhanced behavior.
- On a country screen A marks or unmarks the current original country/lord as a **candidate** for prospective player setup; selections are independently maintained for Mongol Conquest (14) and World Conquest (27). This is an input *intent*, not an assertion that every country is selectable in the original game. It does not apply difficulty, automatic statistics, character setup, Spring processes, or any simulation mutation.
- A new 2618-byte exclusive-create PRE_SETUP persistence format layers a 24-byte validated `GK1DRF52` extension over the exact previous 2594-byte `GK1PRE51` payload. The old payload remains binary-identical, including its original source identity and checksum. The extension records rule profile, both candidate indexes, strict reserved zero fields, and a second CRC across the entire new file.
- The real native Linux/Windows-source application X/Y route uses the new format. Both new saves and previous 2594-byte saves load. Legacy loads deterministically select Classic stock and unset both candidates because the old format did not encode them. Corrupt/incorrect/truncated/extending data rejects with **no mutation**. Saving never overwrites an existing path.
- Actual native controller software events, separate-process export/import, and real X11 graphical execution are covered. Every earlier battle/Divide/roster/research save test remains in CTest.

## Source evidence boundaries

The original numeric data is the existing v1.07 CSV (exact SHA256 `fa3958f156f09d4b4c15bb559c12aa357fc617a9b1f9217505fe07e8800de581`). PRE_SETUP_TEMPLATE is not game start. Classic vs Enhanced profile is only a selector and prohibition of using Enhanced-only research cases under Classic, not a claim of Classic battle implementation. No original emulator gameplay occurred in this increment. No Windows or physical Xbox test occurred. The game remains incomplete.

## Build, test, manual CLI

From repository root or an unpacked source archive:

```sh
cmake -S execution_052 -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j 4
ctest --test-dir build-debug --output-on-failure
build-debug/gk1_native_shell --setup-draft-export /tmp/gk52_slot.gkp 2 5 classic
build-debug/gk1_native_shell --setup-draft-import /tmp/gk52_slot.gkp
xvfb-run -a build-debug/gk1_native_shell --smoke-scenario 60
```

CLI `--setup-draft-export PATH SCENARIO(1|2) COUNTRY_ID(1..14|27) classic|enhanced` executes the app's actual Gk1Input commands and saves a candidate; it is not an NES input trace. `--setup-draft-import PATH` runs a new process, validates source identity and both checksums, and reports persisted intent. Original CLI `--pre-setup-export`/`--pre-setup-import` remain supported, now saving/reading v2 and reading v1 as well.

## Run evidence

- Strict C11 Debug / Release / AddressSanitizer+UBSan CTest **12/12 each**, including all 11 inherited tests and the new `GK1_052_ProfileSeparatedCandidateInputAndColdLoad` (which exercises all 2618 possible one-bit corruptions and atomic rollback).
- Native two-process `--setup-draft-export ... 2 5 classic` and `--setup-draft-import` PASS, 2618 bytes, candidate World #5, Classic; attempted overwrite exit 6 and corrupted-file import exit 7; synthetic controller X/Y save/import test PASS.
- X11 original World scenario 60 rendered frames at 1280x720; Divide and path prior scene smoke 18 frames each PASS. These are **TESTED_PORT_LINUX**, not installed Windows or physical Xbox acceptance.
- A new full source archive includes earlier unchanged execution history for reproducibility; no licensed game ROMs, Mesen save state or proprietary image/audio files.

## Open gates

Original valid selectable rulers, difficulty labels, automatic/manual stats, Spring entry and initial AI state remain unproven; recorded candidates must not be consumed as actual NES setup. Strategic orders, campaign turns, combat generalization, AI/dynasty, authentic graphics and audio, complete campaign save, Windows installer and actual Xbox QA are outstanding.
