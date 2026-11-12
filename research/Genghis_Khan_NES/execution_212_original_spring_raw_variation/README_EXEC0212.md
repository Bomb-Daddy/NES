# GK1 EXEC0212 — exact-original two-history 83-byte raw variation; native C11

## Actual change

EXEC0211's complete native C11 application is retained. New `gk212_stock_raw_variation.c/h` and `gk1_app_original_raw_variation()` enforce original-stock SHA validation, immutable original 41-country templates, exact 14x83 original country data, correct WorkRAM-vs-SaveRAM profile, and decoded-versus-raw integrity. They compute aggregate per-offset/per-country history differences between **two genuine original Spring1175 runs**, without copying licensed byte values into the program or report. A new native CLI `--audit-original-mongol-variation <private_fresh.raw> <private_archived.raw>` returns an aggregate source-fidelity receipt. Failed checks neither change output nor mutate game or orders. No RNG chronology, start values or original future seasons are synthesized.

## Reproduce with private original source outside public repo

```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j6
ctest --test-dir build -j8 --output-on-failure
# Original ROM/mss/JSON must stay in private storage; do not commit them.
./build/gk1_native_shell --extract-original-fresh-json /PRIVATE/fresh_14_after_wait.json /PRIVATE/fresh.raw
./build/gk1_native_shell --extract-original-mss /PRIVATE/stock_spring1175.mss mongol /PRIVATE/mongol.raw
./build/gk1_native_shell --extract-original-mss /PRIVATE/stock_world_spring1206.mss world /PRIVATE/world.raw
./build/test_gk212_stock_raw_variation build/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /PRIVATE/fresh.raw /PRIVATE/mongol.raw
./build/test_gk112_stock_setup_deltas build/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /PRIVATE/fresh.raw /PRIVATE/mongol.raw /PRIVATE/world.raw
./build/gk1_native_shell --audit-original-mongol-variation /PRIVATE/fresh.raw /PRIVATE/mongol.raw
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
cd build && cpack -G DEB
```

## Result (2026-11-12)

VERIFIED_STATIC original-source comparison: 14*83=1,162 byte cells; 119 differ across both independently observed original histories, 23/83 raw byte offsets and 14/14 countries. Of the 24 previously selected numeric metrics, 16 vary by country and 93 country/metric pairs differ. No observed equality in only two histories is treated as a universal invariant. Original 27-country World capture remains separately verified as archived SaveRAM evidence, not compared directly against a 14-country scenario. Source profile SHA: fresh 04eb853683a3dada2a6a127baf61b6be2332114213cedc9e295c183647c6c73e; archived Mongol ba3f26ae278356a8e5f5b2ecb93ac1d576ea579fff3920bda2e52a05a764c328; archived World 0f9f254df49268a00cfb1599a5617364ba4511e2bb098da716dd845c1b1178e5.

Linux Release 175/175 CTest PASS; Linux Debug 175/175; focused private-original positive and negative assertions under ASan/UBSan PASS; native and Debian-extracted 1280x720 Xvfb smoke120 PASS; 7/7 independent patch-replay source paths byte-identical. Windows/NSIS/physical Xbox/full campaign NOT RUN. No new actual original Mesen session.

## Adversarial refusal and privacy

- Reject wrong profile/bank (archived versus fresh reversed), forged 83-byte raw byte in country14 even outside projected metrics, projected source value corruption, altered original template, missing canonical CSV; assert outputs unchanged and original game state byte-identical.
- Entire source distribution is copyright-clean. Original ROM `.nes`, savestates `.mss`, Mesen screenshots/JSON, extracted 14x83/27x83 RAM records stay ONLY in private Genghis folder. No licensed bytes on public GitHub.
- This is an uncertified 1.99.0-dev1 research executable. The central first-Spring generation problem, real merchant schedule, full AI/diplomacy/dynasty, battle end/aftermath, original art/audio, full Windows installer/Xbox acceptance remain open.
