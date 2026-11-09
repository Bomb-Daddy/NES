# GK1 EXEC0117 — stock original template-attestation boundary (2026-11-09)

## Branch and supersession

This is an independent, additive patch to the **last locally retrievable complete C11 native tree, EXEC0115**. EXEC0116 already exists in the Collaboration Drive guide and its three GitHub implementation files, but the complete EXEC0116 tree/install package is NOT accessible in this runtime. Therefore **this source directory and its installer are NOT a replacement for the documented EXEC0116 Enhanced Assign implementation**. Apply these three new source/test files and the two production wiring edits to the EXEC0116 full tree once recovered; do not delete or supersede its labor functionality.

## Real production defect fixed

`gk79_open_original` SHA-verifies the private 14*83 / 27*83 original country records but accepts an arbitrary caller-supplied PRE_SETUP `Gk1LoadedScenario`. A separate mutable pre-setup template can contain modified fields not present in the 83-byte stock record (e.g., original country/lord names or extra-tax marker), yet the imported game checkpoint is marked `source_verified=1`. This silently mixes authentic original stock numeric RAM fields with unsourced port modifications. The original stock first-Spring player may then receive a source-verified game world containing manipulated values.

New `gk117_stock_pristine_gate.c/h`: read the **exact SHA-identified 41-country source CSV**, open the exact canonical all-country pre-setup world and compare its whole `Gk1LoadedScenario` to the supplied state *before* attaching stock RAM via the existing `GK77/GK79` verifier. The native shell now calls `gk117_open_attested_original(...)`, not direct `gk79_open_original(...)`. Any refusal preserves the caller's prior `Gk79Checkpoint` byte-for-byte. This closes the production identity mismatch without hardcoding first-Spring Gold/Rate, changing any original record, inventing a merchant RNG, or advancing the season.

## Actual tests and evidence

- Linux Release: 74/74 CTest PASS.
- Linux Debug: 74/74 CTest PASS.
- Linux ASan/UBSan reduced suite: 73/73 PASS; inherited expensive GK101 excluded; full sanitizer suite NOT certified.
- Focused new ASan/UBSan positive tests PASS using exact PRIVATE authenticated country records: original fresh **14** WorkRAM records SHA `04eb8536...`; original archived Mongol **14** SaveRAM records SHA `ba3f26ae...`; original archived World **27** SaveRAM records SHA `0f9f254d...`. All 41 original-country records are authenticated from the prior Mesen captures; NO new Mesen gameplay run occurred.
- Negative controls: absent/edited source CSV, unsourced changes in nonplayer country, player name and commodity, wrong RAM bank/profile, invalid scenario, post-Pass immutable source, and no mutation on refusal.
- Source checkpoint manual-derived Pass consumes the remaining three orders but does not create a next season; original source countries remain byte-identical after Pass.
- Native Linux X11 1280×720 120-frame smoke PASS.

## Provenance classification

VERIFIED_STATIC: native source branch, compiler, CTest, SHA source checks.
OBSERVED_RUNTIME_STOCK (earlier Mesen, NOT rerun): fresh Mongol1175 Gold430 Food208 Troops61 Rate121; archived Mongol1175 Gold439 Food208 Troops61 Rate101; archived World1206 Gold193 Food332 Troops116 Rate114; these remain separate histories.
TESTED_PORT_LINUX: Release, Debug, reduced sanitizer, Xvfb 120 frames; focused authentic private records acceptance.
NOT RUN: native Win32 NSIS installer, physical Xbox controller, true independent native-ROM startup/first-Spring RNG, autonomous AI, battles to final state, completed campaigns.

## Remaining critical blocker

The native game does NOT independently initialize the original first playable Spring from fresh setup and RNG; this patch instead authenticates existing stock captures at an earlier trust boundary. Real Trade merchant presence/offer RNG, original 12 locked strategic commands, dynasty age 6/7/8, battle terminal, AI, Windows and physical Xbox remain unfinished.

No original ROM, NES RAM bytes, screenshots, licensed art/audio or checkpoint are included in this source, patch, or QA archive. All private proof files are kept outside these archives.
