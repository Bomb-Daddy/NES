# EXEC0121 source/QA handoff — 2026-11-09

Inheritance: COMPLETE EXEC0120 copyright-clean C11 native research application. This is not an isolated micro-demo, and the complete previous ZIP history is retained. App: 1280x720 Linux X11 / Windows Win32-XInput intended; Win32/Xbox NOT RUN.

Actual code: `gk121_bit_flags.c/.h` provides pure documented NES 2A03 BIT (zero page/absolute) status semantics. Wired directly into the existing private stock-ROM CPU source interpreter `gk83_reset_trace.c`, with `gk100_cpu_cycles.c` supporting exact 3/4-cycle BIT, 2-cycle CLV, and 2/3/4 cycle BVC/BVS branches. BIT refuses any unknown RAM or unmodeled PPU/APU read rather than manufacturing an original NES status result. The program does NOT unlock original first Spring or the original three orders.

Test: `test_gk121_bit_flags.c` exhaustively covers 524,288 accumulator/memory/flag combinations, preserved flags, all four cycle and page branch cases, plus inherited tests. Original private ROM SHA was verified locally; bounded 70-instruction reset still correctly gates on unknown RAM $00F5 and makes no false parity claim. Old emulator research and authentic Mesen history remain separate.

To reproduce (Linux):
```
cmake -S execution_121 -B build121 -DCMAKE_BUILD_TYPE=Release
cmake --build build121 -j8
ctest --test-dir build121 -j8 --output-on-failure
(cd build121 && cpack -G DEB)
xvfb-run -a build121/gk1_native_shell --smoke-frames 120
```

Status: VERIFIED_STATIC for opcode and source; TESTED_PORT_LINUX for CTest/graphical/package; OBSERVED_RUNTIME_STOCK historical captures only, no new Mesen run; NOT RUN native Win32, NSIS, physical Xbox. No executable original first Spring RNG, real Trade, autonomous AI, diplomacy, dynasty, battle aftermath or accepted campaigns.

Publication requires upload / readback and must not be confused with an agent ACK. NEVER put original ROM, screenshots, NES RAM, SRAM, checkpoint bytes or licensed artwork on public GitHub.
