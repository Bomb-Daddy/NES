# GK1 EXEC0074 — Authenticated archived Spring comparison recovered in combined native application

This incremental merge preserves the standalone 1280×720 C11 Genghis Khan I native game from EXEC0073 (itself incorporating EXEC0071's manual order budget and EXEC0072's battle Pass). It reconstructs the missing **read-only** EXEC0070 archived Spring comparison behavior because its originally tested full source remains inaccessible; **it is not a byte-for-byte recovery of the EXEC0070 source**. Neither the approved modified-ROM observations nor user campaigns are rewritten.

**Source boundary:** `GK1_1175_RAM_FILE` points to an optional 8192-byte private archived WorkRAM image with SHA-256 `dd22847140cc0397ab8f42e29f1832681628ed52bfa17e95041c288afc4ef773`. The archive was historically described as original, but no matching original NES ROM hash establishes its stock identity; evidence class is `ARCHIVED_SOURCE_CAPTURE_ROM_UNVERIFIED`. No private RAM data are shipped or embedded. Exactly 14 records of 83 bytes from NES WorkRAM offset `$0DCD` (`$6DCD` CPU base) are decoded into Gold, Food, Troops, Arms, Skill, Economy. These are **observations from a later 1175 checkpoint**, not the original first playable Spring, formula, or effects of initial setup.

Controller: complete normal Mongol setup draft until manual rules, then **A** enters archive screen, D-pad cycles 14 countries, **B** returns; **View** still opens order budget; **Start** still opens battle what-if and Pass. X/Y research saves disabled in archive screen. Missing, altered, shortened or oversized private RAM refuses the archive; the world and saved campaigns remain unchanged. Without a private archive the screen shows an explicit unavailable message; no fabricated figures.

Public QA and binary build commands:
```
cmake -S execution_074 -B build074 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build074 -j4
ctest --test-dir build074 --output-on-failure
build074/gk1_native_shell --spring-pad-check
```
Private optional QA; do NOT commit the source image:
```
build074/test_gk74_spring_archive /private/baseline.sram
build074/gk1_native_shell --spring-audit /private/baseline.sram
build074/gk1_native_shell --spring-pad-check /private/baseline.sram
GK1_1175_RAM_FILE=/private/baseline.sram xvfb-run -a build074/gk1_native_shell --smoke-spring-archive 60
```

Evidence class: source-only public tests / private archive replay / native Linux port. There is **no** new stock NES run, implemented original Spring, gameplay diplomacy/AI/seasonal processing, Windows/Xbox validation, or playable full two-campaign release.
