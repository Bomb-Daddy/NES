# EXEC0201 — CANDIDATE handoff and hostile-QA evidence (2026-11-11)

## Exact provenance and scope

The newest certified publication independently found in Drive/GitHub was EXEC0199; EXEC0200 is an uncertified reconstructed source candidate. No EXEC0201+ was found before this change. The original complete EXEC0197 predecessor was not available, so **this is neither a certified release nor byte-identical reconstruction**. It is source/test changes to the **same native C11 app** derived from the EXEC0200 candidate. Original stock USA ROM and licensed assets remain private and are not contained in any public GitHub files.

## Actual linked C11 change

The native shared pad router now quarantines simultaneous A+B, X+Y, LB+RB and opposing D-pad chords until a *fully neutral sample* occurs for that pair. Previously, on a single suppressed X+Y sample, releasing Y while X stayed held caused X to emit a new edge and potentially invoke the controller-only Save command. A+B -> A could similarly cause an accidental confirm. Both Xbox raw XInput mapping and Linux controller routes enter `gk1_pad_step`. This is a PORT-ONLY quality/safety improvement; no claim of stock NES button behavior.

Files changed: `execution_199/gk1_pad_router.c`, `.h`, `CMakeLists.txt`; new `execution_199/test_gk201_chord_rearm.c`. C source patch `GK1_EXEC0201_Controller_Chord_Rearm_Source.patch` applies with `git apply` at the candidate source root; four source paths replay byte-identically. Physical Xbox not exercised.

## Reproducible local commands

```
git apply --check GK1_EXEC0201_Controller_Chord_Rearm_Source.patch
git apply GK1_EXEC0201_Controller_Chord_Rearm_Source.patch
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build --output-on-failure
cmake -S execution_199 -B build_debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build_debug -j8
ctest --test-dir build_debug --output-on-failure
xvfb-run -a build/gk1_native_shell --smoke-frames 120
```

The full source archive additionally preserves the inherited candidate's sibling execution_034, 035, 039, 040, 045, 047, 049 and QA files. It contains source and research-only assets, **not** ROMs, captured RAM, licensed soundtrack/art or gameplay savestates. It retains the full original EXEC0200 source candidate unchanged except the four listed paths.

## Verified actual results

- **VERIFIED_STATIC**: prior reconstructed EXEC0200 candidate 619/631 official hashes exact; this new candidate 616/631 exact, 15 mismatches = 12 historic disclosed + 3 intentionally changed source, zero missing, plus one new test. Four changed paths replayed exactly through source patch.
- New focused C11 unit test: 108 assertions PASS (5 contradictory button pairs; XInput raw X/Y; reconnect; unrelated START). ASan+UBSan 108 assertions PASS.
- **Adversarial negative control**: compile *original pre-change* router against the new test: exit 1 with `GK201 FAIL` at partial-release edge. New router passes. Positive/negative design discriminates bug.
- **TESTED_PORT_LINUX**: actual same C11 native app Release 163/163 CTest PASS, fresh independent Debug 163/163 PASS; `xvfb-run -a ... --smoke-frames 120` 1280x720 PASS. CPack Debian package generated; Debian-extracted X11 executable via packaged wrapper override ran 120 frames at 1280x720 PASS; independently extracted full source archive built Release 163/163 CTest PASS. Tests use simulated inputs, not real Xbox hardware.
- **OBSERVED_RUNTIME_STOCK**: no new original Mesen run. Older authenticated fresh WorkRAM and archived SaveRAM evidence are inherited, NOT independent first-Spring calculations. No new source RNG/difficulty proof.
- **NOT RUN**: native Win32 build, NSIS installer, physical Xbox controller and full campaigns.

## Open priority gates

(1) Recover exact original 0197 historical source, (2) original independently generated first Spring setup/ability/difficulty/stat RNG for all 14 Mongol and 27 World countries, (3) merchant presence/quotes real scheduling and all strategic commands, (4) autonomous AI, diplomacy, dynasty, battle terminal aftermath, multimedia, campaign cold-load, verified Windows/physical Xbox all-campaign test. Avoid hardcoded rates or gold from isolated Mesen reset screenshots. Continue existing hourly work, no new task created. Do not claim external agent read acknowledgement from publication.
