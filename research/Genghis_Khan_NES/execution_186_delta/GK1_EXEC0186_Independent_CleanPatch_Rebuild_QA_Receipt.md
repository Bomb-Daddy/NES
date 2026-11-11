# GK1 EXEC0186 — independent clean patch replay / CTest QA receipt — 2026-11-11

The *complete* EXEC0185 current 584-file source was independently copied, and the exact EXEC0186 integration patch was applied with `patch -p1` to that copy. Every resulting byte was hashed and matched against the 585-entry authoritative EXEC0186 source manifest: **585/585 PASS**. This is a real source replay, not the same working tree used for development. The inherited historical dependencies (`execution_034`, `035`, `039`, `040`, `045`, `047`, `049`) were provided from the same verified historical source archive.

A clean CMake/Ninja Release build from the independently patched copy then compiled and completed **149/149 CTest PASS**, process exit 0. Other current build tests: primary Release149/149, Debug149/149, focused authenticated stock source ASan/UBSan 62 assertions, normal and extracted Debian X11/Xvfb 120-frame visual smoke PASS. The authentic original ROM-based callsite audit found zero conditional direct JSR `$C289` candidates in each of 15 hypothetical low banks; this is *not* original Mesen execution.

```
cmake -S clean_replay/execution_185 -B build186clean -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build186clean -j8
ctest --test-dir build186clean --output-on-failure -j8
# 100% tests passed, 0 tests failed out of 149
```

**VERIFIED_STATIC:** clean patch SHA replay PASS. **TESTED_PORT_LINUX:** actual independent compilation and tests PASS. **OBSERVED_RUNTIME_STOCK:** earlier evidence, not rerun. **NOT RUN:** new native Mesen, Win32/NSIS, physical Xbox and full campaigns. Still no authentic source-generated first Spring or three original Classic strategic orders; never claim completed game or licensed ROM publication.
