# Genghis Khan I — EXEC0136 native C11 research build

Baseline: exact complete EXEC0135 native game, preserving Classic vs Enhanced, controller-only 1280x720, both template campaigns, historical files, original-source-gated checkpoints and prior save paths. Version: **1.36.0-dev1**.

## Actual implementation

Fixed a source-authentic 6502/PPU research interpreter timing defect: the NTSC unrendered PPU frame-event scheduler previously enumerated each frame with a loop, making long valid time advances effectively nonterminating. The new `gk103_ppu_advance` computes exact VBlank start/end counts and latest crossed event in constant time using unsigned integer arithmetic. It rejects counter overflow atomically and retains the PPUSTATUS-cleared VBlank value when no new VBlank start occurs. NMI edge counts and pending flags remain conditional on the explicitly known NMI enable state.

This updates an existing linked native game library, not a standalone replacement. `test_gk136_ppu_longspan.c` compares against an independently coded bounded per-frame oracle, probes long horizons, integer extremes, read-to-clear and rejected no-mutation controls. The matching verified private original-stock NES ROM was also recovered for bounded original-source regression checks; its bytes, original gameplay records and licensed assets are **not** part of this source distribution.

## Evidence boundary

`VERIFIED_STATIC`: code, causal diagnosis and CPU-to-PPU mathematical tests. `TESTED_PORT_LINUX`: only if executable CTest/build and GUI/package QA logs are present. `OBSERVED_RUNTIME_STOCK` previous Mesen Spring 1175/1206 histories only; no new Mesen runtime gameplay here. `NOT RUN`: Win32 native, NSIS installer, physical Xbox device and full campaigns. **The actual original 3-order first Spring remains blocked on verified power-on and PPU source behavior; no arbitrary RNG or player-ready Spring was introduced.**
