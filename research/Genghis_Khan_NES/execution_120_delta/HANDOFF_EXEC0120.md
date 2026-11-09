# EXEC0120 — Native user-data save routing, C11 integration and hostile QA

Source inheritance: full, verified EXEC0119 native application with 309 source/test/package files; this changes the same native shell, does not replace any 14/27-country source, battle, market, existing save, or Enhanced receipt implementation.

## Implemented in actual game

- New `gk120_save_home.c/h`: absolute, traversal-refusing Linux default save-path policy. Honor absolute `XDG_DATA_HOME`, otherwise absolute `HOME/.local/share`; append `gk1-native-research/<filename>`. Reject path injection and path overflows atomically. Make missing user subdirectories with mode 0700, verify they are directories. Do not create or overwrite game save files as a side effect of startup.
- The **same** Linux native executable now uses this policy for all six existing default save families: modified Divide replay, observed path replay, source-bounded research replay, manual setup, source-attested Enhanced Assign receipt, and exact original stock checkpoint receipt. Explicit environment overrides remain unchanged. Running the binary directly from another CWD no longer writes saves beside the binary or depends on the desktop wrapper changing directories.
- On Windows, the preexisting `gk59_profile_default()` (which builds under LocalAppData/GK1NativeResearch and ensures the directory exists) now also handles Enhanced Assign saves. Previously the Enhanced receipt alone used a relative path and could fail if the shortcut started in a protected installer directory. **Windows code static only, not native Windows execution proof.**
- New native CLI `--default-storage-pad-check`: real X/Y synthetic controller route performs an exclusive pre-setup save and source-gated cold-load from the resolved XDG default, asserts an actual file with expected bytes and unchanged scenario identity. Not a playable original first Spring.
- New standalone negative/positive C11 test, `test_gk120_save_home.c`: XDG preference, HOME fallback, trailing slashes, slot names, malformed paths, no CWD save, directory creation, readback, non-directory component refusal and Windows LocalAppData filename composition.
- Linux package contract now independently invokes the new native CLI with a separate CWD and XDG data directory. The Debian-installed wrapper runs the same test after package extraction, plus 1280x720 Xvfb smoke.

## Reproduction

```
cmake -S execution_120 -B build120 -DCMAKE_BUILD_TYPE=Release
cmake --build build120 -j6
ctest --test-dir build120 --output-on-failure -j6
(cd build120 && cpack -G DEB)
XDG_DATA_HOME=/tmp/gk120-own-data xvfb-run -a build120/gk1_native_shell --smoke-frames 120
```

Additional Debug and ASan+UBSan CTest runs were performed with leak detection disabled for sanitizer testing. Tests use synthetic native gamepad events and **do not** substitute for physical Xbox acceptance.

## Evidence labels and unresolved work

- `VERIFIED_STATIC`: exact inherited source and original-stock research evidence references retained. No private ROM or original gameplay RAM bundled or published.
- `OBSERVED_RUNTIME_STOCK`: preserved historical original Mesen Spring1175 fresh Gold430 Food208 Troops61 Rate121, archived Spring1175 Gold439 Food208 Troops61 Rate101, and World1206 Gold193 Food332 Troops116 Rate114; none rerun here, no values hardcoded as universal native first Spring.
- `TESTED_PORT_LINUX`: 78/78 Release, 78/78 Debug, 78/78 ASan+UBSan (leak detection disabled), native graphical 120 frames and extracted Debian wrapper default X/Y cold-load completed.
- `NOT RUN`: Win32 native executable, NSIS installer, physical Xbox; original source-derived first Spring 3 orders, source merchant presence/transaction, 12 locked commands, AI/dynasty/diplomacy/battle aftermath/full campaigns remain unfinished.

This is a save integrity/playability infrastructure correction, **not an original NES fidelity breakthrough** or a completed Genghis game. Keep hourly continuation enabled pending Director stop or accepted playable release.
