# GK1 EXEC0209 — Authenticated archived Spring RNG underdetermination — implementer and hostile QA

**Date:** 2026-11-11 America/Detroit. **Predecessor:** exact complete EXEC0208 C11 game, 1.99.0-dev1 uncertified, original-source import and all other inherited game systems preserved. **Scope:** Genghis Khan I English NES native Windows/Linux port only. The game is NOT certified, and this is NOT a standalone prototype.

## Changed code in the existing native game

- `execution_199/gk209_archived_rate_ambiguity.c/.h`: read-only authenticated source audit. It calls the existing complete original-USA-ROM + individual full Mesen `.mss` SHA validator `gk208_archived_source_rng`, then calls the preexisting original-source inverse 48-bit PRNG rate search `gk138_scan_spring_rate`. It refuses fresh WorkRAM input as an archived SaveRAM checkpoint, demands odd post-draw boundary and scans a *hypothetical* 4,096 prior-draw window against all five difficulty-rate formulas. It outputs multiple plausible matches but NEVER selects a chronological actual draw. It sets neither `chronology_proved` nor `first_spring_generated`.
- `execution_199/gk1_native_shell.c`: adds `--audit-stock-archived-rate-ambiguity ROM MSS mongol1175|world1206 4096` using the same linked native game. Prints 5 per-difficulty candidate counts and explicit no chronology.
- `execution_199/CMakeLists.txt`: links source into native game target and adds CTest 172; integrates README into Linux packaging.
- `execution_199/test_gk209_archived_rate_ambiguity.c`: strict argument checks, missing/cross-profile archived refusal, result rollback and original SHA-authenticated private input checks. 38 assertion checks with private source fixtures, ASan/UBSan PASS.
- `execution_199/README_EXEC0209.md`: reproduction and boundaries.

## Exact observation types and results

**VERIFIED_STATIC:** Private original stock USA ROM 262160 bytes SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; original stock archived Mesen identities Mongol1175 `a3838705c58010a11cac37ae2d8f446cccf29aa7b904f41b199a604a98245cce`, World1206 `d0806e73bd98f3f2c4df1d1458dc668dbe8a711d925da95ae5ef82393d1389fb`. Original bytes and full Mesen state private, **not present in any public package**. On each archived state, 4096 mathematically possible reversed RNG candidates scanned *per difficulty* 1..5 (20,480 per archive), with observed original Spring Rate labels (Mongol 101, World 114). Candidate matching prior draws:

| Archived state | Diff 1 | Diff 2 | Diff 3 | Diff 4 | Diff 5 |
|---|---:|---:|---:|---:|---:|
| Mongol Spring 1175 SaveRAM, Rate101 | 45 | 31 | 28 | 42 | 29 |
| World Spring 1206 SaveRAM, Rate114 | 46 | 39 | 40 | 24 | 38 |

All five difficulty formulas have multiple matches in both profiles. **Retrospective compatible RNG arithmetic does not establish the actual RNG chronology, the selected difficulty, the original startup seed, a real source-executed RNG call, or a first Spring implementation.** In particular do NOT copy a nearest matching draw into the native game. The separate fresh stock WorkRAM first Spring1175 Gold430 Rate121 is a different source history and is not an archived SaveRAM input. Original archived Mongol Gold439 Rate101 and World Gold193 Rate114 are preserved, not universal first-Spring constants.

**OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** Previous Mesen captures only. Existing authentic archived SaveRAM original states used as source inputs; no new emulator session.
**OBSERVED_RUNTIME_STOCK_FRESH_RESET:** Earlier fresh WorkRAM source capture preserved separately, not rerun or conflated.
**TESTED_PORT_LINUX:** Ninja Release 172/172 CTest PASS, Debug 172/172 CTest PASS, clean independent exact EXEC0208 archive + patch replay 172/172 CTest PASS. Original full private archived source checks 38 PASS, focused ASan+UBSan PASS. Linux native and Debian-extracted graphical 1280×720 120-frame Xvfb PASS. Deliberate corrupted ROM, corrupted Mesen and cross-profile archive refused. All six changed/new paths independent patch replay byte-identical.
**NOT RUN:** New original Mesen runtime with CPU watch; native Windows Win32 binary and NSIS installer; physical Xbox controller; full Mongol/World campaigns. No first Spring end-to-end or gameplay completion.

## Exact reproducibility

```sh
# Start with the COMPLETE, UNMODIFIED EXEC0208 source archive, not older EXEC0062.
unzip GK1_EXEC0208_Complete_Integrated_C11_Source_QA.zip -d exact208
cd exact208
git apply ../GK1_EXEC0209_CopyrightClean_Integration.patch
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
# Private original stock evidence must be supplied separately; DO NOT COMMIT or package it.
./build/test_gk209_archived_rate_ambiguity ORIGINAL_STOCK_USA.nes ORIGINAL_MONGOL1175.mss ORIGINAL_WORLD1206.mss
./build/gk1_native_shell --audit-stock-archived-rate-ambiguity ORIGINAL_STOCK_USA.nes ORIGINAL_MONGOL1175.mss mongol1175 4096
./build/gk1_native_shell --audit-stock-archived-rate-ambiguity ORIGINAL_STOCK_USA.nes ORIGINAL_WORLD1206.mss world1206 4096
```

**Next executable priority:** trace authenticated fresh original-Mesen reset through actual 6502 `$0084–$0089` seed writes + original setup/difficulty/controller chronology, then implement source-authentic first playable Spring for all 14/27 countries in the inherited C11 application with three orders/season and original Trade. Do not substitute a synthetic ROM result or unverified constant. Then original AI/diplomacy/dynasty/battle aftermath, complete persistent saves, original sound/graphics and actual Windows/physical Xbox acceptance. Retain separate stock Classic and approved Enhanced mechanics. No public ROM/stock Mesen assets, no deletion of older source, no claim that a published handoff is a different agent's execution acknowledgment.
