# GK1 EXEC0048 — Source-proven Divide integrated into inherited native desktop application

Scope: separate Genghis Khan I English NES-to-Windows project, not ROTK II or the original KOEI-inspired game. **No licensed ROM/checkpoint/SRAM/audio/screenshot data in this source tree.**

## Delivered code and evidence level

**TESTED_PORT_LINUX**: `execution_048/gk1_native_shell.c`, `gk1_app_state.c`, `gk1_divide_store.c`, tests and CMake now link the **exact imported EXEC0047 portable C11 source contract** with the **inherited 1280×720 X11/Win32 native shell and EXEC0046 two-step movement**. The source contract was created from **OBSERVED_RUNTIME_MOD EXEC0047** Mesen results; this increment does NOT make a new Mesen gameplay claim. All earlier original-Stock/Classic research and approved Enhanced modified-ROM profiles remain distinct. The app has a fourth title-menu option, source-bounded controller quantity selection (1/6/7/8 only), selected destination, command commit, zero-soldier 'bit the dust' message, two-stage B handling and completed word, visible source & slot3 records, Mobility and RNG, and exclusive-create 64-byte replay saves with cold-load event validation. Existing previous path/48-byte replay/snapshot-view options remain operational.

Specific live native-ROM targets, all from **EXEC0047** on approved modified ROM SHA256 `4764d46359581be1e7d12bcb85bd375c84cd4497d1084d9d306a6e884afbe7c5`:

| Request q | Original source soldiers | Split slot3 at (8,7) | Source Mobility after completion | $613E completion | Status |
|---|---|---|---|---|---|
| 1 | 39 | 0 soldiers, still records position | 10 | 1 | 2 B: dismiss death, finish |
| 6 | 34 | 0 soldiers, still records position | 10 | 1 | 2 B: dismiss death, finish |
| 7 | 33 | 1 soldier | 10 | 1 | 1 B: finish |
| 8 | 32 | 2 soldiers | 10 | 1 | 1 B: finish |

Before-target B is an observed refusal; second B restores quantity menu and leaves soldier values, completion and RNG unchanged. The source contract EXEC0047 returns DESTINATION after the second B, whereas the observed original UI returns the quantity screen; EXEC0048's native app resets an unmodified precommit contract after that dismissal. **This is a source-bounded UI integration choice**, not proof of a generalized battle cancellation mechanism. No arbitrary quantities, military AI, other unit slots, general Divide algorithms, stock-ROM Divide or terminal battle outcomes are implemented or claimed. `0 soldiers` in slot3 immediately after split is **not** proof of future cleanup or an end-of-battle trigger.

## QA executed

Run from this archive's root (all relative dependencies are packaged):

```sh
cmake -S execution_048 -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j4
ctest --test-dir build --output-on-failure

cmake -S execution_048 -B release -DCMAKE_BUILD_TYPE=Release
cmake --build release -j4
ctest --test-dir release --output-on-failure

cmake -S execution_048 -B asan -DCMAKE_BUILD_TYPE=Debug \
  '-DCMAKE_C_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer -O1 -g' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined'
cmake --build asan -j4
ctest --test-dir asan --output-on-failure

./build/gk1_native_shell --divide-export /tmp/gk48_q7.gkd 7 3
./build/gk1_native_shell --divide-import /tmp/gk48_q7.gkd
Xvfb :84 -screen 0 1280x720x24 &
DISPLAY=:84 ./build/gk1_native_shell --smoke-divide 120
```

The new `test_gk1_divide_in_app.c` drives the inherited app state through its actual routed input API for all four quantities, negative B/error and bad target controls, then tests encode/decode replays, a valid-CRC forged quantity, fixed approved-ROM identity, extra file byte, exclusive file creation and source-state tampering. A separate test feeds software-generated Xbox pad signals through `gk1_pad_step` and the same app code. Never interpret this as hardware Xbox acceptance. Native subprocess export/import shows cold-load of real disk files, not campaign saves. Xvfb smoke is real Linux X11 rendering, not Windows graphics acceptance. No Windows executable or installed Windows/Xbox device was run.

## Current limits / next work

Unrestricted tactical movement, general source selection/Divide, other troop types and terrain, zero-unit terminal handling, combat AI, strategic AI/economy/dynasty, original two playable campaigns, authentic pixel/sound pipeline, resilient full campaign saves, cross-platform installer, full Windows build and physical Xbox QA remain OPEN. Existing campaign references/user artifacts preserved unchanged.

All original historic `execution_046` and `execution_047` source files retained as separate directories; new changed files live under `execution_048` and are not overwriting history.
