# GK1 EXEC0107 — Implementer + hostile QA handoff v1.00

## Authoritative base
Continues the exact complete inherited EXEC0106 native C11 application as a new
`execution_107/` source tree (version 1.07.0-dev1). Existing execution_106
and all historical source archives are preserved, Classic original-stock vs
Enhanced Shared-BP/Rest profiles remain distinct. No full campaign completion.

## What was actually implemented
New linked `gk107_ppu_ports.c/h` and `test_gk107_ppu_ports.c` integrate an
explicit PPU I/O write-only-register read path into the existing original ROM
6502 source interpreter (`gk83_reset_trace.c/h`). Read-only PPUSTATUS $2002,
OAMDATA $2004, PPUDATA $2007 are intentionally excluded. Nominal write-only
registers $2000/$2001/$2003/$2005/$2006 and mirrors through $3FFF return a
known source-written PPU I/O bus value only within an upper-bounded freshness
window of 3000 modeled 2A03 CPU cycles. Unknown, stale, reversed-time reads
refuse atomically. New counters record when source executions actually use
that path or refuse stale values.

NESdev references: https://www.nesdev.org/wiki/Open_bus_behavior ;
https://www.nesdev.org/wiki/PPU_programmer_reference . Those confirm PPU port
semantics only, not Genghis-specific original Mesen parity. Conservatively
refusing older values is an intentionally incomplete analog-decay model.

## Actual results / distinction of evidence
VERIFIED_STATIC: exact privately accessed original USA stock ROM 262160 bytes,
SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3.
SOURCE_HYPOTHESIS: exact-ROM bounded local tracer deterministic replay at 3
hypothetical PPU origins 0,10240,80000 produced PC $C5BE/$C5E9/$C5E9.
New write-only port event count was **zero** for all three cases; this addition
is executable NES hardware correctness and testing, NOT a claimed solution to
Genghis's first-Spring CPU startup block. It did not generate original RNG.
OBSERVED_RUNTIME_STOCK: previously recorded authenticated fresh/archived original
Spring 1175 and World Spring 1206 Mesen records stay authoritative, distinct,
not rerun in this task. MODIFIED_NES observations not conflated.
TESTED_PORT_LINUX: Release 66/66, Debug 66/66, targeted original-ROM sanitizer
2/2, reduced sanitizer suite 60/60 (excluded inherited expensive/time-limited
GK1_054/055/058/084/105); first sanitizer-all run did not complete and shall
not be reported as 66/66 sanitizer. Linux X11 actual 1280x720 120 frames PASS,
Debian package installed-style executable and real X11 frame run PASS; correct
1.07.0-dev1 printed in title/CLI/menu; desktop entry version updated.
NOT RUN: Windows Win32/XInput executable, NSIS installer, physical Xbox,
original Mesen fresh reset in this increment or complete gameplay campaigns.

## Hard priorities
1. Ground next original CPU hardware gate in Mesen-source CPU-cycle traces and
   interpret actual original stock input/PPU timing rather than forcing zero bus.
2. Source-authentic player/ruler/difficulty/ability selection and first Spring
   14/27 country seasonal transforms from RNG and character setup. Existing
   exact authenticated checkpoints are test oracles, not hardcoded initial state.
3. Only then unlock working controller Trade with merchant RNG and real Gold,
   orders/turn, and persistent save, plus all other commands, true AI,
   battle terminal, diplomacy, dynasty, artwork, sound and Windows installer.

## Release status
NOT COMPLETE. No private ROM/SRAM/Mesen originals in public source or QA.
New source and binaries are local reproducible outputs until remote readback
succeeds; document publication separately from actual tested code.
