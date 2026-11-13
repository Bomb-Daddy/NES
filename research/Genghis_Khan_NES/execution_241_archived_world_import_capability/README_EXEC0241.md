# GK1 EXEC0241 — Implementer / hostile-QA handoff — Attention Hamilton, Nobunaga, Cao Cao

Predecessor exact EXEC0240 full source ZIP SHA256 `b2b362b146ee89b4d874ca1c54c82c2ee1a187b6014a32aca8fcd00d583dface`. This increment actually modifies `source/execution_199` in the same existing C11/X11/WinMain game and adds a native C11 test. It does NOT generate independent first Spring, new merchant offers or a playable finished campaign.

**Defect:** prior Win32 packaging could silently lack ZLIB and therefore never load authenticated archived World owner `.mss`; GUI error said only original source refused. This is false diagnosis when the source may be valid and the compiled library is absent.

**Fix:** new `gk241_archived_import_capability.{h,c}` provides source-true compile-time capability and a truthful GUI failure reason. `gk1_native_shell.c` uses the reason on `GK239_REFUSED`. `CMakeLists.txt` includes new app source/test and `GK1_REQUIRE_ARCHIVED_WORLD_IMPORT` opt-in fatal configuration gate. No made-up RNG, no mixed RAM-bank assumptions; `gk239` remains original archived SaveRAM source-locked and fresh WorkRAM remains distinct. No positive source is ever fabricated.

**Reproduce** (copy complete inherited archive to a clean directory, then `git apply` `EXEC0241/GK1_EXEC0241_Integration.patch` or inspect the already integrated `source`):

```sh
cmake -S source/execution_199 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build --parallel 8 --output-on-failure
cmake -S source/execution_199 -B build-nozlib -G Ninja -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE
cmake --build build-nozlib -j8
ctest --test-dir build-nozlib --parallel 8 --output-on-failure
cmake -S source/execution_199 -B build-reject -G Ninja -DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE -DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON  # expected NONZERO
```

Hostile controls: negative source handling does not change 14/27 world/template or player state; dual environment source conflicts rejected; diagnostics never touch owner path; backend messages distinguish missing library from invalid evidence; no-zlib release opt-in fails build, no-zlib research runs honestly. This is NOT source-positive private Mesen execution and is NOT Windows MSVC/NSIS acceptance. Next real task is native Windows-2022 CI of the exact current source with zlib/x64 static and full installer smoke, then independent first Spring seed/setup trace and actual orders.

Copyright/privacy: no original ROM, SRAM, `.mss`, controller screenshots or licensed assets in public GitHub; private research remains under `1mJ72ezJb9kzIGUHHXPTh3lXtiVxWjEL7`. Do not confuse publication with recipient execution or ACK.
