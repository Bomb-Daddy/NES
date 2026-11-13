# EXEC0245 — Cross-platform authenticated owner Spring startup (2026-11-13)

## Change performed in the inherited, complete C11 game

Started from exact EXEC0243 complete source ZIP and reproduced authenticated EXEC0244 static-CRT CMake postimage SHA256 b288693a8e83056d2e35295792bf9dd487c6e6d0286308b697013b029bc6bdd2. No previous source was discarded. Source added `gk245_owner_source.c/.h` as a common, fail-closed stock owner checkpoint startup for BOTH real WinMain and the interactive Linux `main`. Previously only the Windows entrypoint honored `GK1_OWNER_STOCK_FRESH_JSON` and `GK1_OWNER_STOCK_WORLD_MSS` during normal startup; Linux normal startup omitted both. The original startup source gate was moved to the common function without changing the existing Win32 precedence/visibility.

On Linux normal interactive launch, explicitly owner-selected original fresh-reset WorkRAM JSON or archived World SaveRAM Mesen MSS now enters the existing source-attested Classic first-Spring checkpoint screen. It does NOT generate first Spring, does NOT claim all original stock setup/RNG parity, does NOT mutate unverified original loaded templates, and does NOT access owner's file on command-line test/diagnostic modes. The two options are exclusive. Source failure preserves the app before mutation, explicit diagnostics refuse owner file IO, and the original stock 14/27 country templates remain unchanged.

## TESTED_PORT_LINUX

CMake Release + zlib, full inherited source compiled to actual 1280x720 Linux X11 app; `ctest --test-dir ... -j12 --timeout 35 --output-on-failure` => **209/209 PASSED, exit 0**. New real source-linked C11 test covers absent source, fake source, both-source conflict, smoke isolation, wrong-phase/precondition, atomic state preservation and 14/27 pre-setup integrity. Independently invoked optional **PRIVATE stock archived 1206 World Mesen MSS** as an owner-only test argument (outside source ZIP) and obtained **exit 0** with imported World 27 countries, year1206, authenticated source proof, three commands, preserved source templates. This is TESTED_PORT_LINUX real existing native source/import library, NOT an emulator run and NOT an original first-Spring generator. Private original sample was extracted from Director-supplied EXEC0076 private archive, whose SHA256 was verified exact. Test output logs included no licensed bytes or ROM.

## VERIFIED_STATIC, historical and NOT RUN

VERIFIED_STATIC: source replay from complete inherited 0243 plus exact 0244 SHA-gated patch, full source zip includes all prior 949 baseline entries and the new files; 5 source SHA256 values stored separately; no original cartridge, Mesen states, pixel/audio files, or user save files packaged. OBSERVED_RUNTIME_STOCK_FRESH_RESET and ARCHIVED_RESTORED remain the pre-existing independent original Mesen observations, not new runtime evidence. EXEC0245 native Windows binary/NSIS was **NOT RUN**. EXEC0244 Windows static-CRT CI (10/10 tests) remains historical verified evidence, not promoted to new Windows run. Physical Xbox and two complete campaigns **NOT RUN**.

## Reproducibility

1. Extract EXEC0243 complete source ZIP; apply the existing verified EXEC0244 `apply244.py` once (CMakelists SHA256 b288693a8e83056d2e35295792bf9dd487c6e6d0286308b697013b029bc6bdd2).
2. Apply `GK1_EXEC0245_Integration.patch` from the `source/` directory (`patch -p1 < ...`). The patch never contains private ROM or Mesen data.
3. CMake configure/build from `source/execution_199` (Release, zlib installed), then `ctest --test-dir build --output-on-failure -j12 --timeout 35`.
4. For owner's optional positive World proof only, pass private `stock_world_spring1206.mss` as second argument to `test_gk245_owner_source` with original 41-country CSV as first. Keep MSS private; do not upload it to GitHub/installer.

## Unfinished critical tasks

Independently execute source-authentic stock setup/ruler/difficulty/seed and first Spring processing in both 14/27-country games; complete actual 3 original order effects, merchant presence, autonomous AI, battle aftermath, diplomacy, dynasty, graphics/sound and owner physical Xbox campaign acceptance. Do not hardcode archived rate 114 or fresh Mongol rate 121 as universal. Keep established development automation running unless Director stops or full release genuinely qualifies.
