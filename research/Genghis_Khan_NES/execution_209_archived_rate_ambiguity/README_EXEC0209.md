# GK1 EXEC0209 — Original archived Spring Rate RNG ambiguity proof (development candidate)

**Source inherited:** complete EXEC0208 native C11 application, unchanged beyond six integration paths. No original ROM, Mesen savestate, raw country data or licensed art is part of this package. This is an authentic-stock *retrospective arithmetic audit*, not a new NES playthrough or independently initialized game.

`gk209_archived_rate_ambiguity.c/.h` links to the application's exact full-source Mesen CPU state authentication (GK208) and original 48-bit inverse PRNG (GK138). The public deterministic command is:

```
gk1_native_shell --audit-stock-archived-rate-ambiguity PRIVATE_ORIGINAL_ROM PRIVATE_MESEN_MSS mongol1175 4096
gk1_native_shell --audit-stock-archived-rate-ambiguity PRIVATE_ORIGINAL_ROM PRIVATE_MESEN_MSS world1206 4096
```

The source verifies the entire original USA ROM and the complete distinct archived Mesen state, ensures country bank was SaveRAM, checks the archived CPU seed is on an odd post-draw boundary, and searches 4096 prior *possible* draws for each of the five difficulty formulas. The result is an ambiguity count, **never a selected causal draw or claimed actual difficulty**. Actual CPU seed writes, event RNG call order and first Spring seed remain unknown.

**Original archived Spring 1175 Mongol, rate 101**: matches over 4096 possible prior draws, difficulty 1–5 respectively: **45, 31, 28, 42, 29**. **World Spring 1206, rate 114**: **46, 39, 40, 24, 38**. All five difficulties have many mathematical matches. These are NOT claims of source-executed calls. Original controller choices and fresh reset are independent and are NOT part of this archive-derived scan. This underdetermination is evidence against selecting a convenient apparent random draw and hardcoding first Spring.

**VERIFIED_STATIC:** source, archived-Mesen and ROM identity gates, 5×4096 inversion arithmetic per archived profile, deterministic outcome and adversarial rollback. **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** prior archived Mongol/World original Mesen sessions are authenticated inputs; **no new emulator execution**. **OBSERVED_RUNTIME_STOCK_FRESH_RESET:** separate prior fresh WorkRAM capture preserved, not scanned or interpreted as SaveRAM. **TESTED_PORT_LINUX:** source integration, Release/Debug/independent builds, executable CLI, regression/negative tests, native Linux/Xvfb and Debian extraction. **NOT RUN:** native Win32, NSIS, physical Xbox, full campaigns, live Mesen CPU logging.

**Unfinished priority:** run authenticated Mesen stock from fresh reset with CPU memory-write watch on `$0084–$0089`, instruction/mapping provenance and real setup/difficulty/controller chronology; then implement the *dynamic* 14/27-country Spring in the existing app. The current code **does not grant a playable original Spring via pre-setup bypass**, and does not pretend that the 101/114/121 observations are constants.
