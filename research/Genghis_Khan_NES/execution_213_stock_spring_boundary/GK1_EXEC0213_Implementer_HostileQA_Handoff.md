# GK1 EXEC0213 — Actual implementer / hostile-QA handoff — 2026-11-12

## Implementation
Native C11 existing `execution_199` modified, not a separate prototype: `gk213_stock_spring_boundary.c/h`, `gk1_app_state.c/h`, `gk1_native_shell.c`, test, CMake, README. Original Spring boundary is a source-bounded test of **one** original fresh-reset history, **not** independent Spring generation or playable campaign completion. The CLI produces a human-readable value receipt; source CSV and all fourteen private source stage JSON hashes and final WorkRAM 83-byte record are validated before output. Out-of-scope original RNG chronology never filled with guesses. One proof of nonuniqueness: economy186 and187 both imply Gold430 by integer truncation, so alternative histories are plausible.

## Reproduction commands
```
cmake -S execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build -j8 --output-on-failure
./build/test_gk213_stock_spring_boundary /PRIVATE/fresh_stages execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
./build/gk1_native_shell --audit-original-first-spring-boundary /PRIVATE/fresh_stages execution_049/GK1_ALL_41_COUNTRY_ECONOMIC_DATA_v1.07.csv
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
cd build && cpack -G DEB
```
No source ROM or private JSON distributed. Test log/receipt/manifest in archives.

## Hostile QA and classification
- **VERIFIED_STATIC:** authenticated previous original source (exact SHA), 14 original fresh-stage source files (authenticated by gk211), original unchanged template, patched 8 current C11 paths byte-identical to full archive, old files preserved, wrong data refused without mutation.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET:** previously recorded stock NES history only; WorkRAM source vs archived original SaveRAM **NOT** conflated. **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** preserved but not used as equivalent initialization.
- **TESTED_PORT_LINUX:** Release 176/176, Debug 176/176, independent clean source rebuild 176/176; 32 private original-source focused ASan+UBSan PASS, 120-frame native and Debian-extracted Xvfb PASS.
- **NOT RUN:** new Mesen session, original source character/difficulty/seed chronology, native Win32/NSIS, physical Xbox-only full game, complete campaigns. `1.99.0-dev1` remains uncertified.

## Next task
Move beyond retrospective finance checks: capture genuine stock original Mesen startup RNG writes, 14/27 state transforms and direct original setup/season entry; implement dynamic first Spring in existing C11 app, tests under multiple distinct starting histories, then player-facing merchant scheduler and full campaigns. No frozen screenshot values and never silently transition PRE_SETUP to playable.

## Privacy/publication
All ROM, savestate, RAM, raw gameplay/screenshots remain private Genghis folder 1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7. Only copyright-clean source/test/patch in public GitHub; full source and handoff to verified Collaboration Drive parent 1d0TA1lgqsVA3AWQQ9o0FZgmvao90oCeT. Publishing does not imply Nobunaga read/execute acceptance.
