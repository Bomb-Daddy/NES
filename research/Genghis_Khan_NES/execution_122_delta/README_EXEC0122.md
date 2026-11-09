# Genghis Khan I native C11 EXEC0122 — NMOS 2A03 logic addressing

This is a research increment to the **existing** native 1280x720 C11 Linux X11 / Windows Win32 intended application, not a separate demo and NOT a playable complete game.

Inherited source: complete EXEC0121 `execution_121/` and historical files; preserve both Classic and Enhanced profiles, original 14/27-country scenario source templates, controller menus, historical source evidence gates, research slot saves and user save directory rules.

## New exact CPU semantics (VERIFIED_STATIC)

`gk122_cpu_logic.c/h`, wired into `gk83_reset_trace.c`: all 24 official ORA, AND, EOR instruction forms (immediate; zero page; zero page X; absolute; absolute X/Y; indexed indirect X; indirect indexed Y). Absolute and indirect indexed reads use source-gated memory, with correct page-cross timing. Zero-page pointer wraps before high-byte fetch; 16-bit addresses wrap normally. N and Z flags change, other flags are preserved. `gk100_cpu_cycles.c` contains corresponding precise instruction cycles. No private ROM data was bundled, no unknown memory/PPU value is assumed. Old correct ORA, AND, EOR opcodes are now handled by the same implementation.

## Reproduction

```
cmake -S execution_122 -B build122 -DCMAKE_BUILD_TYPE=Release
cmake --build build122 -j8
ctest --test-dir build122 -j8 --output-on-failure
(cd build122 && cpack -G DEB)
xvfb-run -a build122/gk1_native_shell --smoke-frames 120
```

`test_gk122_cpu_logic.c` checks all 256 byte opcodes for exact inclusion/exclusion, 786432 actual/reference arithmetic+flag combinations, 65,536 zero-page X wraps, 24 exact cycle groups and branch-independent page penalties. Existing inherited tests remain present. Bounded original stock reset tracing still stops at source-unknown RAM in its unsupplied setting; this change does **not** establish original first playable Spring.

## Evidence separation

VERIFIED_STATIC: official 6502 documented opcode semantics and integrated C11 source. TESTED_PORT_LINUX: specific current-build compiler, CTest, Xvfb, package logs in QA evidence. OBSERVED_RUNTIME_STOCK: only previously captured fresh-reset and archived-restored original stock gameplay (different Gold/Rate). NOT RUN: new original Mesen gameplay, native Win32/NSIS, physical Xbox, full campaigns. Private original ROM, Mesen states, raw RAM/SRAM, licensed artwork and audio must stay private.

## Remaining priority

Source-authentic player/difficulty/automatic-stat RNG and seasonal initialization for all 14/27 countries, independent original first Spring with three live orders, actual original Trade merchant scheduling, AI, original commands, battle ends and aftermath, diplomacy/dynasty, cold-load campaigns, Windows/Xbox full acceptance. Do not fake a first Spring from an archived screenshot or change Classic with Enhanced rules.
