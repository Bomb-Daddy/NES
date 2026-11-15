# GK1 EXEC0300 original-stock NMI seed-clock — source implementation / hostile QA

Based on authenticated complete **EXEC0299** native C11 game predecessor (the full EXEC0297 source is the identical current source after Windows EXEC0299 repaired the linkage). Adds and links `gk300_stock_nmi_seed_clock.c/h`, a real read-only source-bounded function in the production `Gk1App` API, and a CTest with 91 private original-stock assertions. All inherited app, source, saves, graphics/audio, controller, previous tests unchanged except necessary wiring.

Original USA ROM SHA metadata only, bytes must remain PRIVATE. Fixed ROM bank at CPU $C106-$C12C: NMI routine advances both independent 16-bit words `$84/$85` and `$86/$87` by +2 with carry if `$008A==0`; nonzero gate skips. Other power-on writers and timing not yet established; **never treat hypothetical histories, `RNG` return, or saved screenshots as a general first Spring**. Result explicitly marks no Spring unlock, no first-C289 seed proof. Two scenarios 14/27 validated in C11. No original ROM or copyrighted assets included.

Reproducer:
```
python3 stage300.py /path/to/full/execution_199
cmake -S /path/to/full/execution_199 -B /tmp/gk300_build -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/gk300_build -j 6
ctest --test-dir /tmp/gk300_build --output-on-failure -R GK1_300_
```
For genuine private ROM testing supply the original CSV and PRIVATE ROM to `test_gk300_stock_nmi_seed_clock CSV ROM`. Never upload either. The standalone `stage300.py` fails closed on wrong predecessor, corrupt patch/source, already applied staging and restores original files on failed write.

Evidence this execution: TESTED_PORT_LINUX native app build PASS; targeted 1/1 CTest PASS; private ROM 91 assertions PASS. The complete eligible Linux suite **236/236 PASS** with one source-archive-stale Win32 workflow gate explicitly excluded; the latter checks an old workflow copy embedded in EXEC0297, not the latest GitHub Actions workflow. The targeted new test and a private ROM 91-assertion run pass. No new Windows run claimed. NOT RUN Win32 CI for EXEC0300, physical Xbox or campaigns. Earlier EXEC0299 Win32 220/220 remains predecessor evidence.

STATUS: VERIFIED_STATIC source NMI mapping, TESTED_PORT_LINUX targeted, OBSERVED_RUNTIME_STOCK no new Mesen execution; first authentic playable Spring still OPEN. No Director signoff or third-party acknowledgement claimed.
