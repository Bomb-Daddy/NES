# GK1 EXEC0307 — physical Xbox/XInput diagnostic integrated into inherited native game

2026-11-15. Project: Genghis Khan I English NES -> existing standalone Windows/Linux native C11 game. Not ROTK II. Authority: original ROM recovered in EXEC0076; current published predecessor EXEC0306, Win32 223/223. This source is a separate, verifiable incremental hardware-validation route, **NOT** a substitute for authentic first-playable Spring.

## Implementation (not a memo-only prototype)

- `gk307_physical_pad_qa.c/.h` implements a fail-closed 8-step real-device input diagnostic. Initially requires a neutral sample, then **D-pad Up, D-pad Down, A, B, Start, Back/View, analog left full left, analog left full right** with release gates. Correct source: live XInputGetState (slot 0..3) only, not the game’s synthetic controller CLI. On disconnection, controller switch, wrong key or improperly held button, cannot claim success. 8/8 is a local diagnostic only, not a completed campaign or game achievement.
- `stage307.py` source-hash-gates the full inherited native `gk1_native_shell.c` and anchors the current CMake flow, stages new sources atomically with rollback and duplicate/malformed-line-ending refusal. Source intentionally does not read, modify, or redistribute the recovered original ROM or private captures.
- The actual WinMain will accept `--physical-xbox-qa`. Only live XInputGetState values enter this opt-in mode; A/B/Start do **not** mutate Gk1App game world or save files. It renders instructions in existing 1280x720 window `app.status`; close window exits 0 only after all 8 steps, else 10. Normal no-flag WinMain route remains unchanged. Original owner/private source auto-load is disabled in this diagnostic mode.
- The same C11 game CMake links the new library to the WinMain/X11 native shell and registers `GK1_307_PhysicalXboxDiagnosticInputFSM_NoHardwareClaim`; none of the tests simulate a physical controller claim.

## Reproduce from inherited source

1. Obtain *existing verified* inherited C11 source and extract the same `execution_199` plus sibling source directories; do not install a 307-only prototype. Correct previous normal shell file is 264746B SHA256 `ee3a8ed192670379f812554de0c3e5ba29311f3084a4336db8fa985be6e84df5` (normal LF; Git CRLF normalized); do not stage against any unknown shell.
2. `python3 test_stage307.py /path/to/execution_199`
3. `python3 stage307.py /path/to/execution_199`
4. `cmake -S /path/to/execution_199 -B build -DCMAKE_BUILD_TYPE=Release -DPython3_EXECUTABLE=/usr/bin/python3 && cmake --build build --parallel 8`
5. `ctest --test-dir build -R GK1_307_PhysicalXboxDiagnosticInputFSM_NoHardwareClaim --output-on-failure`
6. On Windows after native full CI compiled, **human operator physically connects an Xbox controller** and starts `gk1_native_shell.exe --physical-xbox-qa` to perform eight required inputs; verified hardware **NOT RUN** as of this handoff. Linux version does not expose Win32 CLI mode because XInput is Windows-specific.

## Actual QA and acceptance

`VERIFIED_STATIC`: 18 assertions of hostile source staging, injected I/O rollback, normal CRLF acceptance, malformed CR refusal, duplicate refusal, source corruption; exact expected postimage. The 69-assertion C11 FSM includes adversarial input and slot changes.

`TESTED_PORT_LINUX`: complete inherited execution_199 app Release configured, all native test binaries compiled, **236/236 eligible CTests PASS** with two existing environment-specific historical exclusions (`GK1_225_TwoProcessTitleControllerColdLoad` and `GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass`); new registered test **1/1**. Actual 1280x720 Xvfb native graphical smoke **12 frames PASS**. Caveat: this is the public EXEC0300 inherited base with EXEC0307 integrated; newer original-stock Linux source modules 302/304 are not part of this particular Linux full build. Preserve separate previous EXEC0304 Linux 238/238 and EXEC0306 Windows 223/223 evidence; do not conflate.

`TESTED_PORT_WIN32_CI`: **NOT RUN for EXEC0307** until new Windows GitHub workflow completes; inherited EXEC0306 223/223 is separate.

`OBSERVED_RUNTIME_STOCK_ARCHIVED` / `OBSERVED_RUNTIME_STOCK_FRESH_RESET`: only older genuine original ROM/Mesen evidence carried forward unchanged. No Mesen game run, no first-Spring parity claim.

`PHYSICAL_XBOX`, `COMPLETE_CAMPAIGNS`: **NOT RUN**. No release accepted. Automatic scheduled development must remain enabled until Director instruction or full accepted release.

**Unresolved highest-priority gameplay:** original stock first playable Spring with source-generated ruler/difficulty/ability/character RNG, all 14/27 authentic country records, real three orders; genuine AI, Trade scheduling, dynasty, diplomacy, battle aftermath and saves/cold-load. This physical QA increment does **not** satisfy any of those requirements.
