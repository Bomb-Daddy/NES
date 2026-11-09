# EXEC0133 integration into the existing native game

**Do not use a new prototype.** The 1.33.0-dev1 game is the complete inherited `execution_132` C11 program with `execution_133` additive changes. The complete tested historical ZIP is the authoritative current source when available.

To reproduce the narrow change from EXEC0132:

1. Copy the complete `execution_132` directory to `execution_133` without deleting predecessor code, tests, assets, Classic/Enhanced profiles, save data or campaigns.
2. Add the published `gk133_boot_order.c`, `gk133_boot_order.h`, `test_gk133_boot_order.c`, and `README_EXEC0133.md` to `execution_133`.
3. In the existing **full** `execution_133/CMakeLists.txt`, insert `gk133_boot_order.c` in the source list of the existing `add_library(gk1_app_state STATIC ...)`, immediately following `gk132_f5_setup.c`.
4. Append the following to CMakeLists.txt:

```cmake
add_executable(test_gk133_boot_order test_gk133_boot_order.c)
target_link_libraries(test_gk133_boot_order PRIVATE gk1_app_state)
add_test(NAME GK1_133_AuthenticatedBootReadBeforeWriteAndMesenPowerOnProfile COMMAND test_gk133_boot_order)
```

5. Update `gk1_native_shell.c` version from `1.32.0-dev1` to `1.33.0-dev1`, including the smoke success label. Update CMake project/package name, Linux desktop entry, packaging contract assertions and NSIS display/version descriptors **without** dropping existing installer rules.
6. Rebuild **the full application**:

```sh
cmake -S execution_133 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
GK1_ORIGINAL_STOCK_ROM=/PRIVATE/original_stock.nes ctest --test-dir build --output-on-failure -j8
cpack --config build/CPackConfig.cmake -G DEB
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
```

Use only the exact user-private original ROM for local verification; never commit it, RAM dumps, original graphics or audio. The accepted test result is **91/91** Release CTest and compiled+Debian-extracted 120-frame Xvfb. **This does not grant a playable first Spring**. Unknown power-on F5 still blocks stock startup, whereas the archived Mesen AllZeros profile reaches the unmodeled PPUSTATUS $2002 gate. Win32, NSIS installation, physical Xbox and full campaigns remain NOT RUN.

Complete integrated research source and reproducible test evidence remain in the local EXEC0133 artifacts; don't claim a complete GitHub tree from this incremental delta.
