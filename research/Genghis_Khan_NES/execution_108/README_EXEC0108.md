# EXEC0108 — NMOS 2A03 RMW old/new bus write correctness

This continues the COMPLETE inherited EXEC0107 native game and NES-source
tracer, version 1.08.0-dev1. The existing 14/27 country data, profile
separation, stock checkpoint Pass and all controller screens are preserved.

**New executed code:** `gk108_cpu_rmw_bus.c/h` atomically refuses invalid
callbacks/cycles and emits the NMOS 6502 OLD then NEW bus writes on adjacent
cycles. `gk83_reset_trace.c` now routes ROR abs,X; ROL zp; INC/DEC zp;
INC/DEC abs through the same bus-writer, rather than ignoring first writes
for non-mapper addresses. MMC1's established consecutive-cycle filter remains
in effect; the existing mapper RMW counter is maintained for 0x8000+.
`Gk83Trace` now reports both bus write counts.

`test_gk108_cpu_rmw_bus.c` tests 65,536 old/new value combinations,
consecutive cycles, both writes to the same address, rollover-limit clock
and atomic invalid-input refusal. These are synthetic CPU-level tests,
not an original Mesen runtime or new original-stock gameplay capture.

**Still OPEN:** original character/difficulty setup and independently
generated first Spring RNG; controller live merchant offers and Trade;
AI, dynasty, diplomacy, battle terminal, complete campaigns and native
Win32/NSIS/physical Xbox QA. All private original-ROM evidence stays private.
