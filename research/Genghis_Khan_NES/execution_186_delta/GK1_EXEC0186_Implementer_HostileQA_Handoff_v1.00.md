# GK1 EXEC0186 — Implementer and Hostile QA Handoff — 2026-11-11

## Production source inheritance
Direct latest COMPLETE source EXEC0185, recovered from Collaboration Drive file `1p_T2Q2KCVoraO4-cdPAfIgnnx0khgK9s` (not superseded EXEC0076). 39,082 exact historical ZIP entries preserved by name, CRC and size; new historical ZIP 39,667 entries, including 585 new current `execution_186/` source and test files. 14 actual changed/new paths; complete 585/585 source SHA matching after independent `patch -p1` replay onto the inherited `execution_185` directory. Version `1.86.0-dev1`, in native title, CLI, package/desktop.

## Actual C11 changes
* Extended existing `gk181_bank_seed_trace.c/.h` in same `gk1_app_state` library with a deduplicated source-decode inventory of reachable **JSR $C289** instruction addresses, fixed bank15 and caller-explicit MMC1 lower bank0..14, with RESET/NMI/IRQ source root mask; hard limit 512 and overflow refusal. Existing mapper-write and indirect-flow uncertainty boundaries preserved. The decoder does **not** claim executing any instruction in NES/Mesen.
* Added source-identity-gated `gk1_native_shell --audit-original-rng-calls PRIVATE_ROM` CLI which accepts only SHA256 ada7066d9e71a81310fff78bc7037126798018a69dc2b92382c3cbdcd9b243b3, nonzero exit201 for invalid ROM, bank hypotheses separately. No original bytes or RAM saved in published files.
* Added `test_gk186_original_rng_call_roots.c` with positive synthetic static RESET/NMI root and fixed/low mappings, alternate bank, unreachable false-byte pattern, mutation rollback, authentic original hash-gated stock bank sweeps. CMake and Linux Debian manifest updated.

## New result — independently derived negative
With EXACT private original USA NES ROM, **zero conditionally reachable direct `JSR $C289`** under every one of 15 fixed-bank + hypothetical low-bank mappings. This rules out a direct `JSR $C289` on paths resolved by THIS bounded source decoder only. It DOES NOT imply no RNG calls: indirect jumps/mapper chronology/unknown-RAM and emulator runtime have not been resolved, and the known $EF85 dispatcher is a different mechanism. Do not treat an absent static direct call as original NES runtime absence.

## Verification
**VERIFIED_STATIC**: authenticated ROM SHA (private, not published), exact 585/585 patch SHA replay, 39,082 historical preserved CRC/size. Synthetic and private-case 62 assertions PASS. Incorrect-ROM gate exit201. Genuine source bank sweep 0 direct calls, NOT original NES runtime.

**OBSERVED_RUNTIME_STOCK_INHERITED** (not rerun): original fresh WorkRAM Mongol Spring1175 Gold430 Food208 Troops61 Rate121 orders3; separate archived SaveRAM Mongol Spring1175 Gold439 Food208 Troops61 Rate101 orders3; archived World1206 Gold193 Food332 Troops116 Rate114 orders3. No hardcoded universal values.

**TESTED_PORT_LINUX**: full Release149/149 CTest PASS, full Debug149/149 PASS, focused ASan+UBSan 62 source-authenticated assertions PASS; production native X11/Xvfb 120 frames at 1280×720 PASS; Debian extracted executable Xvfb 120 frames PASS; true Debian package version1.86.0-dev1. No GPU or physical controller claim. Clean-source SHA replay 585/585; independent replay compile/test must be reported separately if run.

**NOT RUN**: new original Mesen live session; Windows Win32 executable, Windows NSIS installer, physical Xbox controller; full Mongol/World campaign.

## Critical original milestone is STILL UNFINISHED
Authentic source-owned 14/27 first playable Spring, player setup/difficulty/ability/RNG, three actual Classic commands (still VIEW/PASS), merchant presence and Trade RNG, autonomous AI, dynasty daughter ages6/7/8, diplomacy, war aftermath, media, reliable full campaign save/load and complete playthroughs. Do not claim accepted full game or external implementer ACK. Preserve private original ROM, licensed assets and Mesen checkpoints in private Genghis folder only.

## Exact reproducible commands
```sh
cmake -S execution_186 -B build186 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build186 -j8
ctest --test-dir build186 --output-on-failure
# static authenticity CLI (OWNER PRIVATE: never upload original ROM)
./build186/gk1_native_shell --audit-original-rng-calls /private/original-stock.nes
./build186/test_gk186_original_rng_call_roots /private/original-stock.nes
# independent integration: copy complete EXEC0185/execution_185, then patch -p1 from parent
patch -p1 < GK1_EXEC0186_Complete_Integration_From0185.patch
```

## Integrity
* `GK1_EXEC0186_Complete_Historical_C11_Source_v1.00.zip`: 61717625 bytes; SHA256 `9ffc53fe2e58f921dc3986ee7a646073408cf47039e3c6faab72354a42414258`
* `GK1_EXEC0186_Linux_Research_Installer_1.86.0-dev1_amd64.deb`: 300354 bytes; SHA256 `bc0a647b1e2923f23b754cdb3b32399dd81182011b5c1ad2a6e0f86dc2600585`
* `GK1_EXEC0186_Copyright_Clean_Source_Delta_v1.00.zip`: 107306 bytes; SHA256 `b00f58ba3d5d4fdb42cf34bf5e8966a244f20e85900050c970d65c41895061c6`
* `GK1_EXEC0186_Complete_Integration_From0185.patch`: 32680 bytes; SHA256 `557acd43b076929a5f6fc1161c98927c00d29442f6a6cde81af2d6f05ac287c3`
* `GK1_EXEC0186_Source_SHA256_Manifest.json`: 56070 bytes; SHA256 `def8a9265ad7c88c4a367ef6794fcd8db46a6f40d2fe7673c824c50152a24c31`

Changed/new: CMakeLists.txt, README_EXEC0186.md, gk181_bank_seed_trace.c, gk181_bank_seed_trace.h, gk1_native_shell.c, packaging/gk1-native-research.desktop, packaging/test_gk176_version_contract.py, packaging/test_gk179_version_contract.py, packaging/test_gk181_version_contract.py, packaging/test_gk182_version_contract.py, packaging/test_gk183_version_contract.py, packaging/test_gk184_version_contract.py, packaging/test_package_contract.py, test_gk186_original_rng_call_roots.c
