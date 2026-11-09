# GK1 EXEC0100 native C11 handoff — 1.00.0-dev1

Authority: complete, same `execution_099` native C11 game inherited without source deletions or user campaign overwrite, now `execution_100`; original ROM only user-private. Do not confuse source-derived model with real stock Mesen runtime or game parity.

## Actual source and player-visible changes

- `gk100_cpu_cycles.h/.c` adds authentic NES 2A03 **completed-instruction** cycle counts for every previously executed/implemented 6502 opcode. Branch taken and crossed-page +1 costs, indexed READ penalties, and fixed RMW/STORE penalties are separated. Unimplemented opcodes remain fail-closed.
- `gk83_reset_trace.c/.h` accumulates `completed_cpu_cycles`, records writes attempted to the mirrored PPUCTRL ($2000, $2008..$3FFF / every eighth address), records bit7 NMI request intent, source PC/value, and counts a source-derived 7-cycle interrupt entry only for the **explicitly hypothetical** one-shot NMI. No PPU/APU/NES input event scheduling was added; zeroed RAM only under the same source-verified Mesen `RamPowerOnState=1` option.
- Controller START original-stock source probe in the **same 1280x720 native app** now prints dynamic CPU cycle count and NMI-intent gate with clear `HYP` and `PPU CLOCK UNMODELED` warning, preserving world state and avoiding invented Spring RNG.
- New `test_gk100_cpu_timing.c` tests 2/3/4-cycle branches, all index penalty modes, no erroneous STORE penalty, known bounds, unknown opcode guard, deterministic original SHA-guarded traces, and explicit original-source PPUCTRL write intent. No copyrighted bytes embedded.
- Native Linux install metadata and version now consistently `1.00.0-dev1`, including launcher, .desktop, package verifier and package name. Existing Win32 XInput 4-slot source retained, **NOT RUN Win32**.

## Results

Private original stock `262160`-byte ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` was actually read by a guarded native Linux source trace; the test emitted **PPUCTRL write attempts=5, bit7 attempts=2, source last=$10 at $C71F; hypothetical one-shot NMI after 7730 completed CPU cycles**. The 7730 is **not** original NES elapsed cold-start cycles: supplied PPUSTATUS reads were hypothetically immediately true, whereas real NTSC PPU startup takes many more cycles. Thus the hypothetical NMI trace is not physical NES timing or first Spring parity. The source table/CPU timing passed counterfactual replay, source gate and rollback testing; original real Spring preserved as prior observations only.

`cmake -S all/execution_100 -B build100release -DCMAKE_BUILD_TYPE=Release`, `cmake --build build100release -j8`, `ctest --test-dir build100release --output-on-failure -j8`: **59/59 PASS**, and Debug plus ASan/UBSan (`detect_leaks=0`) without stock ROM: each **59/59 PASS**. Focused with private original stock ROM Release **6/6 PASS** and sanitizer **4/4 PASS**. Actual `xvfb-run ... --smoke-frames 120` = **120/120 native X11**. `cpack -G DEB` produced installer, extracted ELF `--version` reports 1.00.0-dev1. No native Windows test or physical Xbox test. No new original Mesen emulation run.

## Remaining core blocker and next work

True original 1174→1175 / World 1205→1206 first Spring still needs actual Mesen PPU/NMI source event chronology, real ability/difficulty/player choices and RNG parity. Original playable campaign remains blocked; all 14/27 country template vs Spring country records must not be hardcoded from one RNG history. Trade viewer is read-only; live merchant presence/RNG and transactions, 12 locked commands, autonomous AI, diplomacy, dynasty ages6/7/8, battle terminal/aftermath, sound, graphics, full save/cold-load campaigns and Windows+Xbox validation remain incomplete. No implied recipient developer acknowledgment from uploading artifacts.
