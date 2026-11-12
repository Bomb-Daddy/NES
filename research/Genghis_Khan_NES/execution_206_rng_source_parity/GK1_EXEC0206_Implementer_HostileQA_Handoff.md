# GK1 EXEC0206 — RNG source-parity bridge and hostile QA handoff

**Inheritance:** Exact complete EXEC0205 `execution_199` C11 game, version metadata unchanged `1.99.0-dev1` pending Director acceptance; 0206 is an uncertified research candidate. Not an isolated prototype. Original ROM stayed PRIVATE (Genghis research folder). Destination verified: Google Drive `/Retro Gaming/Collaboration` parent ID `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`.

## Executed original-source arithmetic, not a stock gameplay emulation

The existing C11 application now integrates `gk206_rng_source_parity.c/.h` into `gk1_app_state`, exposes `--audit-stock-rng-parity PRIVATE_ORIGINAL_NES`, and adds compiled `test_gk206_rng_source_parity`. It loads six confidential coefficients from SHA-authenticated private original ROM bank 15, CPU `$C2EC` and compares the original `$C289` 6502 6-byte multiply-and-negate arithmetic (`gk178_step`) with the native `gk137` 48-bit form for **512 independent seed cases × 4 chained draws = 2,048**; it also compares each of 5 source-modeled Spring rate difficulty expressions, **2,560** additional pairs. **All 4,608 comparisons agreed with zero mismatches.** Crucially, testing arbitrary chosen seeds does NOT establish the real NES seed, original reset sequence, number/timing of RNG draws, or first playable Spring. The initial country Gold430/Rate121 fresh and Gold439/Rate101 archived remain separate Mesen observations; neither is hardcoded or inferred from this test.

Input hardening: fake/missing ROM is refused before reading coefficients; deliberately corrupted coefficient input returns mismatch and does not commit output; argument failures/failed source checks preserve outputs. Source coefficient bytes are not logged, packaged, published, or kept in receipt; only counts and authenticity digest. App campaign state is not modified. Original ROM/private snapshots and licensed graphics are excluded from public GitHub and copyright-clean source packages.

## Tests performed

- CMake/Ninja Linux Release **169/169 PASS** and Debug **169/169 PASS**.
- Independent clean re-extraction of EXEC0205 ZIP + exact `git apply` patch; **all current source paths matched**, independent Release **169/169 PASS**.
- Original stock SHA `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` authenticated locally and compared in native Release, sanitizer Debug, and Debian-extracted source execution, without exporting ROM bytes.
- Focused AddressSanitizer/UndefinedBehaviorSanitizer **14 assertions, 4608 numeric pairs PASS**.
- Counterfeit ROM mutation rejected by actual shell exit code **8**; deliberately invalid coefficient input rejected. No world transition was asserted.
- Actual 1280×720 native X11 120-frame graphical smoke PASS and extracted Debian wrapper 120-frame PASS; `.deb` wrapper source-RNG audit PASS. These exercise synthetic/host software controls, not a physical Xbox.

## Reproduction

```bash
unzip GK1_EXEC0205_Complete_Integrated_C11_Source_QA.zip -d source
cd source
git apply /path/GK1_EXEC0206_CopyrightClean_Integration.patch
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build --output-on-failure -j8
export GK1_ORIGINAL_STOCK_ROM=/private/authorized_exact_original.nes
./build/test_gk206_rng_source_parity
./build/gk1_native_shell --audit-stock-rng-parity "$GK1_ORIGINAL_STOCK_ROM"
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
```

The source test runs its negative controls when no private ROM is attached; positive original-ROM checks require `GK1_ORIGINAL_STOCK_ROM`. Never add the ROM to a public bundle.

## Explicit classification and next implementation

**VERIFIED_STATIC:** original ROM SHA, authenticated source RNG coefficient arithmetic parity across 4,608 comparisons, clean patch replay and corrupt-source refusal.
**OBSERVED_RUNTIME_STOCK:** only the inherited fresh WorkRAM and archived SaveRAM original Mesen captures. No new original Mesen emulator session this run.
**TESTED_PORT_LINUX:** Release/Debug/clean-build CTest, ASan/UBSan focused regression, native and Debian-extracted X11 frames and CLI parity completed.
**NOT RUN:** Win32, NSIS, physical Xbox controller or full campaigns. No original live first Spring independently generated.

The active binding gate is **actual original stock NES CPU reset/setup**: instrument Mesen RAM `$0084..$0089` seed values and actual `$0050` selector/BRK invocation during the original UI setup; then feed observed seed/draw provenance into the C11 entry path. Existing bounded CPU stops at unknown power-on `$00F5` (unspecified mode) or PPUSTATUS `$2002` (documented Mesen AllZeros mode). Do not invent either value, bypass `PRE_SETUP`, or pretend a numeric RNG regression achieves playable Spring. After that finish authentic merchant presence, AI, battle aftermath, dynasty, diplomacy, Win32 installer, full campaigns and physical controller acceptance.

**Existing hourly development schedule remains enabled.** This published handoff does not prove another agent acknowledged or executed it, or constitute a Director-certified release.
