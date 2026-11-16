# GK1 EXEC0329 — Original archived first-Spring seed: CPU source replay

2026-11-16. NEW executable C11 source + actual Gk1App guard, appended to
inherited source — NOT a new standalone game or fake playable Spring.

## Scope

* Authentic private ORIGINAL USA stock iNES ROM (262,160 bytes,
  SHA-256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3)
  is supplied by owner at runtime and never embedded in public files.
* Authenticates two distinct restored original Mesen saved-state *decompressed
  named-field streams*, their sha256, lengths, CPU PC EFC9 and CPU cycles.
  Mongol Spring 1175 archive: 40,712 bytes, 107624250 cycles, SaveRAM.
  World Spring 1206 archive: 40,711 bytes, 212275526 cycles, SaveRAM.
* Parses each original CPU internal RAM $84..$89 via inherited gk139 parser;
  executes original fixed-bank source C289 on the original ROM using inherited
  EXEC0327 C11 6502 source executor, and independently checks 48-bit math.
* Strict distinction: this only computes the NEXT draw AFTER each archived
  Spring checkpoint, NOT the random draws that produced original Spring.
  The fresh WorkRAM reset is explicitly NOT accepted as an archived source.
  No source data is hardcoded as universal Spring rates or gold.
* Native Gk1App read-only bridge rejects PRE_SETUP/Enhanced/mismatched worlds,
  unverified country raw bytes, and invalid checkpoint projections; never
  mutates the source world or grants player controller orders.
* Any error leaves the output receipt unchanged byte-for-byte.

## Exact source build and tests

Stage into the latest COMPLETE inherited game source (EXEC0328 or newer)
with `python3 stage329.py /path/to/inherited/native/app`. The stage code
preflights dependencies and refuses a conflicting source, partially-staged
app or missing required prior C11 source. It appends only the new 329 sources
and native CTest targets to the inherited CMake. CMake Windows line endings
are preserved; the public source itself uses LF.

`cmake -S /path/to/inherited/native/app -B build && cmake --build build --target test_gk329_public test_gk329_app_bridge && ctest --test-dir build -R '^GK1_329_' --output-on-failure`

`python3 test_stage329.py` (13/13 hostile staged/rollback tests).

The PRIVATE regression requires owner-authenticated ROM, original archived
Mesen *decompressed named fields* for both scenarios, and fresh-stock comparison
JSON as four CLI arguments in that order. No private file is inside this ZIP.
The `test_gk329_private.c` source is public, but private test data never is.

## Tested this execution

* TESTED_HOST_LINUX_C11: strict C11 component and actual inherited native
  `gk1_app_state` target build PASS, 2/2 registered new CTests PASS.
* TESTED_HOST_LINUX_C11_PRIVATE: 32/32 assertions against exact owner-private
  original ROM + original restored Mesen traces, plus a FRESH-origin negative;
  ASan+UBSan PASS. **No NEW Mesen run**.
* TESTED_HOST_LINUX_PYTHON: staging 13/13 PASS.
* Full inherited app CTest suite: many old tests ran, but completed-suite
  acceptance is **not claimed**; an X11 subprocess test exceeded test budget.
* TESTED_PORT_WIN32 for EXEC0329: NOT RUN. Prior CI240 is separate predecessor.
* Physical Xbox, native Windows user campaign, complete campaigns: NOT RUN.

## Preserve source and acceptance boundaries

Original ROM, stock country RAM, named field bytes, Mesen savestates/screenshots,
licensed assets and user saves remain only in PRIVATE Genghis research Drive.
The public source does not include original gameplay dumps. The original-
stock first playable Spring still needs genuine causal ruler/ability/difficulty
RNG, spring finance/events for all countries, and real controller order entry.
No fake source events, Win32/Xbox testing, first Spring generator or release
acceptance may be inferred from this bounded partial progress.
