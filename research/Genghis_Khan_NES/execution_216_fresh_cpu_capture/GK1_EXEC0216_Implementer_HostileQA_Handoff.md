# GK1 EXEC0216 — authentic original-stock fresh-reset CPU capture chronology / integrated C11 handoff

2026-11-12. Status: **UNCERTIFIED RESEARCH CANDIDATE, 1.99.0-dev1**. Source predecessor **EXEC0215** verified current in both Google Drive Collaboration and `Bomb-Daddy/NES` GitHub `research/Genghis_Khan_NES/execution_215_original_spring_gap` on this run. The actual inherited full game remains `execution_199/`, with eight linked historical source directories. No standalone prototype, no deletion/replacement of older source.

## New executable, source-integrated change

- Added `gk216_fresh_cpu_pc.c/.h`, `test_gk216_fresh_cpu_pc.c`; updated `gk1_app_state.c/.h`, native `gk1_native_shell.c`, `CMakeLists.txt` (seven changed/new paths).
- `gk216_verify_fresh_cpu_pc(PRIVATE_ORIGINAL_ROM,PRIVATE_FRESH_DIR,&out)` authenticates the director-authorized original stock ROM via existing `gk82_probe_stock_rom` and all **14 complete archived fresh original Mesen JSON files** via existing `gk211_verify_fresh_reset_sequence` (individual file SHA and WorkRAM==CPU RAM, rejecting empty SaveRAM and bad event ordering). Only then does it decode CPU **PC/A/X/Y/SP/PS/cycleCount** from the SHA-gated evidence. It crosschecks the cycles and changes against the independently authenticated source report. It refuses invalid CPU fields and never modifies the caller's output on any failure.
- An app-level `gk1_app_original_fresh_cpu_pc` enforces exactly installed canonical original pre-setup Mongol world, Classic profile, no imported stock checkpoint, and the AWAIT_SOURCE setup gate. It does not unlock orders or mutate campaigns.
- Real native binary CLI: `gk1_native_shell --audit-original-fresh-cpu-pcs PRIVATE_ORIGINAL_STOCK.nes PRIVATE_FRESH_DIR OBSERVATION_ONLY`. Prints 14 stage-end CPU PC snapshots and CPU cycles together with authenticated country-state changed masks; this is a read-only *actual executable* integration, not a static-only report.

## New ORIGINAL stock source observation (existing archived gameplay; NOT NEW Mesen execution)

Original Mesen captures (fresh reset) at frames 366, 491, 616, 741, 866, 991, 1116, 1241, 1366, 1666, 1966, 2266, 2391, 2691 record CPU PCs respectively `$F6E7`, `$C5BF`, `$F29F`, `$C5C2`, `$EFD7`, `$F6E5`, `$EFA6`, `$EFE1`, `$F0CF`, `$F434`, `$EFE1`, `$FA4C`, `$C2BF`, `$EFA2`. All samples fall in CPU upper program window `$C000..FFFF`; actual mapper bank at each capture is **not derived**. Stage09 was the food doubling observation; stage10 year/Rate update; stage12 original Gold/Food/Troops economic change; stage14 original three-order menu. **PC is merely the CPU address at end of a controller input capture; it is not the address of the relevant write, a seed store, or proof of an RNG dispatch**. Gold430/Rate121 is just one source-observed history, not a general first Spring.

Original archived-restored Mongo/World SaveRAM histories remain separate; neither is inferred from the fresh WorkRAM CPU trace. Source-authenticated original ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, length262160. Private original evidence ZIP SHA256 `8b757e11b12aeb3efe93e36c10628599ca5f12e91b15c02354cd3ebbd11bf52a`. **No ROM, licensed raw states, raw screenshots, original audio or sprite assets in these PUBLIC files.**

## Direct reproducible QA / negative controls

- Linux Release `cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`, `cmake --build build -j8`, `ctest --test-dir build -j8 --output-on-failure`: **179/179 PASS**.
- Linux Debug likewise **179/179 PASS**.
- INDEPENDENT clean-source ZIP extraction, `cmake`/`ninja`/`ctest`: **179/179 PASS**.
- ASan/UBSan compiled focused new test with actual private original ROM and original 14 capture JSON files: **PASS**. Production standalone native diagnostic with same private exact data: **14 accepted stages**, exit0.
- Hostile controls: tampering original stage10 CPU PC JSON (invalidating whole-file SHA), flipping one original ROM byte, and replacing `OBSERVATION_ONLY` with `NO_BYPASS` each rejected with native CLI exit9; no game mutation. Independent private focused test also checks source negative after success leaves `Gk216CpuReport` byte-identical (atomic failure).
- 120-frame native X11 real game rendering at 1280x720 **PASS**. 120-frame extracted Debian-installed launcher with explicit binary-path override and isolated HOME **PASS**.
- Copyright-clean exact integration patch independently `git apply --check` and `git apply` to prior full source; **7/7 changed/new paths match exact SHA** after patch replay.

Reproduce private positive (requires user-owned stock original, and original fresh JSON files in the same private directory; these files are never included in public ZIP):

```sh
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
./build/test_gk216_fresh_cpu_pc /PRIVATE/original-stock-usa.nes /PRIVATE/original-stock-fresh-json
./build/gk1_native_shell --audit-original-fresh-cpu-pcs /PRIVATE/original-stock-usa.nes /PRIVATE/original-stock-fresh-json OBSERVATION_ONLY
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
cd build && cpack -G DEB
```

## Required fidelity labels and priority

- **VERIFIED_STATIC:** Exact authenticated original ROM and 14 complete original stage JSON inputs, linked C11 source, all readback/patch tests. CPU address values are parsed from already-captured authentic ROM runtime, not statically determined program control flow.
- **OBSERVED_RUNTIME_STOCK—FRESH:** Existing archived authentic WorkRAM Spring 1175 capture, 14 stage-end CPU PCs, A/X/Y/SP/PS and cycles; **no new Mesen execution**.
- **OBSERVED_RUNTIME_STOCK—ARCHIVED:** Earlier SaveRAM Mongo/World 1175/1206 observations untouched; no inference of seed or banking from the fresh captures.
- **TESTED_PORT_LINUX:** 179/179 three build contexts, focused ASan/UBSan, negative controls, native graphics and Debian extracted 120-frame smoke.
- **NOT RUN:** Actual new Mesen debugger breakpoint session (the matching headless core was unavailable locally); Win32/NSIS, physical Xbox controller, full campaigns.
- **OPEN, highest priority:** Independent original stock first-playable-Spring generator including ruler/difficulty RNG, 14/27 countries, three orders; **this execution does not complete it**. Source-authentic merchant scheduling, AI, battle aftermath, dynasty/diplomacy and full campaign acceptance also OPEN.

Next actual Mesen target: with matching core, break or log writes to `$0084..$0089` (RNG state), `$0050` (BRK dispatch selector), and country Gold/Food/Rate addresses **during fresh setup**, capturing frame+cycle, CPU PC and MMC1 bank; run a distinct archived-restored case. Stage-end PCs alone are not write PCs. Use grounded chronology to connect source-derived all-country Spring finance and setup transforms to a genuine, dynamically playable state. Keep Classic versus Enhanced distinct. Preserve older source, original-only assets private, and hourly activity enabled. Publication is NOT Nobunaga/Cao Cao read-execute acknowledgment, a certified game release, or Director acceptance.
