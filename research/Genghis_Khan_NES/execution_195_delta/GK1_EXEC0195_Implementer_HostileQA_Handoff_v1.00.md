# GK1 EXEC0195 — Implementer and Hostile QA Reproducible Handoff — 2026-11-11

**Source:** Genghis Khan I USA original NES-to-Windows/Linux native C11, existing 1280×720 controller-first application. This is not the ROTK II NES/DOS nor original KOEI-inspired separate strategy game. Latest predecessor actually located in current Drive and GitHub: **EXEC0194 v1.94.0-dev1**. Full inherited code integrated as **EXEC0195 v1.95.0-dev1**, not a forked demonstration and not stale EXEC0076.

## 1. Actual executable change

The existing authentic-checkpoint-only Classic ASSIGN screen allocates workforce from original imported 83-byte country records into a separate mutable, *manual-derived* Classic model. **Bug before this change:** subsequent Classic country VIEW displayed original workers instead of the actual allocation, and pressing A twice from VIEW invoked the wrong `gk119_pass` on the separate stock checkpoint's budget, not the active Classic three-order worker action ledger. Consequently VIEW and PASS could misrepresent the live player's state and spend an incompatible order budget.

**Fixed in linked game C11:** `gk195_classic_country_view` reads/validates the exact source (`gk85_raw_source_intact`), original numeric projection (`gk88_stock_projection_intact`) and action-history replay (`gk187_intact`), then yields *copied* source original plus derived active worker allocations, effective live remaining orders, action count and passed state; other countries remain immutable read-only stock records. The existing actual native controller VIEW double-confirm A now performs `gk187_pass` on the correct active Classic state; source checkpoint remaining orders and all ROM-derived records stay unchanged. Invalid source/projection/replay, forged data and wrong profile are refused. Enhanced BP/Rest/Demob and existing save/cold-load are preserved. This is an explicitly PORT-ONLY repair, NOT original NES stock command parity.

Actual changed/new C11 files: `gk195_classic_view.c`, `.h`, `test_gk195_classic_view.c`; modified linked `gk1_app_state.c`, `gk1_native_shell.c`, `CMakeLists.txt`, packaging version/contract tests and desktop launcher; `README_EXEC0195.md`. The shell adds `--smoke-classic-view-original <PRIVATE_AUTHENTIC_SOURCE> <frames>` with synthetic pad input and Xvfb to exercise integrated menu route; not an actual Xbox test.

## 2. Verification receipts

- **VERIFIED_STATIC:** complete source exact original EXEC0194 613 files plus 4 new files; **617/617** exact patch replay, **15** new/modified file paths, no inherited deletions. Independent source copy built from patch (not copied resulting binary), CTest 158/158 PASS. Historical source ZIP carries **44,488** prior entries with each earlier payload byte-identical, appends 617 current files = **45,105 total**. Whole ZIP CRC PASS, no `.nes`, `.mss`, `.srm`, `.rom`, `.gc19` or licensed original gameplay. Patched source is the same current native executable, not a standalone proof-of-concept.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET (previous evidence, no new Mesen run):** original USA NES Spring1175 Mongol WorkRAM country1 Gold430 Food208 Troops61 Rate121 orders3. The different archived histories must not be used as constants.
- **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED (previous evidence, no new Mesen run):** original USA NES Mongol Spring1175 SaveRAM Gold439 Food208 Troops61 Rate101 orders3 and World Spring1206 SaveRAM Gold193 Food332 Troops116 Rate114 orders3.
- **TESTED_PORT_LINUX:** CMake/Ninja Release **158/158 CTest PASS**, Debug **158/158**, independent clean patch Release **158/158**, focused C test **150 original source assertions PASS** with all three private authentic profile inputs and deliberate corruption negative controls; focused ASan+UBSan **150/150** PASS. Native 1280×720 X11 via Xvfb simulated gamepad player ASSIGN→VIEW→PASS→PASS in fresh Mongol1175, archived Mongol1175 and World1206 **120 frames each PASS**. Debian package 1.95.0-dev1 exact version, extracted normal 120-frame graphical run PASS and authentic fresh source view/pad PASS. An unchanged source checksum/stock order budget checked before and after derived Pass.
- **NOT RUN:** actual native Win32 executable, Windows installer NSIS, physical Xbox controller, new headless original Mesen execution, full campaigns; these are distinct from synthetic controller tests.

## 3. Clean reproducibility

Start from the **complete EXEC0194 historical archive**, extract both `execution_194/` and its inherited sibling `execution_049/` and other execution siblings. This code's existing CMake references sibling original-source tables. Do not build isolated source without inherited siblings.

```bash
cp -a execution_194 execution_195
(cd execution_195 && patch -p1 --batch < /path/GK1_EXEC0195_Complete_Integration_From0194.patch)
cmake -S execution_195 -B build195 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build195 -j8
ctest --test-dir build195 --output-on-failure -j8
# private positive tests require own original authorized sources; NEVER distribute them
build195/test_gk195_classic_view build195/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv /PRIVATE/fresh_14_after_wait.json /PRIVATE/stock_spring1175.mss /PRIVATE/stock_world_spring1206.mss
xvfb-run -a build195/gk1_native_shell --smoke-classic-view-original /PRIVATE/fresh_14_after_wait.json 120
(cd build195 && cpack -G DEB)
```

Original ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` and original 34-member source evidence ZIP SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a` were independently checked in PRIVATE workspace. This user-owned ROM and raw states are never in public GitHub or Collaboration packages. Fresh original state lives in WorkRAM physical 0x0DCD, archived restored states in SaveRAM physical 0x0DCD. Goods offset original record+0x22, not +0x24. No new primary manual re-audit.

## 4. Hostile QA and open release blockers

Invalid, unverified and source-tampered checkpoints refuse VIEW, especially forged Classic action history. Source records remain byte-identical throughout allocated-worker VIEW and Pass; Classic action budget is charged only in the derived mutable model. An Enhanced profile is not promoted to Classic. The current native PC build still imports authentic original first-Spring checkpoints rather than independently generating all original NES first-Spring setup/difficulty/character RNG transformations for all 14/27-country campaigns. Three actual original Classic strategic commands are not complete; trade merchant RNG, autonomous AI, dynasty (daughter ages6/7/8), diplomacy, battle terminal/aftermath and source-accurate graphics/sound, campaign-complete cold-save and Windows/physical acceptance remain open. No claim of full playable release or other agent's acknowledgement.

## 5. Artifact integrity

| Exact artifact | Bytes | SHA256 |
|---|---:|---|
| `GK1_EXEC0195_Linux_Research_Installer_1.95.0-dev1_amd64.deb` | 317,494 | `86513ca7f08a02d5e77bd040c594c59c0b556add5ad87a05f8853bbc3b06f41d` |
| `GK1_EXEC0195_Complete_Historical_C11_Source_v1.00.zip` | 70,601,838 | `a12126108d002a195cb4af9a44d128e289d102dd07130d4b1e89b894c4b7f84b` |
| `GK1_EXEC0195_Copyright_Clean_Source_Delta_v1.00.zip` | 136,637 | `8027377aacbcd623b897bf6a425b0b7d28df85460c754388d2a4a715db16a37b` |
| `GK1_EXEC0195_Complete_Integration_From0194.patch` | 41,236 | `45d1bc70ca96f445c5084d97f8380b10bb314670a545989afb298ba89d857a0c` |
| `GK1_EXEC0195_Source_SHA256_Manifest_v1.00.json` | 88,625 | `cc853f8eb2e7ce13a335cc24d1b0c9931bd256840db10cf41f4716ceb17eb5f8` |

Copyright-clean GitHub delta only; publish this handoff/ledger, manifest, full source and Debian package to verified Collaboration parent `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT`. Keep original ROM/raw snapshots in private Genghis research folder `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. Do not disable recurring development until Director stops or complete release certified.
