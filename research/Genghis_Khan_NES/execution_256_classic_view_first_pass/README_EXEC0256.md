# GK1 EXEC0256 — Classic VIEW first-action PASS source integrity

Date 2026-11-13. The latest checked predecessor was EXEC0255, not stale EXEC0076/EXEC0062.

## Executable correction

In authentic original-stock Classic Spring, pressing A twice while browsing VIEW without any previous Tax/Assign action **previously fell through to `gk119_pass()`**, which altered the original checkpoint's three-order receipt instead of the port's authenticated mutable Classic ledger. The 14-command PASS was separately fixed in EXEC0254, but VIEW-A still retained the bug. EXEC0256 adds one `classic_ui_pass` function and routes both controller surfaces through it. First A only arms a cancellable prompt, B cancels without opening the ledger, second A creates/replays an authenticated derived `gk187` Classic ledger then passes its remaining orders. Original 83-byte country records, three original orders, scenario identity and SHA remain unchanged. Tax/Assign->VIEW->PASS uses the same derived ledger. No fake NES next season is generated.

## Evidence and status

- VERIFIED_STATIC: Exact complete EXEC0255 source ZIP SHA, new 3-file integration patch, source SHA manifest. Historical original Mesen stock evidence was sourced privately, never published.
- TESTED_PORT_LINUX: Complete Release C11 build and 218/218 CTest PASS. Native 1280×720 X11 15-frame smoke PASS. Linked C11 test against private three authentic source profiles (fresh Mongol WorkRAM Spring1175, archived Mongol SaveRAM Spring1175, archived World SaveRAM Spring1206) reports 262 assertions PASS, including cancel, A-A, cold receipt and Tax->VIEW->PASS. Full Debug build completed and new target test passed; full Debug suite ended early during serialized graphical tests and is not certified.
- RED negative control: identical test on old unmodified EXEC0255 `gk1_app_state.c` fails on original country VIEW A-A at `classic_assign.active==1 && passed==1`, proving the bug before the fix. Negative test must fail, not be misreported as a successful CTest.
- OBSERVED_RUNTIME_STOCK_FRESH and OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: original historical evidence reused; NO new Mesen run.
- NOT RUN: native Windows/MSVC/NSIS for EXEC0256, physical Xbox, independent original first Spring setup/RNG, full campaigns.

## Reproduction

```
sha256sum GK1_EXEC0255_Complete_Inherited_C11_Source_QA_v1.01.zip
unzip -q GK1_EXEC0255_Complete_Inherited_C11_Source_QA_v1.01.zip -d inherited
cd inherited
patch --dry-run -p1 < ../GK1_EXEC0256_Integration_From0255.patch
patch -p1 < ../GK1_EXEC0256_Integration_From0255.patch
sha256sum execution_199/gk1_app_state.c execution_199/CMakeLists.txt execution_199/test_gk256_classic_view_first_pass.c
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release -DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON
cmake --build build -j12
ctest --test-dir build -j8 --timeout 30 --output-on-failure
xvfb-run -a ./build/gk1_native_shell --smoke-frames 15
```

Opt-in authentic positive test requires the owner-private original canonical CSV, fresh stock JSON and two archived original Mesen MSS files, passed as arguments `(CSV, FRESH, MONGOL_ARCHIVED, WORLD_ARCHIVED)` to `test_gk256_classic_view_first_pass`. Never include them in public archives. Public CTest runs fail-closed negative tests without original files.

## Unfinished scope

Original recovered ROM is NOT missing; stock SHA `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`. Source-authentic first Spring ruler/character/difficulty RNG, subsequent seasons, merchant RNG, AI, diplomacy, dynasty, battle aftermath, original graphics/sound, and full Windows Xbox controller campaigns remain incomplete. Preserve existing 14/27 source templates, profiles and all prior evidence; do not infer original NES action parity from Linux port tests. This source publication does not acknowledge reading/executing by other agents. Recurring job remains enabled.
