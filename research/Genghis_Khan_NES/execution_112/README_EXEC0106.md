# EXEC0106 — Source-driven PPU IO register latch, Linux research build

Complete inherited EXEC0105 native C11 game preserved, now version 1.06.0-dev1.
`gk106_ppu_bus.c/h` implements the NES PPU I/O bus latch as a small bounded,
tested native C11 module. A CPU write to any register in $2000-$3FFF, including
mirrors and even an otherwise ignored write, drives the bus latch. Subsequent
PPUSTATUS reads use known CPU-written lower 5 bits; upper sprite bits require
an elapsed unrendered pre-render clearing interval. Power-on unknown bus and
sprite state are explicitly hardware-gated. Status reads refresh only upper
three bits of the bus, with lower five retained. This is implemented in the
existing bounded original-ROM 6502 source tracer via
`gk106_trace_original_ppu_latch()`, and leaves all earlier research modes intact.

The original NES reset-to-first-Spring setup, RNG, all original country seasonal
transformations, full playable campaigns, and twelve locked strategic commands
are STILL INCOMPLETE. This source-tracing hypothesis is not original Mesen
runtime observation, not a ROM parity result, and not a source-authentic game
world seed. It does not include the ROM, private game states, copyrighted
assets or Mesen snapshots. Original ROM may be supplied locally through
`GK1_ORIGINAL_STOCK_ROM` for source-only testing after verifying its SHA-256.

Build: `cmake -S execution_106 -B build106 -DCMAKE_BUILD_TYPE=Release && cmake --build build106 -j2 && ctest --test-dir build106 --output-on-failure`.
Windows, NSIS and physical Xbox controller have not been tested in this run.
