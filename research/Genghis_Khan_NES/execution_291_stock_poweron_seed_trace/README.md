# GK1 EXEC0291 — private original-stock power-on seed writer + C289 capture

This is a **source/tools implementation** for the full inherited C11 game's open first-Spring blocker. It does **not** replace EXEC0290 or the existing native app, which retains both 14-country Mongol and 27-country World campaigns, saves, graphics, controller and profiles.

## Real capability added

`GK1_EXEC0291_Private_Mesen_PowerOn_SeedChronology.lua` merges previously independent EXEC0218 seed-write and EXEC0282 C289 observability into **one chronological original-stock CPU-source record**. It intentionally skips internal C289 seed stores (one C event represents them), records source PC + mapped physical PRG bank and CPU cycle for externally completed six seed writes, records entry/exit CPU cycles, actual six-byte seed before/after and result for each C289 call, and captures all 14 controller stages using exactly the already-observed original reset sequence. It writes **only** to a required private path, not public GitHub.

`gk291_verify_stock_trace.py` fail-closes malformed input, missing/unverified seed writes, chronology overlaps, bank failures, C289 arithmetic disagreement, unreported changes and wrong memory-bank designation. It independently hashes the **owner-supplied local original ROM** when `--rom` is passed. Synthetic test data must never be presented as an authentic Mesen execution.

## How to use with the matched Mesen headless runtime

1. Verify private input ROM is exactly 262160 bytes and SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`.
2. Set `GK291_TRACE_PRIVATE_PATH` to a location inside **private** Genghis research; enable Mesen Lua I/O and run the exact USA stock ROM with the Lua probe **from power-on**.
3. Once it finishes, run `python gk291_verify_stock_trace.py <private-trace> --rom <private-stock-rom>`. An arithmetic PASS proves only internal trace consistency, not that a submitted trace is authentic or that the native game generated Spring.
4. Independently check CPU writer PC/bank/instruction provenance, title input timings and fresh vs archived history. Do not mix archived `nesSaveRam` with fresh `nesWorkRam` physical `0x0DCD`.

The validator requires first six explicitly recorded seed-byte writes and all subsequent interleaved changes to match C289 calculations. Previous source snapshots cannot fill an unexplained seed gap. Every failure is read-only and does not mutate the native game. The tools do **not** embed a ROM, licensed graphics or raw gameplay.

## Acceptance

`VERIFIED_STATIC`: positive synthetic trace, 23 hostile negative controls, ROM SHA rejection, and Python compile PASS. `OBSERVED_RUNTIME_STOCK`: no new Mesen execution in EXEC0291; previous fresh/archived evidence remains separate. `TESTED_PORT_LINUX`: Python tests run on Linux, not new native gameplay; inherited EXEC0290 233 eligible CTests retained. `TESTED_PORT_WIN32_CI`: EXEC0290 untested; EXEC0289 217/217 inherited. `NOT RUN`: Lua under matched Mesen, hardware Xbox, full campaign.

**Next action:** run this private capture in real original-stock Mesen; parse source writer PCs/instruction bytes; integrate authenticated start-seed + controller/NMI timing into native C11 setup, producing actual source-generated Spring/three orders without hardcoding Gold430/439 or Rate121/101. Then merchant RNG, AI, battle aftermath, diplomacy, dynasty and full campaigns.
