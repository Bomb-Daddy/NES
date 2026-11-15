# GK1 EXEC0297 — original USA reset initializer source implementation (not Spring unlock)

**Date:** November 15, 2026. **Predecessor:** EXEC0296, the complete inherited native C11 game and its new genuine original-stock Mesen four-writer observation. **Status:** partial implementation; the original NES first Spring is not playable in the native port.

## What actually changed

The exact original USA ROM (262,160 bytes; SHA-256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3) is authenticated from a PRIVATE user-supplied path. Fixed PRG bank 15 at $C0B8–$C0C7 authenticates the original LDA-immediate and STA-absolute reset initializer for $84,$85,$86,$87: **04,D2,04,D2 hexadecimal**, respectively. The caller's observed $88,$89 RAM tail is marked external, not claimed source-written.

**These are reset initializer values, NOT a universal pre-first-C289 RNG seed.** EXEC0296 independently witnessed 948 seed writes and 289 C289 calls; there are intervening transformations. The output explicitly reports `first_c289_seed_proven=0` and `gameplay_spring_unlocked=0`.

The C11 production function `gk297_reset_initializer_from_source` and its `gk1_app_original_reset_initializer` bridge were added to the *complete inherited* `execution_199` application. The bridge checks pristine Classic templates for BOTH 14-country Mongol and 27-country World setups, selected difficulty 1–5 and source-awaiting stage; it refuses Enhanced or corrupted world state atomically. The native game state, turn, saves and orders remain unchanged. Source-static only; no fabricated gameplay.

## Source files

- [gk297_stock_reset_seed.c](./gk297_stock_reset_seed.c)
- [gk297_stock_reset_seed.h](./gk297_stock_reset_seed.h)
- [test_gk297_stock_reset_seed.c](./test_gk297_stock_reset_seed.c)

The complete source ZIP and a 6-file exact integration patch are in the EXEC0297 reproducible handoff (not publicly redistributing a private ROM). Apply the exact patch to the *full* EXEC0296 inherited C11 source, not to an older or isolated app.

## Verification executed

Exact EXEC0296 source fetched from correct Collaboration Drive and clean-patch replayed into a second ZIP extraction with zero fuzz; all 6 modified/new file SHA-256s match. GCC C11 Debug compile and targeted CTest **1/1 PASS**. Private original ROM test: **89 assertions PASS** across both scenarios and five difficulties. Inherited EXEC0296 original-ROM test: **88 assertions PASS**. Native Linux X11 1280×720 graphical shell smoke: **60 frames PASS**. Full unfiltered Linux suite, EXEC0297 Windows and physical Xbox/full campaigns were **NOT RUN**. Historical EXEC0295 Windows CI218 is NOT evidence EXEC0297 ran under Windows.

## Publication and privacy

No ROM, raw NES RAM, screenshots, save states, sounds or licensed assets are committed to this public folder. This GitHub publication is not implementer read acknowledgment, stock runtime capture or a completed release.

**Outstanding critical path:** original stock CPU seed-write chronology/source-generated first playable Spring in the native C11 game (all 14/27 countries, ruler/difficulty/ability, 3 controller orders). Then controller Trade scheduling, strategic AI, battle aftermath, diplomacy, dynasty and complete campaigns.


## Full selected Linux regression after complete build

The full inherited native project was compiled with `cmake --build build297 -j16` (exit 0), then the applicable suite ran as `ctest --test-dir build297 -j16 --output-on-failure -E '([Ww]in32|[Ww]indows|[Xx]11)'`: **227/227 PASS, exit 0**. This supersedes the earlier test-target-only result, but does not establish Win32 or physical Xbox validation. An earlier run attempted before all test executables were built correctly reported Not Run failures; the 227/227 passing retest followed the complete build. The full unfiltered cross-platform suite was not run.
