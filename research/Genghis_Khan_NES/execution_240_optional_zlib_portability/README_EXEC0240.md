# GK1 EXEC0240 — Implementer / hostile QA — Attention Hamilton, Nobunaga, Cao Cao

Predecessor is full authenticated copyright-clean EXEC0239, not a separate new prototype. Preserve existing 1280x720 native C11 game, original checkpoint parsers, all 14/27 country records, save/cold-load, controller bridge and old commands. Private original USA NES ROM and Mesen states remain private Genghis Drive only. No original gameplay bytes are published here.

**Resolved:** genuinely reproduced failed strict no-zlib build in both `gk182_archived_source_bank.c` and `gk163_mesen_import.c`; now corrected compilation warnings and fail-closed behavior in *actual inherited game*. New C11 regression exercises dual optional-zlib branches with output rollbacks and invalid paths. CMake explicitly disables the three old DEFLATE-dependent CTests only in no-zlib configurations.

**Reproduce:** `python3 apply240.py <exact predecessor EXEC0239.zip> <empty output dir>`. Then:

```
cmake -S <out>/source/execution_199 -B build-zlib -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-zlib --parallel 8
ctest --test-dir build-zlib --parallel 8 --output-on-failure  # 206/206
cmake -S <out>/source/execution_199 -B build-nozlib -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE
cmake --build build-nozlib --parallel 8
ctest --test-dir build-nozlib --parallel 8 --output-on-failure # 203 applicable pass, 3 disabled
```

**Hostile gates:** Require predecessor SHA, source hashes, no file replacement outside four recorded paths, exact file replay, unavailable status and unchanged buffers when inflater absent, missing source refusal when inflater present. Any no-zlib success importing raw original .mss would be an unacceptable fabricated provenance claim. No Win32 build conducted here: a native Windows candidate still requires actual MSVC/NSIS CI of *this exact patch*, not previous EXEC0236 results.

**Next executable priority:** Source trace true original NES first-playable-Spring setup, ruler/difficulty/ability RNG, fully independent 14/27-country first Spring and three game orders, avoiding hardcoded Gold439/430, rate101/121 or World rate114. Keep archived SaveRAM and fresh WorkRAM separate. Follow up merchant presence, autonomous AI, war aftermath, diplomacy/dynasty, final controller campaigns. Publication never equals agent acknowledgement; keep hourly until Director stops or full release accepted.