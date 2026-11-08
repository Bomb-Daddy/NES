# Genghis Khan I — EXEC0094 cartridge SRAM bank correction (C11 native app)

**Native game 0.94.0-dev1 — INCOMPLETE.** Built from the complete, current
EXEC0093 game source (all earlier modules, assets, research saves, controllers,
profiles, campaigns and previous test coverage retained). This is a source
fidelity correction, NOT authentic first-Spring generation or complete game.

An overlooked failure in the reset interpreter represented all CPU addresses
$6000–$7FFF as the same generic unmodeled *hardware* read, despite that being
8 KiB of original NES cartridge PRG save RAM. `gk94_prgram.c/h` now treats
this as a separate 8192-byte knownness-tracked RAM bank, physically indexed
CPU-$6000. Source execution can read values it previously wrote there; an
uninitialized battery-backed read explicitly stops at UNKNOWN_RAM_GATE rather
than inventing zeros or misclassifying the event as a PPU/APU register read.
Fresh Mesen all-zero `nesWorkRam` **does not establish** `nesSaveRam` values.

A separately supplied, independently hash/bank-verified private SRAM buffer may
be seeded using physical offsets; seeding is never performed automatically in
the normal game, and this public source embeds no ROM or SRAM. At original
country record base physical SaveRam `0x0DCD`, the corresponding CPU address is
`0x6DCD`, **not** `0x0DCD` (the work RAM mirror). This distinction is explicitly
unit-tested with positive, unknown, invalid and rollback controls. This module
does not yet model MMC1 PRG-RAM disable bit, true battery persistence, source
setup RNG, or PPU/APU cycle timing; PRG-RAM-enabled state is an interpreter
assumption, not a proven original stock runtime conclusion.

Build from project workspace with sibling `execution_034`, `_039`, `_045`,
`_047`, `_049` retained, for example:

```
cmake -S execution_094 -B build94 -DCMAKE_BUILD_TYPE=Release
cmake --build build94 -j4
ctest --test-dir build94 --output-on-failure
```

Evidence: VERIFIED_STATIC = RAM mapping and authenticated original cartridge
signature; OBSERVED_RUNTIME_STOCK = earlier private fresh/archived evidence only;
TESTED_PORT_LINUX = C11 binary and tests actually run for this increment;
NOT RUN = native Windows, Windows installer, physical Xbox, new original Mesen
emulation, source-authentic setup/character RNG or a complete campaign.

**NEXT:** trace the genuine original stock first Spring/setup and RNG against
Mesen fresh and archived origins and implement the actual full 3-order season,
not a fixed Spring screenshot, then merchant presence/Rate RNG/AI/end battles.
