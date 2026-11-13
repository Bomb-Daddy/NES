# Genghis Khan I — EXEC0238 — exact inherited EXEC0237 owner-source startup isolation

This is a *source-implemented* modification to the existing complete cross-platform native C11 game, not a new prototype or a reconstruction of original NES first-Spring RNG. Public source contains no ROM, Mesen state, screenshots, capture JSON, or other owner-private/licensed data.

## User-visible behavior

- For normal Windows WinMain startup, explicit owner-only `GK1_OWNER_STOCK_FRESH_JSON` can import the SHA-authenticated original fresh Spring 1175 snapshot only if installed source CSV matches canonical 41-country identity and gameplay is pristine. This was already present in EXEC0237; EXEC0238 preserves it.
- An unset or empty variable leaves startup untouched. CI/smoke modes deliberately **do not read private owner files**, even if an environment variable was inherited. Failures report a refusal without creating a false original first-Spring state.
- The same source-pure startup gate is now separately testable via Linux native C11, with optional owner-private positive input. The WinMain integration uses that identical gate, avoiding divergence between a standalone validation function and the actual Windows startup.
- Historical checkpoint entry exposes the existing original 14-command UI and source-derived three-order receipt, NOT a new independently initialized Spring, original merchant RNG or complete original commands.

## Reproduce

1. Extract `GK1_EXEC0237_Complete_Inherited_C11_Source_QA.zip`; root of C11 source is `source/execution_199`.
2. Run `python3 apply238.py <root-containing-execution_199>` from this folder. Both full predecessor file SHA-256 digests and all five resulting file digests are enforced.
3. `cmake -S source/execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j8 && ctest --test-dir build -j4 --output-on-failure`.
4. Run `test_gk237_owner_fresh_win` and `test_gk238_owner_startup` with the built canonical CSV. The public tests exercise refusal and smoke isolation; owner-approved private JSON may be supplied as the optional second argument for positive replay.
5. `git apply --check GK1_EXEC0238_CopyrightClean_Integration.patch` from the directory containing `source/` before applying the patch; the Python script is the exact source-hash-gated canonical installer.

## Actual execution classification, 2026-11-13

- VERIFIED_STATIC: exact full EXEC0237 inherited source; 5/5 exact source overlay reconstruction; Git patch reverse check; original public EXEC0237 Git blobs matched; hostile repeated/tampered/corrupt predecessor controls fail closed.
- TESTED_PORT_LINUX: whole integrated C11 source compiled; Release 204/204 CTests passed; independent exact replay Release 204/204 tests; actual 1280x720 Xvfb 15-frame smoke passed. Private source input was loaded only for private standalone tests: prior EXEC0237 34 assertions PASS, new EXEC0238 25 assertions PASS; no raw captures published.
- OBSERVED_RUNTIME_STOCK_FRESH: historical original WorkRAM 14x83 record referenced as private oracle, not rerun in Mesen here.
- OBSERVED_RUNTIME_STOCK_ARCHIVED: historical original SaveRAM 14/27 evidence distinct, not rerun here.
- TESTED_PORT_WIN32: previous EXEC0236 native WinMain/NSIS passing run retains historic status. EXEC0237/238 Windows-specific new integration NOT RUN until CI succeeds; do not transfer past result.
- NOT RUN: physical Xbox controller, independent authentic first Spring, original 3-command program across 14/27 countries, complete campaigns.

Next priority remains stock Mesen CPU setup/RNG source instrumentation and real playable 14/27-country Spring; then complete Trade, AI, diplomacy, dynasty, battles, media and hardware certification.
