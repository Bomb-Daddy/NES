# GK1 EXEC0301 — actual Windows CMake-variant integration repair (2026-11-15)

Predecessor: EXEC0300 (code) and EXEC0299 (last Windows CI success, 220/220).

Actual GitHub Actions run [43698862057](https://github.com/Bomb-Daddy/NES/actions/runs/43698862057) completed **FAILURE** on November 15, 2026 at 09:22:50 UTC. In the real Windows workflow, stages through EXEC0299 passed, then `stage300.py` refused at `CMakeLists.txt` SHA check, *before native compilation*. EXEC0300 assumed the Linux source archive's entire CMakeLists.txt must equal the Windows stage output, ignoring cumulative Win32 CTest registrations. This is a build-stage provenance mismatch, not proof that the ROM is missing or a game rule differs.

`stage301.py` corrects this with exact-hash pre/post verified **native `gk1_app_state.c/h`** and exact SHA-pinned three-file EXEC0300 code/test payload; uses narrow, once-only CMake library-anchor insertion and appends the original test while preserving all inherited Win32 CMake additions. Updates the existing `packaging/test_gk265_current_win32_source.py`'s authenticated app source hash **only if that gate exists**, refusing unexpected source-gate text. This is the same production `Gk1App` and C11 binary, not a new demo/prototype. All rejected inputs leave files untouched. No ROM/assets embedded.

Reproduce with complete EXEC0297 source checkout and exact EXEC0300 public source in sibling `execution_300_original_stock_nmi_seed_clock`:

```
export GK301_PRE300_APP=/path/to/inherited/execution_199
python3 test_stage301.py
python3 stage301.py /path/to/inherited/execution_199
cmake -S /path/to/inherited/execution_199 -B /tmp/gk301 -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/gk301 --parallel 6
ctest --test-dir /tmp/gk301 --output-on-failure -R 'GK1_(294|296|297|300)_'
```

Local evidence: `test_stage301.py` **13/13 PASS** including Windows CMake additions, Win32 SHA gate, CRLF normalization, tamper/refusal, duplicate and atomic no-write. Linux actual C11 app compiles; all 236/236 eligible CTests PASS (one known inherited stale extracted-workflow QA test excluded, exact name in ctest log), and 4/4 targeted source tests PASS. No original-stock Mesen run this execution, no independent source-generated first Spring, no physical Xbox or complete campaign. Win32 CI requires a new successful Windows run; do not promote local Linux to Windows.

Status **VERIFIED_STATIC** pass; **TESTED_PORT_LINUX** 236/236 eligible pass; **TESTED_PORT_WIN32_CI** pending new workflow result; **OBSERVED_RUNTIME_STOCK** historical fresh/archived unchanged; **NOT RUN** physical Xbox/campaign. No agent receipt implied by publishing.
