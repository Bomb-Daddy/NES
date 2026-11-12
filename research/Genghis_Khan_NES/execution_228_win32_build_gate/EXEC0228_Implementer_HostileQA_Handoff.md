# GK1 EXEC0228 — Native Win32 compiler/installer qualification harness
2026-11-12 · additive implementation from verified **EXEC0227** · Director's Windows gap closure

## New executable implementation

- `execution_199/gk228_win32_smoke.c`: new C11 **console** executable linked directly to the existing mutable `gk1_app_state` and *same* `gk99_xinput_bridge` as the actual Win32 WinMain game. Tests exact v1.07 CSV identity, complete 14 Mongol/27 World country installation, every original country ID and PRE_SETUP state, blocked synthetic merchant offer without mutation, XInput A/START mapping, 4-slot controller selection and Windows-profile save destination. On `_WIN32` it additionally calls Win32 `GetModuleFileNameA` and `GetFileAttributesA` and emits `NATIVE_WIN32_BUILD_EXECUTED`; on Linux it explicitly emits `LINUX_PORT_NEGATIVE_CONTROL`.
- `execution_199/packaging/test_gk228_windows_contract.py`: hostile test calls the *compiled binary* with valid, single-byte mutated, truncated and missing-source CSV. Invalid inputs must return nonzero. Valid input must leave CSV untouched and identify the lack of original first-Spring and physical-device testing.
- `execution_199/CMakeLists.txt`: adds actual target to **existing app build** and two CTest tests; discovers Python on Windows as well as Linux.
- `.github/workflows/gk1-native-windows.yml`: **staged candidate, not yet installed as an active repository Actions workflow**. Describes Windows-2022 Visual Studio build of same full inherited source, test execution, NSIS installer generation and silent installer file checks. Does NOT upload an unaccepted game release, original NES assets or user saves. **Do not claim native Win32 execution until this workflow actually runs and succeeds on an accessible complete full source checkout.** GitHub's currently published numbered directories provide deltas, not the complete integrated application; workflow activation is deferred until the complete source tree is published in the checkout. The complete local verified ZIP is authoritative in the meantime.

## Reproduction (complete source)

1. Unzip this release's **Complete Integrated Source QA** ZIP into an empty directory. Its `execution_199` and sibling `execution_034/035/039/040/045/047/049` are the same complete inherited app used for the Linux build.
2. `cmake -S execution_199 -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build-release -j8 && ctest --test-dir build-release -j8 --output-on-failure` — **199/199 PASS**.
3. The analogous Debug configuration — **199/199 PASS**.
4. The separate `GK1_228_*` tests — **2/2 PASS** on native Linux; they assert `LINUX_PORT_NEGATIVE_CONTROL`. This is **not** a Windows run.
5. Clean patch replay: unpack verified predecessor EXEC0227 ZIP, run `git apply --check GK1_EXEC0228_CopyrightClean_Integration.patch`, then `git apply ...`, hash all five changed/new paths. All 5/5 byte-equal. Clean independent replay compiled and **199/199 CTest PASS**.
6. A genuine Windows host or CI runner must build with VS2022 x64, run `ctest -C Release -R 'GK1_228_|GK1_099_|GK1_062_'`, package NSIS, perform silent install/checks and capture exit codes. Physical Xbox controller checks require actual hardware and campaign QA; simulated controller tests do not certify it.

## Evidence labels

- **VERIFIED_STATIC**: published compiler-source contract, source hashes, exact 5-path patch replay, original CSV identity and mutation controls.
- **OBSERVED_RUNTIME_STOCK_FRESH_RESET**: only previously authenticated WorkRAM evidence, no new Mesen execution here.
- **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED**: only previous SaveRAM evidence, distinct from fresh.
- **TESTED_PORT_LINUX**: 199 Release, 199 Debug, 199 clean rebuild CTest; native C11 compiled and Linux graphical regressions inherited and rerun; Linux .deb repacked.
- **NOT RUN**: Win32 native compiler/test execution, NSIS installer on Windows, physical Xbox device, complete campaigns, actual source-authentic 14/27 first Spring, fresh Mesen instruction-level trace. The exact recovered private stock NES ROM is **not missing**; its SHA remains `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`.

## Gates / correction priority

Do not infer source-authentic first Spring or real original Trade/AI/war/dynasty from Windows smoke. Next work is to provision matched Mesen executable+core and trace original memory writes/selector and difficulty/ruler RNG, implement actual first playable Spring and 3 orders in existing native game, then run Windows CI and physical Xbox with complete campaigns. Do not stop hourly development until Director stop or verified complete release. Publication does not prove agent read/execute acknowledgment.
