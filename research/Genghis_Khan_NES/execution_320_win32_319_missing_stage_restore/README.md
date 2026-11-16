# Genghis Khan I â EXEC0320: restore actual Win32 original-stock CPU-trace arithmetic integration

**Verified parent:** EXEC0319 source and C11 math bridge in the existing 1280x720 native Genghis Khan I application. No duplicate game or source replacement.

## Actual bug / correction

At the November 16, 2026 GitHub Actions run `43786260248` (commit `4746b4e1c903a16ecaa8a21144847a2947d36643`), the `EXEC0319` stage FAILED. The committed `stage319.py`, `test_stage319.py`, and `SHA256_MANIFEST.json` were **empty Git blobs** despite the correct 3,874/2,829/1,842-byte source versions existing in the canonical Drive EXEC0319 ZIP. Python happily compiled/ran the empty scripts and exited zero. The subsequent honest native CMake CTest registry check failed because nothing was integrated. This is not an original ROM/gameplay failure.

The three missing files were restored on GitHub by exact bytes from the previously published Drive source archive; local Git hash-object matched the returned GitHub blob SHA for each.

## Executable new guard

`verify320.py` (17 hostile tests via `test_verify320.py`) verifies all six source/manifest files by SHA256, refuses missing/empty, mutated, NUL and invalid line-ending files; accepts exact CRLF Windows checkout only if its LF-canonicalized bytes match expected SHA. It normalizes authenticated checkout bytes to LF with rollback on error and checks **actual staged native `gk1_app_state` CMake linking** and original test registration exactly once, as well as matching C11 source bytes.

Run in repository:

```
python research/Genghis_Khan_NES/execution_320_win32_319_missing_stage_restore/test_verify320.py
python research/Genghis_Khan_NES/execution_320_win32_319_missing_stage_restore/verify320.py research/Genghis_Khan_NES/execution_319_cpu_trace_math_bridge
python research/Genghis_Khan_NES/execution_319_cpu_trace_math_bridge/test_stage319.py
python research/Genghis_Khan_NES/execution_319_cpu_trace_math_bridge/stage319.py current-gk1/execution_199
python research/Genghis_Khan_NES/execution_320_win32_319_missing_stage_restore/verify320.py research/Genghis_Khan_NES/execution_319_cpu_trace_math_bridge current-gk1/execution_199
```

This preserves the **existing** complete native game. The original stock NES ROM, screenshots and private original states are never published here. A claimed CPU trace is not proof of emulator execution, and first playable Spring, full campaigns and physical Xbox remain OPEN. Classification after local tests only: VERIFIED_STATIC source and regression, TESTED_HOST_LINUX_PYTHON 17/17, TESTED_HOST_LINUX_C11_COMPONENT inherited EXEC0319 46/46. Windows acceptance pending actual new GitHub CI; no new stock Mesen run.
