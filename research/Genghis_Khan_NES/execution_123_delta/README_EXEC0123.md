# GK1 EXEC0123 — 2A03 load/store source-gated implementation

**This is an incremental source delta, not a full release.** The complete locally tested native C11 app is supplied in the historical source ZIP. Its baseline is the complete EXEC0121 ZIP plus source-equivalent re-integration of GitHub EXEC0122 logical opcode behavior. The exact complete EXEC0122 ZIP was not available; no claim of its byte-identical inheritance.

The files `gk123_cpu_loadstore.c/.h` and `test_gk123_cpu_loadstore.c` are **independently Git-blob verified against the actually compiled and tested code**. To integrate them into the existing complete native app, wire the 31 official LDA/LDX/LDY/STA/STX/STY modes into `gk83_reset_trace.c` **before its opcode switch**, using the preexisting `operand`, `word`, `read_byte`, `write_byte` and `gk100_page_crossed` functions. The exact integrated code and packaging are available in the complete EXEC0123 historical source ZIP, not recreated by this README. Add `gk123_cpu_loadstore.c` to the `gk1_app_state` CMake library, add the new test executable, and call `gk123_cycles` in `gk100_cpu_cycles` before the legacy switch; preserve all EXEC0122 ORA/AND/EOR logic modes and their timing.

**Behavior:** Source-gated read/write provenance, zero-page X/Y wrapping, indexed indirect X pointer wrapping, indirect indexed Y page-cross penalties, fixed store cycles, loads update only N/Z and stores preserve P. Never substitute zero for unknown NES RAM or PPU data.

**Verified host QA**: Release 81/81; Debug 81/81; ASan+UBSan 81/81 with leak checks disabled; independent source ZIP rebuild 81/81. Actual Linux 1280x720 X11 graphics 120 frames and extracted Debian package 120 frames. Exhaustive module checks: all 31 official forms, all 225 decoder exclusions, 65,536 status/byte checks, 65,536 zero-page address wraps, fixed cycle and page-cross cases. No original stock Mesen run in this increment.

**Open:** Independent source-authentic original first Spring and three playable orders, merchant presence/Trade, AI, dynasty/diplomacy, remaining strategic commands, battle aftermath, Windows/NSIS, physical Xbox and full campaigns. Preserve fresh 1175 original Mesen Gold430/Rate121 and archived 1175 Gold439/Rate101 separately; do not hardcode. Stock ROM, SRAM and licensed artwork/audio remain private.

**Artifact hashes**: historical C11 ZIP SHA256 `45b5d888ad047fde7587bfab2d2c636f8e6961bbcd334fab90022283aa426611`; Debian `103548c094d3dcbc1f976b53779de69d5fe6538a7d9df76eca99c1ef2d095f56`; QA `d6f6b168dfc7dcbc0ac0b06b4938cadaaf06d2b63119a4edaf21eea234f044b8`.

The raw ZIP/installer transfer to Google Drive failed (`container_session_unavailable`). The numbered handoff/ledger Docs and cumulative v3.01 master are published in the correct Collaboration folder. **This public GitHub directory is a partial delta, not the complete verified 331-file source tree.** Publication is not another agent's read/execution acknowledgment.
