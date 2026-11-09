# EXEC0125 — original NES 2A03 binary ADC/SBC, source-gated, 2026-11-09

This directory is the **copyright-clean public delta**, not a standalone playable game. It accompanies the complete inherited 343-file native C11 `execution_125` source tree in the locally verified historical archive. Base is **EXEC0124**, not EXEC0062. Do not publish original NES ROM, modified ROM, SRAM, screenshots, original graphics or licensed audio.

## Implemented change

- `gk125_cpu_arith.h/.c` provides exact official 2A03 ADC and SBC decode, 16 addressing forms, binary arithmetic ignoring the CPU's decimal flag (which itself remains preserved), N/Z/C/V flags, and cycle timings.
- `test_gk125_cpu_arith.c` exhaustively checks 67,108,864 independent arithmetic/status combinations, all 16 decoder forms, page-cross cycles, null/invalid rollback, and the bounded original-stock trace gate.
- Add `gk125_cpu_arith.c` to the existing `gk1_app_state` CMake source list and `test_gk125_cpu_arith` to CTest. Retain the entire original game and predecessor tests. Version to `1.25.0-dev1`.
- In the original `gk83_reset_trace.c`, include `gk125_cpu_arith.h` and dispatch before the older opcode switch. Call `gk125_decode(op,&ar)`; fetch according to `ar.mode` using existing `operand`, `word` and source-gated `read_byte` functions; use 8-bit zero-page wrapping for X-indexed and zero-page pointer high bytes, and `gk100_page_crossed` for absolute X/Y or indirect Y. If all reads succeed, call `gk125_eval(ar.kind,m->a,b,m->p,&m->a,&m->p)` and then the inherited instruction-finalization path. Remove superseded duplicated ADC/SBC switch arms. **Do not bypass known-RAM guards or seed unknown RAM values.**
- In `gk100_cpu_cycles.c` after prior registered opcode handling, if `gk125_decode(op,&ar)`, return `gk125_cycles(op,page_crossed)`. The complete tested implementation is in the historical source ZIP; the public GitHub repository contains this portable delta, not a false full replacement.
- Preserve previous 14 Mongol/27 World source-based PRE_SETUP country worlds, Classic/Enhanced profiles, controller screens, battle research, XDG/LocalAppData saves, and all earlier source archives.

## QA — 2026-11-09

`TESTED_PORT_LINUX`: 83/83 Release, 83/83 Debug, 83/83 ASan+UBSan (leak detection disabled), 83/83 independent clean historical-source extraction, 1280x720 X11 120 frames from built executable and from extracted Debian installer. The optional private, independently SHA-verified original USA stock ROM test still stops after 70 original CPU instructions at unknown work RAM `$00F5`. **No original first Spring generated. No new Mesen gameplay run.**

`OBSERVED_RUNTIME_STOCK` only preexisting: fresh Spring 1175 Mongol country1 Gold430 Food208 Troops61 Rate121 orders3; archived Spring 1175 Gold439 Food208 Troops61 Rate101 orders3; archived World1206 Gold193 Food332 Troops116 Rate114 orders3. Those are histories, not constants. `NOT RUN`: Win32 binary, NSIS, physical Xbox controller.

## Local artifacts and publication limits

- Full historical ZIP: `GK1_EXEC0125_Complete_Historical_C11_Source_v1.00.zip`, 20,243,321 bytes, SHA256 `08cd259fca2448695fcf3c453f4f95ffc523aed402c5c5d1583eefff149a89c8`.
- Linux installer: `GK1_EXEC0125_Linux_Research_Installer_1.25.0-dev1_amd64.deb`, SHA256 `31984656de8e27d84e5dd57358d7f1138459dfb89a4cdb42f5db9d26beadb24e`.
- QA archive: `GK1_EXEC0125_Linux_QA_Source_Manifest_v1.00.zip`, SHA256 `b4c499c33096d84a0aea8dae91e89c68647f78af090cf0fffd751d3879ba73de`.
- These artifacts were built and verified **locally**. Google Drive binary transfer was refused by the connector: **Drive ZIP/DEB publication NOT certified**. New cumulative Drive master v3.03 and EXEC0125 ledger were successfully published separately.
- Public GitHub source is only this delta. Publication is not execution/read ACK by another agent.

**OPEN:** original player/difficulty/stat RNG, authentic first playable Spring and original 3 orders, merchant scheduler/Trade, AI, 12 original strategic commands, diplomacy, dynasty/marriage ages6/7/8, battle terminal/aftermath, complete campaigns, 8-bit audiovisual fidelity and verified Windows/physical-controller release. Hourly work remains enabled.
