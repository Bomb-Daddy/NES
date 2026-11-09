# Genghis Khan I — EXEC0124 (1.24.0-dev1)

This version retains the **complete inherited EXEC0123 native C11 application** and all historical research. It is still an incomplete native strategy game, **not an authenticated independently playable first Spring**.

Implemented source code:
- Added `gk124_cpu_compare.c/.h`, recognizing all 14 official NMOS 2A03 CMP/CPX/CPY opcode/addressing forms (8 newly routed into `gk83_reset_trace.c`; six supersede duplicated legacy cases).
- CMP reads through pre-existing `read_byte` known-memory and peripheral gates: zero-page indexed wrap, `(zp,X)` and `(zp),Y` high-byte pointer wrap, indexed page crossing, correct binary unsigned carry/zero/negative flags, untouched other P flags, and 2/3/4/5/6-cycle table.
- Added 16,777,216 independent complete-state arithmetic assertions, all 256 decode discriminators, cycle and cross-page edge tests, and missing/private ROM rejection and unchanged-output rollback.
- Rebuilt the **same** 1280x720 native Linux/Win32-intended shell and corrected visible version plus Linux package version, not a browser replacement.

Reproduce on Linux with the full historical ZIP extracted, including `execution_049`, `execution_045`, `execution_047`, `execution_039`, `execution_034`:
```
cmake -S execution_124 -B build124 -DCMAKE_BUILD_TYPE=Release
cmake --build build124 -j8
ctest --test-dir build124 -j8 --output-on-failure
(cd build124 && cpack -G DEB)
xvfb-run -a build124/gk1_native_shell --smoke-frames 120
```

The previously recovered *private* exact original USA NES ROM can be supplied only locally for research via `GK1_ORIGINAL_STOCK_ROM=/private/path`, with strict SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. Neither ROM nor its RAM/checkpoints/Mesen screenshots/audio is present in any public package.

Status: **VERIFIED_STATIC** source and executable comparison math; **TESTED_PORT_LINUX** compiler, tests and graphics; **OBSERVED_RUNTIME_STOCK** historical fresh vs archived 1175 / World1206 only, no new Mesen run; **NOT RUN** native Win32, NSIS and physical Xbox acceptance. Inherited reset gates still halt at 70 instructions on unknown `$00F5` RAM, or under the specifically archived MesenCE all-zero RAM hypothesis, after 142 instructions on `$2002` PPUSTATUS. Do not treat these as successful first Spring. Original RNG/difficulty/ability setup, playable stock orders, authentic merchant scheduling/Trade, autonomous AI, 12 commands, diplomacy, dynasty/marriage, battle terminal/aftermath and complete campaigns remain open.
