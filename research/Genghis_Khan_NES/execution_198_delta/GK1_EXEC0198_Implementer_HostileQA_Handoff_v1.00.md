# GK1 EXEC0198 — Verified Native Implementation / Hostile QA Handoff — November 11, 2026

## Inheritance
EXEC0197 version 1.97.0-dev1 is the immediate complete C11 source predecessor. All 46,349 historical ZIP entries are preserved exactly; the latest source tree includes 627 files (up from 623). Previous two-mode Classic/Enhanced application, original checkpoints, X11/Win32 source entry points, controller, source authenticity guard, Classic TAX action, saved-state replay, 14/27 templates and earlier mechanics are inherited unchanged except identified integration edits. Code is in the EXISTING application, not a separate simulation.

## Actual linked player-facing change
`gk198_tax_forecast.c/.h` computes a rate-only comparison for the Classic TAX controller selection. The actual 1280x720 TAX interface now displays source-traced Spring gross-tax and after-payroll net-Gold values for the currently committed tax rate compared to a proposed rate 1..100, after authentic source import and validated Classic replay, INCLUDING previously committed Classic worker assignments. Reuses inherited source-bounded `gk75_spring_food_tax_pay` 6502 bank1 $8099/$8106 formula rather than newly guessed arithmetic. It does not mutate original 83-byte records or commit the proposed rate; A and B retain EXEC0197 commit/cancel semantics. This is an **experimental projected original Spring math comparison**, NOT an actual source-authentic next Spring, original ruler ability deduction, morale change, trade, full startup, original RNG or battle/AI.

## Reproduction and regression
From exact full source ZIP extract `execution_198` plus inherited dependent source folders. Run:
```
cmake -S execution_198 -B build198 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build198 -j12
ctest --test-dir build198 --output-on-failure -j16
cmake -S execution_198 -B build198_debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build198_debug -j12
ctest --test-dir build198_debug --output-on-failure -j16
# With private original stock files provided separately, run the 3-profile exact-stock test.
./build198/test_gk198_tax_forecast <original CSV> <fresh_original_JSON> <archived_original_Mongol_MSS> <archived_original_World_MSS>
xvfb-run -a -s '-screen 0 1280x720x24' build198/gk1_native_shell --smoke-classic-tax-original <original JSON or MSS> 120
(cd build198 && cpack -G DEB)
```
QA RESULT: Release **161/161** CTest, Debug **161/161** CTest, **52/52** private profile assertions, three separate 120-frame 1280x720 native Linux X11 simulated pad runs exit0, Debian-extracted executable version/120-frame run exit0. Fresh original WorkRAM and archived original SaveRAM evidence validated as distinct oracle paths, not constants. **ASan/UBSan not completed** (sanitizer configuration/build timed out); do NOT count as pass. No new Mesen run; no Win32 binary/NSIS/physical Xbox/full campaign test. No claims beyond these results.

Negative controls include pre-setup and wrong source profile refusal, tax range 0/101, corrupt/replayed Classic state, false source identity, original-source immutability, output atomic rollback, derived worker allocations after ASSIGN and exhausted orders. See C source/test and QA ZIP.

## Evidence classification
- VERIFIED_STATIC: Exact authenticated source identity reference, complete 627/627 file hash manifest and independently clean patch replay, all 46,349 previous ZIP members preserved, full ZIP CRC pass and new artifacts SHA256.
- OBSERVED_RUNTIME_STOCK_FRESH_RESET: Inherited earlier genuine Mesen Spring1175 workRAM; no new original run this execution; original country1 Gold430/Rate121 under *that particular* RNG/choice history.
- OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: Inherited original Mesen Spring1175 SaveRAM country1 Gold439/Rate101 and World1206 Gold193/Rate114; not recomputed or conflated with fresh.
- TESTED_PORT_LINUX: 161/161 CTest each Release and Debug; 52/52 private assertions; three 120-frame X11 simulated-controller runs; one actual Debian-extracted executable test.
- NOT RUN: Full sanitizers, Windows native/NSIS, physical Xbox controller, full campaigns, new original-stock Mesen parity, daughter marriage age6/7/8, real water map and battle terminal.

## Mandatory remaining milestone
Native app STILL imports verified Spring captured snapshots rather than independently executing the original 14/27-country first-Spring setup/difficulty/character RNG and rules. TAX ability, morale and season transition source parity remain open. Finish genuine first playable Spring (original RNG and source-authentic 14/27), then controller-operated real Trade presence/RNG, AI, original complete battle aftermath, diplomacy, dynasty, art/audio, save/quit/cold-load, Win32 installer and physical controller full campaigns. Preserve Classic vs Enhanced separation and old source. This handoff's publication does NOT prove another agent read/acted upon it.

## Privacy and supersession
Copyright-clean native C11 source/test/patch may be public. Director-authorized original stock ROM SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, private Mesen `.mss`, fresh raw WorkRAM, original screenshot/event/audio and licensed assets stay PRIVATE Genghis research. EXEC0198 supersedes only the current-source/status of EXEC0197, not original NES observations or older verified components.
