# Genghis Khan I native C11 — EXEC0304

## Scope

An exact-source delta on the complete inherited EXEC0300 native C11 game with the already-integrated EXEC0302 source chronology verifier. The original ROM is never included. The delta does **not** turn PRE_SETUP into an original playable Spring, grant orders, add merchant offers, or claim live original Mesen execution.

## New actual source defect found and corrected

The source opcode-validated EXEC0302 `Gk302SeedEvent` STORE replay only validated `before`, `source_pc`, target, mapped PRG bank, and subsequent C289 consistency. A claimant could set `after` to a byte that the corresponding 6502 CPU register could not have written, then recompute the claimed C289 history; EXEC0302 would accept the synthetic input. EXEC0304 checks the recorded A/X/Y register from an external CPU witness against the **real original SHA-256-pinned stock ROM** `STA`, `STX`, `STY` opcode and operand. The new regression proves the old accepter/updated rejecter on a malicious non-A value. Witnesses remain externally asserted until separately captured by authenticated live Mesen.

## Reproducible clean integration

1. Restore the **complete inherited** EXEC0300 source and all dependency siblings (`execution_034`, `039`, `045`, `047`, `049`). Apply the exact authenticated `stage302.py` that integrates the original C11 chronological module into the same native app. Do **not** skip source/initial state checks or use an isolated demo.
2. Run `python3 stage304.py <inherited-app-folder>`. This SHA-pins the EXEC0302 C/H/CMake preimages, verifies all three newly published C11 files and stages the exact postimage, with rollback on any failure. CRLF is normalized explicitly and unrecognized text is refused.
3. Configure and compile: `cmake -S <inherited-app-folder> -B build -DCMAKE_BUILD_TYPE=Release -DGK304_PRIVATE_ROM=<local private SHA-verified original ROM path>` and `cmake --build build -j 8`. The private test is **conditionally enabled only when the ROM path is supplied**. Without the ROM, the safe public refusal test still runs.
4. Test: `ctest --test-dir build --output-on-failure -R GK1_304_`; run the full suite separately and classify environmental exclusions. `python3 test_stage304.py <unpatched full EXEC0302 app folder>` validates hostile staging/rollback.

## Tested evidence — 2026-11-15

- VERIFIED_STATIC: independent authenticated original ROM identity and source opcode mapping; no copyrighted ROM/SRAM/audio/checkpoint files included. Original source-claim spoofing rejected.
- TESTED_PORT_LINUX: native C11 app compiled, 2/2 new ROM-gated/public CTests pass; 238/238 eligible inherited CTests pass. Two independent environmental tests excluded: stale relative workflow-file location `GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass`, and slow `GK1_225_TwoProcessTitleControllerColdLoad` (initial unfiltered suite was interrupted). Full unfiltered 240/240 **NOT claimed**.
- HOSTILE_SOURCE_STAGING: 9/9 tests pass, including CRLF normalization, corruption, duplicate injection, failure rollback, and exact postimage.
- OBSERVED_RUNTIME_STOCK_ARCHIVED / FRESH: earlier authentic original evidence preserved, **no new live Mesen execution in EXEC0304**.
- TESTED_PORT_WIN32_CI: prior EXEC0303 222/222 retained; **EXEC0304 Windows not run**.
- Physical Xbox controller/full campaign/first playable stock Spring: **NOT RUN / NOT IMPLEMENTED**.

## Provenance

Original ROM and private NES evidence stay exclusively in the designated private Genghis Drive research folder. This package contains only source, patch, programmatic tests and non-private QA. Publication is not an agent read acknowledgment, tested Windows integration, or final game acceptance.
