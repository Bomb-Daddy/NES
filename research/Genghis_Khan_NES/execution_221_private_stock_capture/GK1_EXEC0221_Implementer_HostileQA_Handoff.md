# Genghis Khan I — EXEC0221 Implementer and Hostile QA Handoff

**Date:** November 12, 2026  
**Inherited authority:** COMPLETE EXEC0220 C11 source/QA archive, 847 members; cumulative master v3.74; prior EXEC0218 original-stock Mesen script and C11 trace validator. **No replacement or reinvention of existing game.**

## Actual newly implemented code

`execution_199/packaging/gk221_original_stock_capture.py` is an executable, private-only **MesenCE capture orchestrator**: exact SHA-256 gated original USA stock ROM `ada7066...`, matching MesenCE binary `e777af...` and MesenCore.so `0668f...`; private chmod-0700 output directory; one-use trace/receipt names; isolated `$HOME/.config/MesenCE` settings; 125/300-frame inherited original reset input sequence in EXEC0218 Lua; `xvfb-run` process with bounded timeout and verified **process exit 0**; subsequent validation using the already-integrated native C11 `--audit-private-stock-spring-write-trace`; owner-private trace SHA and receipt. **If executable/core absent or different, it refuses to call the evidence runtime.** The runner neither uploads nor copies raw gameplay to public code. No optional hash override exists.

`execution_199/packaging/test_gk221_original_stock_capture.py` executes **12** adversarial tests: exact-hash refusal, nonzero exit, timeout, missing trace, native-validator refusal and false-positive-looking output, private symlink rejection, replayed output rejection, illegal timeouts, ROM/core tampering, and a simulated happy path explicitly **not considered NES observation**. These run as a NEW CTest in the complete native app build. Changed `gk1_app_state.c` corrects the real Classic labor planner's misleading `X TO UNDO` message to `LB UNDO`; X remains SAVE. Updated CMakeLists registers the new runner tests.

## Real test evidence from this execution

- Original private stock NES ROM SHA-256 independently rechecked **MATCH** against authorized source. Owner-private evidence archive independently rechecked **MATCH**.
- Existing original-stock native trace parser run against correct exact stock ROM with synthetic 14-stage fixture: **PASS**, expressly **synthetic parser fixture only**.
- New Python runner: **12/12 tests PASS**; exact emulator/core prerequisite probe refused missing `/mnt/data/gk1_gap0063/runtime/Mesen`, **no false original observation receipt**.
- Linux Release **187/187 CTest PASS**; Linux Debug **187/187 CTest PASS**; independent full ZIP extract/build **187/187 PASS**; Python-only targeted CTest in ASan/UBSan-configured build **1/1 PASS** (Python itself not instrumented by ASan).
- Source reconstruction: inherited **847/847** archive members preserved, **845 byte-identical**, 2 existing paths edited, 2 Python source/test paths added, total 849; 4/4 patch replay paths matched byte-identically.
- Linux Debian native research package generated. **No Win32 build, NSIS execution, physical Xbox test, new Mesen execution, or full campaigns.**

## Exact owner-private next command (DO NOT run if hashes/prereqs differ)

```bash
python3 execution_199/packaging/gk221_original_stock_capture.py \
  --rom /PRIVATE/authorized-original-stock-USA.nes \
  --mesen /PRIVATE/matched-MesenCE/Mesen \
  --core /PRIVATE/matched-MesenCE/MesenCore.so \
  --lua execution_199/GK1_EXEC0218_Private_Mesen_Stock_Fresh_Write_Recorder.lua \
  --native ./build/gk1_native_shell \
  --private /PRIVATE/NEW_EMPTY_EXEC0221_FRESH_TRACE
```

This command is **not** proof it has been executed. The actual matched MesenCE executable and core were **not mounted in this runtime** despite the authorized original ROM being successfully recovered; the runner refused, as designed. Earlier original-stock WorkRAM and archived-restored SaveRAM observations remain differentiated, with source-authentic static facts not promoted to runtime. An original Mesen trace, if obtained, must be independently validated against CPU PC, PRG physical mapper bank, RNG `$0084–$0089`, country writes and command-menu timing. The fresh WorkRAM workflow does **not** automatically verify archived SaveRAM.

## Remaining Director release gates

The application still cannot independently **generate** the original first playable Spring for 14/27 countries, player/difficulty/ruler RNG and three seasonal orders. Merchant presence/offers, original Trade player menu, autonomous AI, terminal battle aftermath, diplomacy, dynasty, original art/sound and full campaigns remain OPEN. The Linux research installer is **not a Windows release**. Build Win32 and NSIS on Windows with an actual physical Xbox controller and replay full campaigns before certification. Do not hardcode one observed Rate/Gold screenshot.

**QA classes:** `VERIFIED_STATIC` source/provenance, `OBSERVED_RUNTIME_STOCK` historical original records only (no new original execution), `TESTED_PORT_LINUX` native build/tests only, `NOT RUN` Win32/physical Xbox/full campaigns. Original ROM and licensed Mesen evidence remain PRIVATE. Publication is not agent acknowledgement or Director acceptance. Keep hourly execution enabled.
