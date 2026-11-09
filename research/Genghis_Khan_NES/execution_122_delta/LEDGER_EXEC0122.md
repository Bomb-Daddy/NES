# Genghis Khan I completion ledger — EXEC0122, November 9, 2026

**IMPLEMENTED:** Inherited complete EXEC0121 C11 native game, plus integrated correct NES CPU ORA/AND/EOR across all 24 documented opcodes and addressing modes. Added exact CPU cycles and independent reference tests. Protected source read gates remain unchanged; no introduced private ROM bytes.

**VERIFIED_STATIC:** 24 opcode/mode decoding and semantics, zero-page pointer wrapping, page penalty and N/Z flag oracle, source linkage and package version.

**TESTED_PORT_LINUX:** See exact CTest and compiled binary/package logs; do not promote to Windows. The original-stock source-unknown RAM gate remains.

**OBSERVED_RUNTIME_STOCK (previous, NOT rerun):** fresh Spring1175 Gold430 Food208 Troops61 Rate121 orders3, archived Spring1175 Gold439 Food208 Troops61 Rate101 orders3, archived World Spring1206 Gold193 Food332 Troops116 Rate114 orders3. Do not hardcode screenshot results or interchange nesWorkRam with nesSaveRam physical0x0DCD. Goods begin at +0x22.

**NOT RUN/OPEN:** source-authentic original automatic setup, difficulty/character RNG, first Spring and live three orders; original Trade merchant presence, 12 strategic commands, autonomous AI, diplomacy, dynasty marriage ages6/7/8, battle terminal/aftermath, real graphics/APU, both campaigns, Win32 NSIS and physical Xbox-only acceptance. No full release.

Only increment after independent source artifact, runnable QA and publication evidence. Keep existing hourly execution enabled until accepted full release or Director stop.
