# GK1 EXEC0205 — source-executed CPU selector telemetry, development candidate

Date: 2026-11-11. **Predecessor:** full inherited EXEC0204 C11 source (not EXEC0076). **Release status:** research candidate. Application version intentionally remains `1.99.0-dev1`; this is not a Director-certified 2.05 game release.

## Implemented source change

Actual existing bounded 6502 original-ROM CPU interpreter (`gk83_reset_trace.c`) now records **completed bus writes** targeting zero-page selector `$0050` and its NES internal RAM mirrors. The `Gk205SelectorWatch` structured receipt includes source instruction PC, physical CPU address, selector value, reported PRG bank and known/unknown flag, interpreter bus-cycle count, and bounded history (16 first events plus last and dropped-event count). A pending BRK gate records the source PC and known selector before refusal, explicitly **not** an executed BRK, IRQ handler, RNG invocation or playable Spring. The original Machine RAM write handling is preserved. An authentic source gate stops at the first unmodelled read rather than guessing. Zero or missing runtime events never prove no future writes occur.

Existing app adds `--audit-stock-selector-runtime PRIVATE_AUTHENTIC_STOCK_NES`. It authenticates the **original exact 262,160-byte source SHA256** before the bounded CPU trace, prints diagnostics with no ROM bytes or screenshots, rejects counterfeit source, and has no command to promote PRE_SETUP to playable Spring. Does not modify previous Classic/Enhanced gameplay or private checkpoints. The new gk205 module/test links to the inherited `gk1_app_state` library and existing executable; not a standalone unrelated app.

## Actual new private original-ROM-backed result

Stock SHA256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3` reverified locally. Bounded native original-source executor returned `GK83_UNKNOWN_RAM_GATE=3` after exactly **70 completed CPU instructions**, source PC `$8060`, unknown CPU RAM read `$00F5`; **0 completed selector $0050 writes and 0 BRK attempts before the gate**. This is a genuinely executed, **SOURCE_BOUNDED_PORT_CPU** trace, **NOT Mesen authentic original-game runtime**, and makes no claim about post-gate setup, RNG, selected characters, Gold or Rate. A corrupted original ROM byte was rejected with CLI exit 1.

The existing 21 raw selector store candidates are unchanged: static source byte patterns, not proof of actual reachability. Original Mesen fresh WorkRAM first-Spring and archived-restored SaveRAM captures remain two distinct historical evidence categories; they were not rerun in EXEC0205. No newly observed Mesen execution of daughter ages 6/7/8, water, war termination, or trader RNG.

## QA evidence — performed in this execution

- Native Linux Release **168/168 CTest PASS**, Debug **168/168 PASS**, independent extracted-original-EXEC0204-plus-patch clean Release **168/168 PASS**.
- New compiled Gk205 test: **53 assertions** with real original private stock ROM; repeated Debug and independent clean-source execution. AddressSanitizer and UndefinedBehaviorSanitizer focused binary with authenticated original ROM: **53 assertions PASS**.
- Positive/negative controls: malformed/null watcher input, wrong addresses, NES RAM mirroring, invalid bank/flag, 16-event bounded receipt, overflow, unknown selector-at-BRK vs known, counterfeit ROM exclusion; source patch replay **7/7 changed/new files byte-identical**.
- Native X11/Xvfb 1280x720 smoke **120 frames PASS**; extracted `.deb` native executable **120 frames PASS**. No physical controller tests.
- Source-only `.deb` is a research package with historical 1.99.0-dev1 identity. It is **not** a native Windows build, an NSIS installer, or full game release.

## Reproduction

```bash
unzip GK1_EXEC0204_Current_Integrated_Reproducible_C11_Source_QA.zip -d replay
cd replay
git apply /path/GK1_EXEC0205_CopyrightClean_Integrated_Source.patch
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j6
ctest --test-dir build -j8 --output-on-failure
GK1_ORIGINAL_STOCK_ROM=/private/original.nes ./build/test_gk205_selector_runtime_watch
./build/gk1_native_shell --audit-stock-selector-runtime /private/original.nes
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
```

## Evidence classification and blocking release gates

**VERIFIED_STATIC:** authenticated file SHA, source patch replay, code and negative-control structure. **SOURCE_BOUNDED_PORT_CPU:** stock ROM executed in incomplete model for 70 instructions then stopped on unknown `$F5`, no selector stores/BRK before stop. **OBSERVED_RUNTIME_STOCK:** inherited separate fresh-reset and archived-restored Mesen evidence, no new runs. **TESTED_PORT_LINUX:** Release/Debug/clean build/ASan+UBSan/X11/DEB smoke. **NOT RUN:** Windows Win32/NSIS, physical Xbox, original Mesen new CPU selector trace, complete campaign.

**UNFINISHED HIGH PRIORITY:** make first original Spring independently generated for all 14/27 real countries with source-derived ruler/difficulty/character RNG and 3 actual orders; then complete merchant RNG, autonomous AI, diplomacy, dynasty, aftermath and production packaging. `PRE_SETUP` remains locked. No observed Gold/Rate constants hardcoded. No step upgrades this bounded trace into a source-authentic playable Spring.

**Publication gates:** Raw copyrighted ROM, snapshots, images, audio, original RAM records, and original Mesen trace remain in private Genghis research folder only. Public deliverables are copyright-clean C11 patch, tests, source, SHA manifest and QA. Preserve older assets. Publication does not imply external developer acknowledgement, Director acceptance, or Windows/physical Xbox execution.
