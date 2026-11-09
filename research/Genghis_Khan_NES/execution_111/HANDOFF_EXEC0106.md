# GK1 EXEC0106 implementation and QA handoff — 2026-11-08

## Authority and inheritance

Complete native C11 Genghis Khan I game inherited from EXEC0105, 1.05.0-dev1, GitHub commit `47571df0e2488359231a900cfd8ffcc98e78051a`; built as EXEC0106 1.06.0-dev1. All EXEC0105 files remain untouched in the historical source ZIP and the existing GitHub directory (no overwrite intended). This does not depend on the outdated EXEC0062 source anchor or repeat the manual audit.

## Added executable source

- `gk106_ppu_bus.c/h`: bounded NES PPU register open-bus latch with unknown reset, known CPU write updates, status low-five preservation, status high-three update, input rejection and rollback. No original copyrighted bytes embedded.
- `gk83_reset_trace.c/h`: optional source-derived lower-five bus mode wired into the **existing** original-ROM 6502 reset tracer, preserving older tracing APIs. The default path refuses initial unknown sprite flags before first pre-render clear. The four explicitly named alternative flags are `SOURCE_HYPOTHESIS`, not original Mesen observations.
- `test_gk106_ppu_bus.c`: bus data-flow, invalid source, repeatability, mutation guards, four source-hypothesis contrasts.
- `CMakeLists.txt` and `packaging/*`: native C11 build/CTest/1.06.0-dev1 Linux and Windows packaging names, desktop version, installed CLI checks. No browser shell.

## Tested findings and boundaries

Exact private original stock USA NES cartridge: 262,160 bytes; SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` verified; supplied to Linux source-tracing tests **only**, not included in ZIP, GitHub, installer or QA archive. At hypothetical PPU frame-origin dots 0, 10240 and 80000, default source-bounded bus test stops at `$C5BE` after 142 instructions and 516 completed CPU cycles, after one observed source-executed PPU write; it refuses unknown initial sprite flags. All four **hypothetical** zero/overflow/sprite-hit/both initial flags reach identical bounded control outcomes within each origin: origin 0 `$C5BE` after 16,323 instructions and 8,027 status reads; origin 10240 `$C5E9` after 15,355 instructions and 7,541 status reads; origin 80000 `$C5E9` after 8,711 instructions and 4,219 reads. The identical result across these four assumptions does NOT establish authentic NES timing or RNG. Neither fresh nor archived Mesen runtime was rerun in EXEC0106. Distinguish previously observed genuine stock Spring records as `OBSERVED_RUNTIME_STOCK` from these new `VERIFIED_STATIC` source-dependent hypotheses.

`TESTED_PORT_LINUX`: Release and ASan+UBSan suites 65/65 each with the authenticated ROM, 120-frame native 1280x720 X11 graphical smoke PASS, Debian installer built/extracted, installed-style executable version and graphical smoke PASS. A trade smoke without a private checkpoint explicitly refused, as designed; that is not a completed Trade transaction. Native Win32 binary, NSIS, physical Xbox controller, original Mesen first-Spring parity and full campaigns **NOT RUN**.

## Still blocking release

The native app **cannot independently generate the authentic first playable Spring**, original player ability/difficulty RNG, 14/27-country original Spring transformed state and original three-order player campaign. Twelve strategic commands remain locked, Trade is price reference only, autonomous AI, diplomacy, dynasty/marriage age, battle end/aftermath and both complete playable campaigns remain incomplete. Do not convert source hypotheses to original ROM observations, do not hardcode the three authentic Gold/Rate profiles, do not put the ROM/SRAM/art/audio in public GitHub. Continue from EXEC0106 unless a verified newer execution exists, and preserve the hourly run unless explicitly stopped or verified complete.
