# GK1 EXEC0194 — Source Implementer and Hostile QA Handoff — 2026-11-11

**Game:** Genghis Khan I original English NES behavior ported into the **existing** native 1280×720 C11 Windows Win32-XInput/Linux X11 application. This is not ROTK II or the original KOEI-inspired Windows game. The direct predecessor is **complete EXEC0193** (not EXEC0076 or a separate prototype). Executable version **1.94.0-dev1**.

## Exact hands-on change

**Fixed a player-facing controller-only Classic storage dead end.** The inherited authenticated original checkpoint importer can enter real archived/fresh first-Spring RAM and its Classic ASSIGN allows three manual-derived labor orders. But prior to this release, entering the 14-command `OTHER` private-save submenu after any ASSIGN action was rejected; additionally the native Xbox/X11 controller shell rejected the private save callback from the OTHER screen. The existing SHA-guarded `GC19` source-attested save/cold-load therefore could not be reached via OTHER with actual unpassed labor orders. No original NES command mechanics or values were fabricated.

Now `gk194_classic_storage_allowed()` permits authenticated Classic/assigned states only when the original raw 83-byte source remains exactly immutable, the projection remains exact, and the complete labor history replays successfully (`gk187_intact`). It explicitly refuses Enhanced phase/stages and corrupted source or forged actions. In `gk1_app_state.c` the actual controller command OTHER and its Save/Load confirmation dialog use this guard; in `gk1_native_shell.c` the one-shot storage request reaches the already implemented exclusive-create `gk189_save_new` GC19 writer, and title Y can cold-load it. The port-only Classic Y Undo and Enhanced profile remain untouched. Existing Save/Load calls still refuse overwrite and corrupted hash. The specific input route in the synthetic controller shell is **MENU → DOWN → A → A (one labor) → B → RIGHT → five DOWN → A (OTHER) → A (ask save) → A (confirm) → reboot-to-title → Y**. This is NOT stock NES OTHER semantics.

New code: `gk194_classic_storage_gate.c/.h`, `test_gk194_classic_storage.c`, `README_EXEC0194.md`. Updated actual app source, native shell, CMake and previous inherited version contract tests. Added dedicated shell option `--smoke-classic-storage-original <PRIVATE_AUTHENTIC_EVIDENCE> <frames>` for source-positive *simulated* controller path without packaging source. This flag is a QA-only entry point, not a user-first-Spring generator.

## Executed evidence

**VERIFIED_STATIC:** Original USA NES ROM independently rehashed privately SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; original 34-member evidence ZIP independently rehashed SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`. Both remain PRIVATE in original Genghis research folder, neither in public source or Collaboration artifacts. Current 613/613 code/test/packaging files reproduced byte-identically from exact original EXEC0193 plus attached `patch -p1`, with 15 changed/new files and **zero old source files removed**. Historical archive preserves **43,875 original EXEC0193 ZIP members with payload/CRC equality**, and appends 613 full current entries: 44,488 total. ZIP CRC scan PASS. All four inherited image file entries are *existing native-port QA screenshots*, not new ROM assets.

**OBSERVED_RUNTIME_STOCK_FRESH_RESET vs ARCHIVED_RESTORED:** No new actual stock NES emulation was performed. Previously observed fresh original Mongol Spring1175 WorkRAM: Gold430/Food208/Troops61/Rate121/orders3. Distinct archived-restored original Mongol Spring1175 SaveRAM: Gold439/Food208/Troops61/Rate101/orders3; archived World Spring1206 SaveRAM: Gold193/Food332/Troops116/Rate114/orders3. The first is *not* an unconditional initialization constant. The correct bank is confirmed before import. All positive private tests used these separately authenticated private sources.

**TESTED_PORT_LINUX:** CMake/Ninja Release **157/157 CTest PASS**, Debug **157/157 PASS**; focused code/test **221/221 assertions PASS** with all three original authentic profiles separately in Release and Debug; focused ASan+UBSan **221/221 PASS**. All three sources executed actual app via native Linux X11/Xvfb **120 frames** *each* using simulated controller-pad actions through the actual native shell: Assign → Other → double-confirm → GC19 saved → program reset/title Y cold-load, all success. Separate normal 120-frame graphical execution PASS. The DEB was generated, extracted, inspected (Version 1.94.0-dev1), and extracted binary passed normal 120-frame and private original fresh path 120-frame pad/save/title-load smoke. The private per-profile GC19 test artifacts were retained only privately in this execution workspace and were not published. This is not a physical Xbox controller or installed Windows validation.

**NOT RUN:** Win32 binary, Windows NSIS installer, physical Xbox controller, new Mesen original stock reset, actual original first-Spring independent state generation, enemy AI/autonomous turns or 14/27-country completed campaigns.

## Reproduction commands

```bash
# With exact historical EXEC0193 tree, reproduce only changed source/files:
patch -p1 -d execution_193_copy -i GK1_EXEC0194_Complete_Integration_From0193.patch
# Check 613 exact current file SHA values in GK1_EXEC0194_Source_SHA256_Manifest_v1.00.json.
cmake -S execution_194 -B build194 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build194 -j6
ctest --test-dir build194 --output-on-failure -j6
# Use matching source-authentic templates installed as in existing source.
# Owner must supply original private fresh/archived data; not contained in ZIP.
build194/test_gk194_classic_storage /PATH/EXACT-ORIGINAL-41-COUNTRY-CSV /PRIVATE/fresh_14_after_wait.json /PRIVATE/stock_spring1175.mss /PRIVATE/stock_world_spring1206.mss
GK1_CLASSIC_COLD_SAVE_FILE=/PRIVATE/TEMP_NEW_SLOT.gc19 xvfb-run -a build194/gk1_native_shell --smoke-classic-storage-original /PRIVATE/fresh_14_after_wait.json 120
```

No source extraction/verification procedure requires original licensed ROM to be included in the copyright-clean source archive; checks under the original private authority only.

## Negative controls and risks

`gk194_classic_storage_allowed` refuses uninitialized world, Enhanced profile, any tampering to immutable checkpoint, forged mutable Classic country Gold, and missing source verification. After a forged Classic state the menu does not open OTHER and a forcibly injected storage screen cannot commit a request. Three Profile-specific positive tests preserve raw originals, exclusive GC19 existing-file refuses overwrite, and a single-byte file mutation yields `GK189_CORRUPT` with no target-app mutation. A Cancel after initial Save confirmation does not write. Linux ASan+UBSan focused gate test reports no issues.

The main unresolved fidelity milestone is not disguised by this fix: **the port cannot generate original first-Spring setup/difficulty/character RNG or all 14/27 source-authentic transforms independently**. Player currently imports 3 private first-Spring source checkpoints and uses manual-derived Classic labor/Pass rather than full stock original commands. Trade merchant RNG/presence, autonomous AI, diplomacy, dynasty (daughter ages6/7/8), battle resolution/aftermath, source-authentic original media, Win32 packaging and full campaigns still OPEN. Maintain Classic/Enhanced separation and all inherited code/owner-private sources.

## Deliverables and exact SHA256

| Artifact | Bytes | SHA-256 |
|---|---:|---|
| GK1_EXEC0194_Linux_Research_Installer_1.94.0-dev1_amd64.deb | 315132 | `a00500afb1df40d7dcdfe7dbfd4e312359935228e0b78ff3d4b33cb9791aed1d` |
| GK1_EXEC0194_Complete_Historical_C11_Source_v1.00.zip | 69596315 | `f415aa15bb3c117882e538160c0f8ba35825fcff4b22005581514cf98d0ce9fb` |
| GK1_EXEC0194_Copyright_Clean_Source_Delta_v1.00.zip | 132275 | `737cc8644e23c174e5e6cb0bedba032685e042ee33155a5d28e991dcfa64206d` |
| GK1_EXEC0194_Linux_QA_Source_Manifest_v1.00.zip | 64606 | `b8920bf8ff1423e96310c91a9d55f7319cbce338ae67cbd9f52db8eb5cf05047` |
| GK1_EXEC0194_Complete_Integration_From0193.patch | 40069 | `372225d5aa305135c43f3f9e64e47dd167f9ea215a8b4bfa9cf8a25eb6b69618` |
| GK1_EXEC0194_Source_SHA256_Manifest_v1.00.json | 61249 | `9756376fbfb0e80668a44f89e8434009e8860356c9555a5b759baec72e1ee443` |

**Publication and acceptance:** Upload to the verified Collaboration parent `1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT` and copyright-clean public GitHub delta. Original ROM/checkpoints/savestates stay privately in Genghis research folder `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. Publication is not another agent's acknowledgement, physical-device QA, Windows acceptance or Director-certified release. Preserve hourly development until explicit stop or certified release.
