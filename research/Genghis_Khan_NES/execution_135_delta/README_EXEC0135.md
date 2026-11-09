# Genghis Khan I — EXEC0135 v1.35.0-dev1 — PPU source rollback

## Base and scope

Continues **the complete EXEC0134 native C11 Windows/Linux game source** and preserves all previous execution archives. No replacement game, no private original NES ROM or licensed data in public source. Approved Enhanced behavior remains isolated from Classic. The executable is an incomplete research candidate, **not a finished game**.

## Executable change

`gk103_ntsc_ppu.c` now stages PPUSTATUS reads in a local event-clock copy before committing. If a read falls in the explicitly unmodeled vblank boundary timing race, it rejects without consuming vblank, frame advance, pending NMI, read counters or an output byte. The terminal `race_gate` diagnostic remains available. PPUCTRL and PPUMASK now preflight their event advances, and unsupported rendering writes leave all modeled event state untouched, setting only `rendering_unmodeled` as a terminal gate. These corrections prevent fake source-trace side effects after a rejected hardware event. Existing original CPU/PRG/PPU source interpreter and input routes are preserved.

New `test_gk135_ppu_atomic.c` systematically exercises more than 450 success/refusal assertions, around the vblank race, frame-origin hypotheses, backward clocks, overflow, render rejection, replay and negative controls. With `GK1_ORIGINAL_STOCK_ROM` pointing to the PRIVATE original licensed cartridge, the bounded C11 source interpreter traces the genuine authenticated PRG bytes. This is **not** a native Mesen gameplay run, not source-authentic first-Spring generation, and does not establish Mesen's PPU power-on phase.

## Reproduce

```sh
cmake -S execution_135 -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
GK1_ORIGINAL_STOCK_ROM=/private/original.nes ctest --test-dir build --output-on-failure -j8
xvfb-run -a -s '-screen 0 1280x720x24' ./build/gk1_native_shell --smoke-frames 120
cpack --config build/CPackConfig.cmake -G DEB
```

## Evidence classifications

- **VERIFIED_STATIC:** Source/API, full predecessor retention, source hash gates and the independently checked original-ROM digest.
- **TESTED_PORT_LINUX:** Linux Release all 93/93; Debug bounded 92/92 with slow 128-way original PPU hypothesis excluded after a timed-out full pass; native Xvfb 1280x720 compiled and extracted Debian binary 120 frames each. Focused ASan/UBSan attempted; its full build hit a runtime timeout, so not certified.
- **OBSERVED_RUNTIME_STOCK, inherited only:** Fresh Spring1175 Mongol Gold430/Food208/Troops61/Rate121/3 orders; separately archived Mongol1175 Gold439/Food208/Troops61/Rate101/3 orders and archived World1206 Gold193/Food332/Troops116/Rate114/3 orders. No new genuine Mesen run. These captures must never be treated as general RNG constants.
- **NOT RUN:** Win32 binary, NSIS installer execution, physical Xbox controller, autonomous complete campaigns.

First playable original Spring independently generated from actual NES setup, RNG, 14/27 entire country state and three original orders is **still unimplemented**. The unknown-power-on source trace remains gated on RAM `$00F5` at PC `$8060`. Under the explicitly archived all-zero Mesen power-on hypothesis, the next unmodeled original source read is PPUSTATUS `$2002`. Enabling accurate PPU refusal does not resolve these original runtime source unknowns. Merchant presence/Trade, autonomous AI, diplomacy, dynasty, war aftermath, sound and graphical parity, real complete save/quit/cold-load and acceptance campaigns remain incomplete. This iteration must not be advertised as playable completion.
