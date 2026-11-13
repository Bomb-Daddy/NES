# Genghis Khan I EXEC0259 — Authentic Win32 MSVC test-source portability

**Date:** 2026-11-13. **Project:** existing English NES GK1 native Win32/XInput and Linux X11 C11 game. **Predecessor:** EXEC0257 current SHA-pinned lean 793-file source, with EXEC0258 Win32 host vcpkg path fix. The full unrelated source and both campaign records remain intact. This handoff is actual code and tests, not a new prototype.

## Real failure reproduced

Windows-2022 workflow https://github.com/Bomb-Daddy/NES/actions/runs/43493525766 checked out and authenticated the inherited source, applied exact EXEC0257 patch, installed NSIS, resolved vcpkg, installed static zlib, and compiled most of the real 1280x720 C11 project. MSVC compilation returned error C1083 in **three historical Linux-biased test files**: `test_gk120_save_home.c`, `test_gk91_reset_zero.c`, and `test_gk1_pre_setup_store.c` tried to include unavailable `<unistd.h>`. The actual native `gk1_native_shell.exe` linked in the MSBuild log but full compilation failed; NEVER call the run TESTED_PORT_WIN32 or claim CTest/NSIS had been executed.

## Code correction

The exact reproducible `GK1_EXEC0259_Integration_From0257.patch` applies **after** the exact EXEC0257 patch to the unchanged SHA-pinned 1,249,024-byte lean source. It changes four CMake/C11 files: (1) scope the POSIX/XDG filesystem test `GK1_120` to non-Windows hosts; (2) add Windows-native `GK1_259_Win32LocalAppDataPathAndRefusal` with 15 direct `gk59_user_file` assertions, including path traversal and no-state-change refusal; (3) use `<process.h>`/`_getpid` and `%TEMP%` for Windows protected original-ROM negative controls, retaining Linux `<unistd.h>` and `/tmp`; (4) drop an unused POSIX header from the independent pre-setup save regression. The native strategy game code itself is unchanged.

The exact root `.github/workflows/gk1-native-windows.yml` applies 0257 then 0259 with `git apply --check`, rejecting mismatched source before MSVC, and still requires full Win32 CTest, actual WinMain software controller/WM_PAINT run and actual NSIS install/cold launch. **TESTED_PORT_LINUX:** pinned lean source plus both patches independently clean-extracted and replayed. GCC Release native game rebuilt, focused three CTests passed, 15 new Win32 path-policy numeric assertions passed when separately compiled under GCC, hostile source QA 7/7 passed. Linux-only behavioral coverage remains. Do not confuse Linux policy compilation with a Windows executable run.

## Ongoing blockers and labels

**OBSERVED_RUNTIME_STOCK_FRESH/ARCHIVED:** historical private source evidence only, no new Mesen execution. **NOT RUN:** full Win32 current source acceptance until CI succeeds, physical Xbox controller and full campaigns. Original independent first Spring setup/RNG, original Trade RNG, autonomous AI, dynasty/diplomacy, battle ending and release remain OPEN. Original ROM/SRAM/Mesen states, screenshots and licensed assets NEVER enter GitHub.

**Reproduction:** unzip `GK1_EXEC0256_Lean_Inherited_Build_Source.zip`; `git apply GK1_EXEC0257_Integration_From0256.patch`; `git apply GK1_EXEC0259_Integration_From0257.patch`; `cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release -DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON`; `cmake --build build -j12`; `ctest --test-dir build -R 'GK1_(051_|091_|257_)' --output-on-failure`; run `python3 test_gk259_windows_source_gate.py CMakeLists.txt test_gk91_reset_zero.c test_gk1_pre_setup_store.c test_gk259_win_user_home.c`. Windows native CTest and installer require the actual Windows runner; source hash and publication are NOT run acknowledgment by any agent.
