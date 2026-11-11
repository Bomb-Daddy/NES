# GK1 EXEC0202 — original stock IRQ/BRK RNG indirect dispatch and Linux CLI smoke fix

**Status:** Reproducible native C11 implementation candidate, building on EXEC0201 reconstructed native source, **not** the missing exact historical EXEC0197 predecessor. No full-game certification or authentic first-Spring initializer.

## Implemented in inherited native game (not separate prototype)

`gk202_irq_rng_dispatch.c/h` source authentication is integrated into the existing `gk1_app_state` library and real `gk1_native_shell` via `--audit-stock-brk-rng-dispatch PRIVATE_ROM_PATH`, accessible only while in the source-awaiting setup gate. The audit refuses wrong SHA256, wrong length/header, malformed dispatch and leaves outputs unchanged on failure. Original NES program/opcodes and 34-member authentic evidence archive were never copied to the public source or package. Machine-code windows are authenticated by SHA256, not embedded original opcode arrays.

## New source finding — VERIFIED_STATIC only

Exact user-authorized original USA stock image, 262160 bytes SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. Last 16 KiB fixed PRG bank, original IRQ/BRK vector `$FFFE → $C14B`. In handler: pushed status BRK bit `$10` selects software dispatch; `$C166` reads command selector RAM `$0050`; `$C726` computes 3×slot; indirect dispatch pointer `$0068` targets fixed-bank `$C185 + 3×slot`; the 33 three-byte JMP entries contain slot **17** at `$C1B8 → JMP $C289` (original RNG entry). This explains why previous direct-JSR-only bank scans reported zero `JSR $C289`: this route uses a BRK-mediated indirect JMP. Raw ROM sequence search: direct `JSR $C289` = 0, direct `JMP $C289` = 1. Source finding is testable, but **not live execution proof**; `$0050` assignment, BRK entry from startup and the exact original initial seasonal sequence remain untraced. Do NOT claim the initializer was implemented or that merchant RNG now works.

## Additional player-facing fix

Uncovered and repaired an inherited `if`/`else if` parser break that caused `--smoke-frames 120` to return code 4 even when compiled. Existing CLI argument parsing now shares one conditional chain and the Linux native 1280×720 X11 smoke completes 120 frames. Added CTest guarded by `xvfb-run` presence.

## Reproduction

```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
# private machine ONLY:
./build/test_gk202_irq_rng_dispatch /private/path/stock_original.nes
./build/gk1_native_shell --audit-stock-brk-rng-dispatch /private/path/stock_original.nes
```

Patch base: `GK1_EXEC0201_CANDIDATE_Full_Native_C11_Source_and_QA.zip` (uncertified historical predecessor). Apply `GK1_EXEC0202_Candidate_CopyrightClean_Integration.patch` to the directory that contains `execution_199/` and sibling `execution_034`, `035`, `039`, `040`, `045`, `047`, `049`. Independently replayed all current source files; compare per-file `GK1_EXEC0202_QA_Receipt.json` SHA256. No ROM included.

## Evidence

- VERIFIED_STATIC: original fixed-bank IRQ/BRK indirect dispatcher, selector slot17, checksum-gated analyzer, no mutation on refusal; no original ROM redistributions.
- OBSERVED_RUNTIME_STOCK: **no new Mesen execution**. Earlier archived-restored Spring1175 & World1206 and fresh-reset Spring1175 observations stay distinct.
- TESTED_PORT_LINUX: Release 165/165 CTest, Debug 165/165, original-stock-focused 40 positive/negative assertions incl ASan/UBSan, 120 Xvfb graphical frames 1280×720, rebuilt Debian installation to be verified independently.
- NOT RUN: Win32/XInput on physical Windows, NSIS installer, physical Xbox, complete 14/27 campaigns.
- Inherited gap: exact EXEC0197 historic C11 source missing, 12 incompatible inherited hashes vs canonical EXEC0199; this is a RECONSTRUCTION CANDIDATE, not a certified historical release.

## Next highest priority

Source trace CPU writes to `$0050` and actual BRK callers and conditional dispatch through stock Mesen (fresh and archived separately). Bring those precise branches and the 14/27 country state transitions into existing app; do not hardcode Gold430/439 or Rate101/121. Implement original first Spring for authentic scenario/difficulty/character RNG with exactly three orders. Keep Classic separate from Enhanced Shared-BP/Rest. Finish merchant scheduling, original battle aftermath, dynasty/diplomacy, AI and true Win32/physical Xbox campaigns. Publication is not a separate agent's read or execution acknowledgment.
