# EXEC0150 — bounded rendered-PPU first-window source continuation

Build upon complete historical 1.49.0-dev1 C11 original-game application. This
implements a **new opt-in, explicit hardware hypothesis** in the existing
6502 source emulator. Previous default PPUMASK rejection remains unchanged.
The hypothesis permits a `$2001` rendering-enable write within the remainder
of its current NTSC frame, but refuses before the next pre-render odd-frame
shortening could affect the CPU timeline and refuses PPUSTATUS when rendered
sprite flags are unknowable. Repeated enable writes cannot extend the window,
failed clocks preserve prior state, and no original ROM data is distributed.

This is an enabling investigation of original NES startup; **it does not
implement first-Spring setup, renderer, real merchant RNG or full campaigns.**
Mesen parity, Windows and physical Xbox are not certified.

```
cmake -S execution_150 -B build150 -DCMAKE_BUILD_TYPE=Release
cmake --build build150 -j8
ctest --test-dir build150 --output-on-failure
GK1_ORIGINAL_STOCK_ROM=/private/rom.nes ctest --test-dir build150 -R GK1_150 -V
build150/gk1_native_shell --source-render-window /private/rom.nes
```
