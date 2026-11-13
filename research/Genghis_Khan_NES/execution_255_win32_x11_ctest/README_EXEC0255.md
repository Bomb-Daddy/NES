# GK1 EXEC0255 — implementer and hostile-QA handoff — 2026-11-13

## Baseline and preservation

The baseline is the **COMPLETE** EXEC0254 source ZIP, SHA-256 `b714f55fe52fbd58d5bedae96f7a407799f9e37df698ab95c38704ccb1c283ca`, 995 members. Verified newer execution search: Google Drive EXEC0255 returned no files; public GitHub `research/Genghis_Khan_NES` last numbered directory was EXEC0254. Do not use old EXEC0062/EXEC0076. All unrelated 994 previous ZIP entries are inherited byte-identical; the CMake file is the single modified inherited member and one new Python executable QA source file is added. No original ROM, archived RAM, Mesen states or licensed graphics/audio are in the package.

## Actual reproducible change

**Defect A — Native Win32 test registration:** existing EXEC0225/0226 CMake unconditionally registered Python scripts that require `xvfb-run` and X11. These test registrations should not appear in Win32 CI and can cause an unrelated Windows game-build to fail. EXEC0255 encloses each registration in `if(UNIX AND NOT APPLE AND Python3_Interpreter_FOUND AND GK224_XVFB_RUN)`. The separately linked C11 `.gk24` tests remain registered for all platforms. This is a verified CMake source correction, **NOT an actual Win32 CI execution**.

**Defect B — intermittent GUI test scheduling:** the preceding EXEC0254 unfiltered Linux CTest attempt excluded three Python multi-X11 tests after hangs. The **same original EXEC0254 app binary** independently passed these three tests when run one at a time (4.76s/6.67s/8.74s). The new CMake gives all eight actual Linux Xvfb tests `RUN_SERIAL TRUE`, shared `RESOURCE_LOCK GK1_EXCLUSIVE_XVFB_GUI`, and `TIMEOUT 30`. Non-X11 C11 tests remain parallelized. This is a port-level QA infrastructure change, not NES controller/original gameplay implementation.

A new executable Python QA test `GK1_255_GeneratedX11Registry_WindowsExclusion_HostileQA` checks the **generated** CTest JSON registry for all eight tests and verifies the OS-dependent registration source guard, the independent existing C11 receipt check, resource lock, seriality and bounded timeout. It intentionally corrupts the guarded source and the generated registry (removed properties, forced parallel scheduling) and demands refusal in all three cases. It passed.

## Actual execution, honest classification

- **VERIFIED_STATIC:** Exact baseline ZIP SHA, deterministic unified patch, new CMake and Python SHA, reconstruction and poisoned baseline rejection. No original ROM/CRC/raw evidence in public source.
- **TESTED_PORT_LINUX:** Native Release C11 app compiled. Full **217/217** CTests passed, `ctest --test-dir build -j8 --timeout 15 --output-on-failure`, 34.43s. Eight X11/Xvfb tests ran serially; the three previously excluded multi-process tests all passed in this full run. A prior control configuration with RESOURCE_LOCK only also passed its 216/216 suite but showed unstable GUI scheduling; the final run uses RUN_SERIAL + lock. New QA `GK1_255` passed, including 3 hostile mutations. No private-source positive gameplay test was rerun.
- **OBSERVED_RUNTIME_STOCK_FRESH:** historical original WorkRAM evidence retained; **no new Mesen run**.
- **OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED:** historical original SaveRAM evidence retained; **no new Mesen run**.
- **NOT RUN:** EXEC0255 native Win32/MSVC/NSIS, physical Xbox controller, complete campaigns. No assertion of Windows correctness from CMake guard alone.

## Reproduction commands

```
sha256sum GK1_EXEC0254_Complete_Inherited_C11_Source_QA.zip
unzip -q GK1_EXEC0254_Complete_Inherited_C11_Source_QA.zip -d inherited
cd inherited
patch -p1 < ../GK1_EXEC0255_Integration_From0254.patch
sha256sum execution_199/CMakeLists.txt execution_199/packaging/test_gk255_x11_ci_gate.py
cmake -S execution_199 -B build -DCMAKE_BUILD_TYPE=Release -DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON
cmake --build build -j12
ctest --test-dir build -j8 --timeout 15 --output-on-failure
```

Compare two changed SHA values to manifest; patch on a deliberately modified CMake or patch reapplied twice must be refused. Windows CI must independently configure/build/test the full **latest** source and installed NSIS candidate; prior EXEC0243/0244 MSVC CI does not certify EXEC0255.

## Critical gameplay blocker, next execution

Original USA NES stock ROM is already recovered and privately authorized SHA-256 `ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3`; NEVER claim ROM unavailable. No independent source-accurate first Spring generation from original setup/ruler/difficulty/character RNG has been achieved. Distinguish three authentic original inputs: fresh Mongol WorkRAM; archived Mongol SaveRAM; archived World SaveRAM. Avoid one-case Gold/Rate constants and wrong bank. Prioritize original-ROM source tracing toward independent Spring, then source-parity strategic commands/season transitions, merchant presence/RNG, AI, battle/diplomacy/dynasty, sound/art, Windows and physical controller full campaigns. Keep the existing hourly job enabled until explicit Director stop or accepted complete release. Publication is not another agent's read or execution acknowledgement.
