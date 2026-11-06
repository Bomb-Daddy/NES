# GK1 EXEC0042 — Native research replay save/quit/cold-load (v1.00, 2026-11-06)

**Boundary:** This extends the actually rendering standalone 1280x720 Linux/X11 research shell from EXEC0041 with exclusive-save and load of the EXEC0039 **48-byte deterministic battle replay receipt**. This is NOT a full Genghis Khan campaign save, not NES SRAM and not the NES save format. The two campaign selections remain explicitly unavailable. Every accepted state is one of six approved combined Shared BP/Rest modified-ROM, controlled one-cell movement fixtures. No new native Mesen run, source breakpoint, original Windows build or physical Xbox hardware validation occurs here.

## Increment

- `gk1_replay_store.c/.h`: native filesystem implementation for Linux and Win32 with exclusive create, explicit flush/fsync, exact 48-byte complete-file read and stable 0039 CRC/replay validation; refuses overwrite and corrupt/extra/truncated input. Never creates directories or silently chooses a new slot.
- `gk1_app_state.c/.h`: *atomic* restore of authenticated-by-replay (not cryptographically authenticated) fixture session; rejects a forged in-memory state and retains the old app on failure. Maps the exact six approved fixtures back to the correct menu case.
- `gk1_native_shell.c`: research-only controller **X=save**, **Y=load** (Linux joystick button indices 2/3; Win32 XInput X/Y; Linux/Win keyboard debug S/L); title can load. Default file `./gk1_research_slot1.gkr` or user-set `GK1_REPLAY_FILE`. UI labels it RESEARCH, not a campaign save. An existing file is deliberately *never* overwritten. Win32 code authored, not Windows compiled.
- Linux CLI `--cold-export PATH CASE EVENTS` / `--cold-import PATH` permits two **different native app processes** to demonstrate genuine disk persistence, without X11; these invoke actual app input/session, serialization, disk adapter and restore, not fake JSON.

## Build and test (actual Linux executor)

```bash
cmake -S execution_042 -B build042 -DCMAKE_BUILD_TYPE=Debug
cmake --build build042 -j4
ctest --test-dir build042 --output-on-failure
cmake -S execution_042 -B build042_san -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build build042_san -j4
ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build042_san --output-on-failure
cmake -S execution_042 -B build042_rel -DCMAKE_BUILD_TYPE=Release
cmake --build build042_rel -j4
ctest --test-dir build042_rel --output-on-failure
# Native Linux separate-process cold load, in a fresh temporary directory:
mkdir -p /tmp/gk1-exec0042-mytest
build042/gk1_native_shell --cold-export /tmp/gk1-exec0042-mytest/slot.gkr 0 2
build042/gk1_native_shell --cold-import /tmp/gk1-exec0042-mytest/slot.gkr
# DO NOT remove or overwrite actual player saves. Above test uses a uniquely created test path.
# GUI native 1280x720 Xvfb smoke:
Xvfb :92 -screen 0 1280x720x24 -nolisten tcp &
DISPLAY=:92 build042/gk1_native_shell --smoke-frames 6
```

## Limits / next work

Evidence classification TESTED_PORT=Linux research desktop shell and disk replay only, not Windows. 6-frame GUI rendering; direct two-process export/import at case0 after B completion and friendly case3 before any extra B. Archive-only original RAM and native Mesen evidence from earlier EXEC remain unchanged. No inference that source was naturally reached anew. **Open: actual Mesen native friendly join source, Entire/Divide, multi-step, zero-unit terminal, two whole campaigns, actual 4-channel PCM render, HD/NES art, AI/dynasty, complete saves, Windows build/controller/installer acceptance.**
