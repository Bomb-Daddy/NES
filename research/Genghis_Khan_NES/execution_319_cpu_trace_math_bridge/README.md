# Genghis Khan I — EXEC0319 original-stock NMI/STORE/C289 trace arithmetic bridge

**Source stage:** current native C11 `Gk1App` after verified EXEC0316 + EXEC0317 (not the superseded EXEC0062). The project is **Genghis Khan I English NES**, not ROTK II or a new KOEI-inspired game.

## Source gap closed

EXEC0316 can accept a syntactically valid CPU event *claim* that contains impossible C289 draw values, omitted/changed $0084–$0089 seed changes, or inconsistently interleaved original-stock NMI callbacks. EXEC0317 bounds such claims to the correct selected campaign, but does not prove arithmetic. EXEC0319 links a new C11 source into the **existing** native app library, and tests a strict, fail-atomic reconciliation of event sequence claims.

`gk319_reconcile_cpu_trace_math()` requires an explicit first complete six-byte source-writer set, checks each STORE before/value against the claimed A/X/Y registers, applies the already established GK300 zero-$8A-gated NMI +2 big-endian 16-bit math, then independently checks **every** source GK137/C289 48-bit after-state and 15-bit draw. It rejects missing writers, altered prior seed, corrupted C289 draws, fabricated NMI gate changes, decreasing event cycles, duplicate/mismatched counts and self-promoted emulator/turn claims. The output receipt is atomic and does not mutate any game state.

**Limitations:** A self-consistent claimant can still fabricate an entire history; accepted arithmetic is NOT emulator provenance, and original ROM opcode/PC/bank and runtime identity remain separate GK304/GK312/private-Mesen gates. Source STORE register membership alone does NOT establish the opcode. These checks are not original setup/character formulas. No original-stock first Spring is generated, and no real orders are granted.

## Reproduce

From the actual source repository, after exact staged EXEC0316/317 complete native game integration:

```
python research/Genghis_Khan_NES/execution_319_cpu_trace_math_bridge/test_stage319.py
python research/Genghis_Khan_NES/execution_319_cpu_trace_math_bridge/stage319.py current-gk1/execution_199
cmake -S current-gk1/execution_199 -B build-gk319
cmake --build build-gk319 --parallel 4
ctest --test-dir build-gk319 -R GK1_319_SourceCpuNmiStoreC289MathClaimsNoOrders --output-on-failure
```

The integration script SHA-pins the three new C11 files; it fails closed if EXEC0317 or the current app CMake library is missing, a duplicate is present, or malformed source bytes are found, and restores all previous bytes on write failure. The production dependency is the **existing** `gk137_stock_rng`, `gk300_stock_nmi_seed_clock` and `gk316_cpu_claims_ingest`, not the local `qa_inherited` reference extracts.

Local direct Linux C11 compile/test: **46/46 assertions PASS** with `-Wall -Wextra -Wpedantic -Werror`; ASan+UBSan **46/46 PASS**. Python hostile staging **11/11 PASS**. The `qa_inherited` directory inside the Drive ZIP contains minimal, public, copyright-clean copies of the previously published model portions for an independent standalone component build; these are NOT a complete game or a substitute for native app validation.

**Acceptance labels:** VERIFIED_STATIC source SHA, local code and regression confirmed; TESTED_HOST_LINUX_C11_COMPONENT PASS; TESTED_PORT_LINUX full native app NOT RUN this local build; TESTED_PORT_WIN32_CI EXEC0319 PENDING until actual GitHub CI; OBSERVED_RUNTIME_STOCK fresh vs archived no new capture, old evidence preserved; physical Xbox, full campaigns NOT RUN.

Neither ROM nor raw Mesen/RAM/checkpoint/image/audio data is in this source/QA package. Private original ROM is SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3; original ROM and private evidence remain in the designated Genghis research folder. No physical hardware or release acceptance claim.
