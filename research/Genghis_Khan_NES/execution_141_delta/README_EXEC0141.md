# Genghis Khan I — EXEC0141 (1.41.0-dev1): source-gated 6502 dispatcher scan

This is the existing complete cumulative native C11 Linux X11/Win32-intended game, not a new stand-alone prototype. Inherits EXEC0140 source, all campaign profiles, original-country source templates, prior controller routes and save research. No claim of fully playable NES stock campaigns.

## New reproducible source investigation

`gk141_dispatch_scan.c/.h` integrates with the existing Gk1App in `gk1_app_source_dispatch_scan`. It inspects the exact owner-authorized 262,160-byte original USA NES ROM (SHA-256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3) **only on private systems**. It source-validates both the image it scans and setup/difficulty phase, uses bounded PRG bank addressing and an atomic refusal policy. The source emits no ROM bytes.

It records candidate `JSR $EF85` with following byte selector in the same bank, plus the 256 fixed-bank jump-table target addresses at CPU `$F788`/`$F888`. Authenticated stock *byte-pattern* candidates = 986 total; selector00=400, selectorFE=161, fixed-bank candidates=163, all jump-table target addresses >= $C000. **These are NOT proven executed instructions**: data bytes can match, and bank context, NMI, selector/dispatch chronology remain unresolved. The results do not establish original Spring RNG sequence, all-country startup transforms, or a legal three-order state. Do not hardcode captured Gold/Rate.

`test_gk141_dispatch_scan.c` checks synthetic positive/negative/in-bank boundary controls, immutability after errors, the in-app source gate and optional real-stock aggregate verification. The private ROM and saved states are never uploaded to public GitHub or the source package.

## Build / QA

From alongside the historical `execution_034`, `035`, `039`, `040`, `045`, `047`, `049` dependencies:

```
cmake -S execution_141 -B build141 -DCMAKE_BUILD_TYPE=Release
cmake --build build141 -j 8
ctest --test-dir build141 --output-on-failure -j 8
build141/test_gk141_dispatch_scan --private-stock /PRIVATE/PATH/TO/AUTHENTIC_ORIGINAL.nes
xvfb-run -a build141/gk1_native_shell --smoke-frames 120
cmake --build build141 --target package
```

The optional private test is local-only; it must never appear in public CI. The delivered Debian installer remains a Linux research candidate. Native Win32, NSIS, physical Xbox and complete campaigns NOT RUN. Original stock fresh-reset and archived-restored captures are inherited OBSERVED_RUNTIME_STOCK from earlier Mesen captures, not new Mesen gameplay in this run.
