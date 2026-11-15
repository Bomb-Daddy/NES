# GK1 EXEC0298 — Win32 integration of authenticated stock reset and four-writer source

2026-11-15. Predecessor: full copyright-clean native C11 EXEC0297, NOT EXEC0062. This change stages the established EXEC0296 and EXEC0297 source modules, guarded app entry points, and their CTests into the **same** native Win32/XInput game after EXEC0295. It is a **Windows integration increment**, not an original NES first-Spring implementation.

`stage298.py` verifies SHA-256 preimages for the inherited compiled Win32 C11 app/header, source SHA-256 for six upstream public files, and the predecessor CMake/compiled-source gate. It makes one-time, fail-closed changes to app/header, native CMake source list, two regression registrations and current Windows source gate. It preserves existing Classic/Enhanced separation, saves, 14/27 pre-setup country records and all prior mechanics. Windows Git CRLF is normalized only as CRLF pairs before checks; lone CR, corruption, missing files and duplicate integrations fail prior to writes. The source does **not** import or publish any ROM, private Mesen state, screenshots, audio or copyrighted resources.

## Reproduction from real predecessor

After Windows source staging through EXEC0295, before compiling the native game:

```powershell
$env:GK298_PREDECESSOR_DIR='current-gk1/execution_199'
python research/Genghis_Khan_NES/execution_298_win32_stock_reset_source/test_stage298.py
python research/Genghis_Khan_NES/execution_298_win32_stock_reset_source/stage298.py current-gk1/execution_199
cmake -S current-gk1/execution_199 -B current-gk1/build-win -A x64
cmake --build current-gk1/build-win --config Release
ctest --test-dir current-gk1/build-win -C Release --output-on-failure
```

The existing Windows workflow must run the stage before compile, smoke tests and installer. It must not stage earlier Win32 files after this patch.

## Evidence and limits

Local Linux rebuild of the identical inherited app code: 227/227 eligible CTests passed after full build; targeted 296/297 CTests 2/2 passed. Authenticated private original ROM inputs were exercised locally only: EXEC0296 88 C11 assertions, EXEC0297 89 assertions. The full new 13-case stage hostile suite passed on copied *actual EXEC0295 postimage* source. Exact staged C11/H/CMake SHA matches the complete EXEC0297 inheritance. A GitHub-hosted Windows run, NSIS installer validation, physical Xbox controller and complete campaigns must be separately reported as RUN/NOT RUN according to evidence. Previously completed EXEC0295 CI218 is **prior-version evidence only**.

**Authentic first Spring remains OPEN.** The original ROM's four reset initializer bytes $84..$87 = 04,D2,04,D2 do not establish the later first `$C289` seed: 948 observed writes precede the draw and $88/$89 origin remains unknown. Never hardcode Gold/Rate from one screenshot, bypass PRE_SETUP, or unlock three orders based solely on this source proof.