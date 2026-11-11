# GK1 EXEC0191 — Implementer / Independent Hostile-QA Handoff — 2026-11-11

**Authoritative direct predecessor:** EXEC0190 v1.90.0-dev1, 601 source files; this is not EXEC0076. Source was extracted from the exact inherited historical source archive, NOT replaced by a new prototype. Latest numbered Collaboration and public GitHub checked before version allocation; no EXEC0191+ existed at that check.

## Executed original stock source tracing change

- Added **conditional $EF85 dispatcher inline-selector destination graph** to the existing `gk181_bank_seed_trace.c/.h`: read source byte immediately after JSR ($EF85) at callsite+3; resolve fixed original bank15 address tables `$F788/$F888` under explicit selected-low-bank hypothesis; safely stop on unmapped targets / truncated inline selector. Preserve old mapper write, indirect jump and source-only provenance limits.
- `test_gk191_dispatch_rng_tables.c` with synthetic positive dispatch, wrong selector, unsupported/unmapped target, low-bank mismatch, wrong-length negative control, output no-mutation on errors, and independently SHA-authenticated actual private stock ROM.
- Under all **15 original USA stock bank hypotheses**, 8 possible dispatcher-table edges were newly identified; zero directly decoded conditional `JSR $C289` sites. This is an **unexecuted static conditional source graph**, NOT a claim of no RNG use, not startup initialization, and not actual Mesen parity.
- No private ROM, RAM/save, Mesen states, screenshot or licensed original game asset is in public source; original stock exact 262160B SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` remains PRIVATE and was passed to focused test at runtime.

## QA evidence

- `cmake -S execution_191 -B build191 -DCMAKE_BUILD_TYPE=Release; cmake --build build191 -j12; ctest --test-dir build191 --output-on-failure -j12`: **154/154 PASS**.
- Independent Debug `ctest`: **154/154 PASS**.
- Independent exact EXEC0190 patch reconstruction **603/603 SHA-256 source files**, independent clean reconstruction `ctest` **154/154 PASS**.
- Authentic private stock `test_gk191_dispatch_rng_tables /private/original_ROM`: **50/50 assertions PASS** (15 bank hypotheses).
- ASan+UBSan focused original-stock analyzer: **50/50 PASS**; no sanitizer errors in this focused test. Not the entire 154 suite under sanitizer.
- `xvfb-run -a build191/gk1_native_shell --smoke-frames 120` and **separate extracted Debian executable**: both returned 0 and reported 1280×720/120 frames.
- Debian archive CPack generated on Linux. **Native Windows binary, NSIS, physical Xbox and full campaigns NOT RUN**.
- Previous historical archive **42,050 entries** retained exact (CRC, filenames, size), current source 603 entries appended; current 42,653 total. Source archive ZIP CRC passed. Independent replay changes are file content identities, not assertion about original NES execution.

## Current gameplay reality and next mandatory tasks

**Source-authentic first Spring not solved.** Previous stock source country1 true fresh WorkRAM Spring1175 Gold430/Rate121 vs independently archived SaveRAM Gold439/Rate101 are different actual histories, not universal setup constants. Native Classic still imports authenticated source rather than synthesizing both 14/27 country Spr 1175/1206 with stock difficulty/ability/character/RNG and three genuine source-authentic commands. The manual-derived Classic ASSIGN and port-only Y Undo are not NES command proof. Trade merchant presence, AI, original diplomacy/dynasty, battle aftermath, full campaign persistence, original audiovisual, Windows installation and physical controller testing remain open. Next step: trace mapper bank writes and indirect dispatch through fixed dispatcher, then source-reset timeline into full state generation, testing fresh and archived independently.

This handoff is implementation evidence, not Director release acceptance or other-agent read/execute acknowledgment. Recurring development must remain active until explicitly stopped or complete verified release.
