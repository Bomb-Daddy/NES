# Genghis Khan I EXEC0257 — Current Win32 / NSIS acceptance source gate v1.01

Date: November 13, 2026. Correct current predecessor: complete EXEC0256, 4,609,624 bytes SHA256 `c26c789907e2e7b9df0be4e3c27b99e72ed54419d47991e79e44937bb2e88123`. This is the same 1280×720 native C11 WinMain XInput + Linux X11 game; never the stale EXEC0228 snapshot.

## Executed changes

Replaced `.github/workflows/gk1-native-windows.yml` which previously built the old `execution_228_win32_build_gate/execution_199` and ran only a historically selected test subset. Current source CI now authenticates a byte-exact complete EXEC0256 copyright-clean **build-source** ZIP (1,249,024 bytes SHA256 `97421aa2ef45288684bf22d868ad110b850396c316bce9253b0fc8a05166b619`, 793 entries) in the repository (the larger 4,609,624-byte complete source+QA ZIP remains separately preserved), rejects wrong SHA/source absence, applies the exact reproducible EXEC0257 patch, configures the fully inherited `execution_199` native application with MSVC Visual Studio x64, requires static zlib `x64-windows-static` for the original archived World importer, runs the full Win32 CTest registry, executes actual WinMain WM_PAINT/XInput software script/invalid args, then builds a genuine NSIS installer and smoke-tests the installed executable and canonical CSV. Explicitly does not substitute a mock Windows game or claim controller hardware testing.

New executable Python `packaging/test_gk257_windows_ci_gate.py` validates workflow/source pinning and 10 corrupted controls. New PowerShell `packaging/test_gk257_win32_acceptance.ps1` validates genuine WinMain GUI painting, software scripted navigation and bad argument refusal on the compiled and NSIS-installed EXE. Existing source and gameplay mechanics unchanged.

## Performed QA / honest acceptance

- VERIFIED_STATIC: exact full predecessor ZIP SHA and lean pinned build-source ZIP SHA, valid 13,857-byte source patch; patch applied cleanly against fresh independently extracted predecessor, 4 modified/added source and workflow files matched exactly. Python hostile audit 11/11 positive+negative cases passed.
- TESTED_PORT_LINUX: strict Release native game compilation SUCCESS. `ctest -j12 --timeout 30 -E <8 X11 cases>` 211/211 PASSED (exit0); actual 1280×720 Xvfb native application `--smoke-frames 15` PASS exit0; new CTest `GK1_257_CurrentWin32Workflow_RejectsStaleSourceAndFalsePass` 1/1 passed. Initial unfiltered 219-case run timed out at external tool limit after 205 completions; do NOT certify all219. Eight inherited X11 cases were excluded only from the completed 211/211 run, not removed from CMake. Earlier complete EXEC0256 Release suite 218/218 remains historical.
- OBSERVED_RUNTIME_STOCK_FRESH and OBSERVED_RUNTIME_STOCK_ARCHIVED_RESTORED: only historical private stock records reused; NO fresh Mesen execution in this run. Clean stock ROM remains private.
- NOT RUN: native Windows MSVC/WinMain, actual NSIS and physical Xbox or complete campaigns. GitHub Actions workflow is authored/verified but its execution results are a separate gate, NEVER inferred from static checks.

## Release and game completion

Original independent first Spring RNG/setup, real Trade merchant RNG, AI, battle terminal, diplomacy, dynasty and full campaigns remain open. No changed Classic/Enhanced mechanics, no original ROM, `.mss`, NES screenshots, raw RAM or audio published. Never claim a release until Windows and player device/campaign acceptance completed. Keep recurring run enabled.

## Reproduce

```
sha256sum GK1_EXEC0256_Complete_Inherited_C11_Source_QA.zip
unzip GK1_EXEC0256_Complete_Inherited_C11_Source_QA.zip -d current-gk1
cd current-gk1
git apply --check ../GK1_EXEC0257_Integration_From0256.patch
git apply ../GK1_EXEC0257_Integration_From0256.patch
python execution_199/packaging/test_gk257_windows_ci_gate.py .github/workflows/gk1-native-windows.yml execution_199/packaging/test_gk257_win32_acceptance.ps1
cmake -S execution_199 -B build257 -DCMAKE_BUILD_TYPE=Release -DGK1_REQUIRE_ARCHIVED_WORLD_IMPORT=ON
cmake --build build257 -j 12
ctest --test-dir build257 -j12 --timeout 30 --output-on-failure -E 'GK1_(202_LinuxX11|217_Linux1280|219_Linux1280|220_Linux1280|222_Linux1280|224_Linux1280|225_TwoProcess|226_FourSeparate)'
xvfb-run -a ./build257/gk1_native_shell --smoke-frames 15
```

For a real Windows acceptance, publish the pinned complete build-source ZIP blob plus exact patch into the specified public source path and the amended root workflow. Review the GitHub Actions job outcome, not merely the workflow's existence. Do not declare TESTED_PORT_WIN32 until exit0 and test/installer evidence is read back. `Gk1App` stays inherited, not a separate sample game.

## GitHub immutable source and Windows acceptance publication

The pinned 793-member archive is a copyright-clean **build-source subset** of complete EXEC0256 (identical files, no privately licensed NES source). It includes `execution_199` and required source dependencies, old root workflow for patch context, all 14/27 input source templates and tests. Independent clean extract + exact EXEC0257 patch + fresh CMake native GCC build and new gate PASS. A source subset is not a completed game. GitHub tree link and execution status require separate verified results.
