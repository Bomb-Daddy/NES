# EXEC0140 — verified Mesen saved-state PRNG lineage compatibility

Continues **the full EXEC0139 native C11 game**. Independent memory-safe C11 code
checks whether two authenticated Mesen CPU-RAM `$84..$89` snapshots *could*
be related by 0..65,536 direct original NES 48-bit PRNG draws. The `Gk1App`
method is read-only, guarded by `GK53_SETUP_AWAIT_SOURCE`, and does not unlock
campaign, legal orders, trade, or original first Spring. A compatible count
**never establishes actual draw count** because interrupts and unrelated
writes may have modified the six bytes. A nonmatch means there is no *pure*
PRNG chain inside the bounded interval, NOT that either observation is fake.

This is explicitly **VERIFIED_STATIC**, not a new Mesen runtime observation.
Original stock archived Mesen states remain private; no ROM, SRAM, licensed
art/audio or raw savestate is included in public files. Native Linux QA tests
both parser integration and independent mathematical oracle; Win32/NSIS/physical
Xbox and full campaigns are NOT RUN.
