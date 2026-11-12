# GK1 EXEC0211 — authenticated original-stock fresh-reset event ordering

**Predecessor:** exact EXEC0210 complete native C11 research game. All inherited code, test modules, controller UI, Classic/Enhanced separation and historical source are preserved. This is an incremental executable investigation, not a new game or a certified release.

## Executable implementation

`gk211_fresh_sequence.c/h` validates all fourteen distinct **previously archived** authentic original-USA-Mesen fresh-reset observation JSON files, checking a complete SHA-256 for every file before interpreting it. It requires the actual CPU-visible `nesMemory` statistics to equal physical `nesWorkRam` in all fourteen captures and independently verifies that `nesSaveRam` is blank at each stage. The actual private original 83-byte records in final stage 14 must ALSO pass the inherited `gk165_extract_fresh_json` digest/source check.

The new same-app `gk1_app_original_fresh_sequence()` gate verifies the canonical original 41-country CSV, the untouched Mongol PRE_SETUP native world, Classic mode and absence of a different imported stock checkpoint. Only then is the read-only source chronology returned. No game world, controller state, save state, profile, player orders or country records are changed.

The program identifies the **observed** event boundaries: at capture 2 a 14-country 1174 template is visible; at 9 Food 129→258; at 10 year 1174→1175 and Rate 0→121; at 12 Gold 425→430, Food 258→208, Troops 60→61; at 14 actual 3 orders appear. Those are **one source input/controller/RNG history**, not the universal original NES Spring transformation or original RNG algorithm. Earlier EXEC0114 had only a 6-stage compressed receipt; EXEC0211 binds the entire 14 raw source JSON captures and their distinct RAM banks and CPU frame ordering.

## Reproducible tests

- Configure and build existing native executable: `cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build -j8`.
- Run complete native tests: `ctest --test-dir build -j8 --output-on-failure`.
- Public fail-closed testing, no copyrighted input: `build/test_gk211_fresh_sequence`.
- Private original fourteen-stage verification, ONLY in private workspace: `build/test_gk211_fresh_sequence /private/fresh_dir execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv`.
- Hostile tamper: copy all fourteen files to a PRIVATE temporary directory, flip one byte in `fresh_12_after_wait.json`, and pass the temporary directory as the third argument. The complete file SHA gate must reject it without changing the caller report.

Raw original stock ROM, Mesen screenshots, audio, savestates, 14 JSON observations and 14×83 source bytes are owner-private; public GitHub and ZIP carry only general C source, test source, names and SHA-256 fingerprints.

## Scope and evidence labels

**VERIFIED_STATIC:** source identities and 14 authentic pre-existing original observation receipts checked, with distinct WorkRAM/SaveRAM and event-order witness. **OBSERVED_RUNTIME_STOCK_FRESH_RESET:** only the captured November 7 original run; this execution did NOT run original Mesen anew. **TESTED_PORT_LINUX:** C11 test/rebuild and GUI smoke, if listed in the accompanying execution receipt. **NOT RUN:** native Windows, NSIS, physical Xbox controller, original complete campaigns. Independent, variable-RNG new first Spring generation for all 14/27 countries remains OPEN. Trade merchant scheduling, autonomous AI, battle terminal aftermath, diplomacy, dynasty, authentic art/sound and full release acceptance likewise OPEN. Keep hourly development until Director stops or release is complete.
