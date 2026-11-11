# GK1 EXEC0204 — authenticated original-stock $0050 store candidate inventory

**November 11, 2026 — C11 native integration DEVELOPMENT CANDIDATE.** The authoritative predecessor is **EXEC0203 exact source recovery**, reconstructed from the 631/631 verified historical EXEC0199 source. This adds original-source-path tracing, **not** a new first-Spring generator or a certified game release. Metadata intentionally stays `1.99.0-dev1`.

## Actual new implementation

Added `gk204_selector_store_inventory.c/.h` and focused C11 regression test, integrated into the same `gk1_app_state` library and `gk1_native_shell --audit-stock-selector-store-candidates PRIVATE_AUTHENTIC_ORIGINAL_NES_PATH`. The app-level accessor refuses non-AWAIT_SOURCE stages. A private 262160-byte original-stock SHA256 gate occurs before byte-pattern inventory, and rejection leaves caller output byte-identical. No ROM, SRAM, copyrighted screens, graphics, sound, or ROM byte dumps are published.

**VERIFIED_STATIC findings from the *authenticated stock ROM***: 21 source-byte-pattern candidates touching selector zero-page address `$0050` or indexed/absolute aliases: **9 direct store patterns**, **5 direct read/modify/write patterns**, **6 indexed operand candidates** (not confirmed to address `$0050` at runtime), and **1 absolute address pattern**. Six raw candidates occur in hypothetical bank 10; the two fixed-bank `STA $50` candidates at `$EF3F` and `$EF4D` have adjacent immediate values **23 decimal** and dispatch to `$C419` per EXEC0203, **not RNG slot 17 decimal**. No adjacent immediate `LDA #17; STA $50` candidate exists in this exact full image. These are *raw source patterns*, potentially data or unreachable code. The addresses for switchable banks are only hypothetical CPU window aliases, not verified actual mapper states. Do not label this as demonstrated NES execution, authentic setup, merchant presence, or first-Spring RNG progression.

**OBSERVED_RUNTIME_STOCK:** no new Mesen runtime here. Inherited original observed fresh WorkRAM Spring 1175 Gold430, Food208, Troops61, Rate121 and archived-restored SaveRAM Mongol 1175 Gold439, Food208, Troops61, Rate101 and World 1206 Gold193, Food332, Troops116, Rate114 remain separate and are **not constants**.

**TESTED_PORT_LINUX:** Release 167/167, Debug 167/167 and independently clean-patch rebuilt Release 167/167 CTest; focused original private-stock analyzer **32 assertions** PASS under native Release and AddressSanitizer/UBSan; inherited GK202 40 and GK203 30 authenticated source assertions PASS. Native application Xvfb 1280x720 120 frames and extracted Debian executable with correct wrapper override 120 frames PASS. Original ROM and corrupted/truncated negative-control files were never placed in publication. Counterfeit source CLI exits 1, verified. Exact 640 current non-generated source/QA files were matched between directly edited source and clean extracted EXEC0203 plus new patch (ignoring one nondeterministic Python `.pyc` cache).

**NOT RUN:** native Windows Win32 binary, NSIS installer, physical Xbox controller, new original NES Mesen execution, complete campaigns. No new gameplay-season/difficulty/character RNG formula was established, so main Director M1 first playable Spring gate remains OPEN. Merchant RNG, AI, diplomacy, dynasty, aftermath, original media, full campaigns, Windows/Xbox release likewise OPEN.

## Reproduce

```sh
# Extract GK1_EXEC0203_Exact0199_Integrated_Reproducible_C11_Source_QA.zip and retain sibling execution_034/035/039/040/045/047/049.
# Apply the EXEC0204 patch at the parent containing execution_199/.
git apply GK1_EXEC0204_Canonical0203_to_Integrated204_CopyrightClean.patch
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j6
ctest --test-dir build -j6 --output-on-failure
./build/test_gk204_selector_store_inventory /PRIVATE/authenticated_original.nes
./build/gk1_native_shell --audit-stock-selector-store-candidates /PRIVATE/authenticated_original.nes
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
```

## Source and privacy gates

- Authenticated original stock USA ROM original SHA256: `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` (remains in private Drive folder; not a public artifact).
- Original private fresh+archived evidence ZIP SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a` (kept private).
- Integration patch SHA256: `3fab1ba53f55f24461f00ed6ed759fc39a3a86ad8641e7933594bf3583e86311`.
- Exact combined source manifest is in the attached ZIP. The official historical EXEC0199 ZIP remains unchanged in its Drive location. Publication is not external agent read/execute acknowledgement or Director acceptance.
- Next executable target: instrument actual stock Mesen `$0050` reads/writes, BRK entry selector and bank state during *both* fresh and archived conditions, then use those paths to implement original setup/season/three-order state machine without hardcoded observed Gold/Rate. Do not repeat settled 103 manual rules.
