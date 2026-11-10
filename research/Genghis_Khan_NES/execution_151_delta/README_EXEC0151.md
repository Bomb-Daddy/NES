# EXEC0151 — opt-in even-frame pre-render boundary source trace

**This is not an NES emulator or a finished first playable Spring.** It adds a deliberately conditional, original-ROM-SHA-checked source research path on top of the preserved complete EXEC0150 C11 native application.

## Changed functionality

* Existing `gk103_ppu_advance` continues to enforce the original GK150 no-render boundary unless the caller explicitly invokes `gk151_ppu_assume_even_prerender` before rendering begins. This is an **even-parity assumption**, not a verified power-on property.
* The opt-in path permits **one** 2C02 pre-render boundary crossing on the assumption of no odd-frame skipped dot; it stops before the next possibly timing-sensitive VBlank edge. Rendered PPUSTATUS remains source-gated because sprite-zero/overflow are unknown. The previous strict refusal, CPU/RAM source guards, DMA assumptions and original course are preserved.
* `gk151_trace_original_even_prerender` exposes the conditional 6502 control-flow continuation. `gk151_scan_even_source` authenticates the original private stock USA ROM by SHA-256, evaluates 35 prior origins and distinguishes source progress from NES gameplay. The integrated `gk1_app_source_even_prerender` entry is read-only and locked behind the existing setup stage. `--source-even-prerender <private-ROM>` is a developer command in the original executable; no controller gameplay is unlocked.

## Private original-source findings — VERIFY STATIC, NOT Mesen

Using the actual original stock ROM SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`, the GK150 bound permits render-on for 18 of 35 reset-origin hypotheses, stopping at `$C71F` after 59,561 CPU cycles (first accepted origin). GK151's stricter *even-frame-only assumption* crosses the first pre-render boundary for those 18 origins; one first-origin scan stops at `$C5BE`, 60,039 CPU cycles, an additional 478 nominal CPU cycles. This endpoint is a PPUSTATUS timing gate, **not** a Spring command or verified original CPU history.

No new Mesen session, actual stock first Spring, true original setup RNG or hardware timing parity was proved. The inherited fresh versus archived 41-country observations remain separate. Original stock ROM, SRAM, snapshots and licensed assets stay private and are not included here.

## Reproduction

```
cmake -S execution_151 -B build151 -DCMAKE_BUILD_TYPE=Release
cmake --build build151 -j8
ctest --test-dir build151 -j8 --output-on-failure
GK1_ORIGINAL_STOCK_ROM=/private/original.nes ctest --test-dir build151 -R GK1_151 -V
build151/gk1_native_shell --source-even-prerender /private/original.nes
```

**STATUS:** VERIFIED_STATIC original source-bounded hypothesis, TESTED_PORT_LINUX for tests/build/GUI when logs attest them. OBSERVED_RUNTIME_STOCK from prior archived/fresh Mesen sessions **only**. NOT RUN Win32/NSIS, physical Xbox or actual complete campaigns. Next priority still reconstructing authentic all-14/27-country Spring setup and RNG, three playable original Classic orders, merchant scheduling and autonomous systems. Do not promote this source trace into gameplay certification.
