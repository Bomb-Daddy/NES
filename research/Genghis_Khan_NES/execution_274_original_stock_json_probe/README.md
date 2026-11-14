# EXEC0274 — Original-stock JSON framing acceptance (2026-11-14)

This is a delta against the FULL previously tested EXEC0273 native C11 application. All unchanged files retain exact bytes. Modified production loader `execution_199/gk193_auto_source_entry.c` now accepts UTF-8 BOM and JSON whitespace preceding an original Mesen JSON file. It recognizes only actual byte-zero MSS\x01 as an archived binary source. The existing gk165 SHA-pinned WorkRAM, bank, 14-country and template verification remains mandatory; no arbitrary or synthesized Spring is accepted. No ROM, original private gameplay records or protected assets are included.

The production test `execution_199/test_gk274_source_probe.c` covers 13 source cases, rejects malicious BOM, binary offsets, forged JSON, huge prefixes, and checks byte-identical application rollback. The inherited CMake is amended to register it. The predecessor 0273 C11 loader FAILED the new test; this build passes. Genuine independent first-Spring RNG/season setup is still incomplete.

Build and test:
```
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target gk1_native_shell test_gk162_original_entry test_gk165_fresh_json test_gk166_fresh_entry test_gk193_auto_source_entry test_gk274_source_probe -j6
ctest --test-dir build -R 'GK1_(162|165|166|193|274)_' --output-on-failure --timeout 90
```

Acceptance: VERIFIED_STATIC patch inheritance; TESTED_PORT_LINUX full native shell + 5/5 relevant tests; OBSERVED_RUNTIME_STOCK fresh/archived evidence is historical and not rerun; EXEC0274 Win32, NSIS, physical Xbox and full campaigns NOT RUN locally. Previous successful EXEC0273 GitHub Windows CI is independent earlier evidence.
